#pragma comment(lib,"ws2_32")
#include<winsock.h>
#include<stdlib.h>
#include<windows.h>
#include<stdio.h>

#include"resource.h"
#include <commctrl.h>		// progressbar header

// cd E:\Git\NGP\20-2-NetworkProgramming\Projects\release_6\client
// Net_5-homework_TCPClient.exe test_client.txt
// Net_5-homework_TCPClient.exe suri.jpg
// Net_5-homework_TCPClient.exe katana_dance.mp4
// Net_5-homework_TCPClient.exe katana_dance2.mp4
// Net_5-homework_TCPClient.exe test_video2.mp4

#define SERVERPORT 9000
#define BUFSIZE 512

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ���� ��� �Լ�
void err_quit(char* msg);
void err_display(char* msg);
// ���� ��� ������ �Լ� 
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

int recvn(SOCKET s, char* buf, int len, int flags);

HINSTANCE hInst;		// �μ��Ͻ� �ڵ�
HWND hProgressBar[4];		// ���α׷����� ��Ʈ��
CRITICAL_SECTION cs;	// �Ӱ迵��

int clientCnt = 0;
int rate[4] = { 0 };

int recvn(SOCKET s, char* buf, int len, int flags);
void MakeNewFile(SOCKET client_sock, char fileName[50]);
void SaveFile(SOCKET client_sock, char fileName[50]);

int WINAPI WinMain(HINSTANCE hInstacne, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInst = hInstacne;
	InitializeCriticalSection(&cs);

	// ������ Ŭ���� ���
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstacne;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "MyWndClass";
	if (!RegisterClass(&wndclass)) return 1;

	// ������ ����
	HWND hWnd = CreateWindow("MyWndClass", "TCPS Server", WS_OVERLAPPEDWINDOW, 0, 0, 600, 200, NULL, NULL, hInstacne, NULL);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// �޼��� ����
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteCriticalSection(&cs);
	return msg.wParam;
}


// ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		for (int i = 0; i < 4; ++i) {
			hProgressBar[i] = CreateWindowEx(
				0, PROGRESS_CLASS, (LPSTR)NULL, WS_VISIBLE | WS_CHILD, 10, 10 * (i+3) + i * 15, 500, 15, hWnd, (HMENU)IDPB_PROGRESS_BAR, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			SendMessage(hProgressBar[i], PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // ����
			SendMessage(hProgressBar[i], PBM_SETPOS,rate[i],0 );	// �ʱⰪ
		}

		return 0;
	case WM_TIMER:
		return 0;
	case WM_SIZE:
		//MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	case WM_SETFOCUS:
		//SetFocus(hEdit);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		KillTimer(hWnd, IDT_PROGRESS_TIMER);

		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void err_quit(char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// TCP ���� ���� �κ�
DWORD WINAPI ServerMain(LPVOID arg) {
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));		// �ʱ�ȭ
	serveraddr.sin_family = AF_INET;					// �ּ�ü�� ����
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		// � IP �ּҷε� ���� �����ϵ��� 
	serveraddr.sin_port = htons(SERVERPORT);			// ��Ʈ��ȣ ����
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));		// bind
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			break;
		}

		// ������ ����
		clientCnt++;
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// ������ ������ �����͸� ���� ����
	char fileName[50];		// ���� �̸�

	// ���ο� ������ �����Ѵ�.
	MakeNewFile(client_sock, fileName);

	// ���۵Ǵ� �����͸� ���Ͽ� �����Ѵ�.
	SaveFile(client_sock, fileName);

	// closesocekt()
	closesocket(client_sock);

	return 0;
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;		// ������ ũ��

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			// ���н� SOCKET_ERROR�� �����Ѵ�.
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		// �о�� ������ ũ�� ��ŭ ���� �����Ϳ��� ����.
		left -= received;
		// �о�� ������ ũ�⸸ŭ ������ �����͸� �̵���Ų��.
		ptr += received;
	}

	// ������ ���� ����Ʈ �� / 0(����)�� �����Ѵ�.
	return (len - left);
}


void MakeNewFile(SOCKET client_sock, char fileName[50])
{
	int retval;
	int nameLen;

	// ���� �̸��� ���̸� �޴´�
	retval = recvn(client_sock, (char*)&nameLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return;
	}

	// ���� �̸��� �޴´�
	retval = recvn(client_sock, fileName, nameLen, 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return;
	}
	// �����Ⱚ ����
	for (int i = nameLen; i < 50; ++i)
		fileName[nameLen] = '\0';

	// ���� ����
	FILE* fp = fopen(fileName, "wb");
	fclose(fp);
}

void SaveFile(SOCKET client_sock, char fileName[50]) {
	int retval;
	int len;
	int left;
	int fileSize;
	char buf[BUFSIZE + 1];
	int client_id = clientCnt;

	FILE* fp = fopen(fileName, "a+b");
	if (fp == NULL) {
		printf("������ �������� �ʽ��ϴ�");
		return;
	}

	// ������ ��ü ũ�⸦ �޴´�.
	retval = recvn(client_sock, (char*)&fileSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return;
	}
	left = fileSize;


	// Ŭ���̾�Ʈ�� ������ ���
	while (1) {
		// ������ �ޱ�(��������)
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ������ �ޱ�(��������)
		retval = recvn(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_quit((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ����
		fwrite(buf, 1, len, fp);
		left -= len;

		// ���ŷ� ���
		rate[client_id - 1] = (int)((1 - (float)left / (float)fileSize) * 100);
		SendMessage(hProgressBar[client_id - 1], PBM_SETPOS, (UINT)rate[client_id-1],0);

		if (left <= 0)
			break;
	}
	fclose(fp);
}