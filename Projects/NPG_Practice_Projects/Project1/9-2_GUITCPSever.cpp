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

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 오류 출력 함수
void err_quit(char* msg);
void err_display(char* msg);
// 소켓 통신 스레드 함수 
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

int recvn(SOCKET s, char* buf, int len, int flags);

HINSTANCE hInst;		// 인서턴스 핸들
HWND hProgressBar[4];		// 프로그레스바 컨트롤
CRITICAL_SECTION cs;	// 임계영역

int clientCnt = 0;
int rate[4] = { 0 };

int recvn(SOCKET s, char* buf, int len, int flags);
void MakeNewFile(SOCKET client_sock, char fileName[50]);
void SaveFile(SOCKET client_sock, char fileName[50]);

int WINAPI WinMain(HINSTANCE hInstacne, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInst = hInstacne;
	InitializeCriticalSection(&cs);

	// 윈도우 클래스 등록
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

	// 윈도우 생성
	HWND hWnd = CreateWindow("MyWndClass", "TCPS Server", WS_OVERLAPPEDWINDOW, 0, 0, 600, 200, NULL, NULL, hInstacne, NULL);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// 메세지 루프
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteCriticalSection(&cs);
	return msg.wParam;
}


// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		for (int i = 0; i < 4; ++i) {
			hProgressBar[i] = CreateWindowEx(
				0, PROGRESS_CLASS, (LPSTR)NULL, WS_VISIBLE | WS_CHILD, 10, 10 * (i+3) + i * 15, 500, 15, hWnd, (HMENU)IDPB_PROGRESS_BAR, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			SendMessage(hProgressBar[i], PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // 범위
			SendMessage(hProgressBar[i], PBM_SETPOS,rate[i],0 );	// 초기값
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

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg) {
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));		// 초기화
	serveraddr.sin_family = AF_INET;					// 주소체계 지정
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		// 어떤 IP 주소로든 접근 가능하도록 
	serveraddr.sin_port = htons(SERVERPORT);			// 포트번호 지정
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));		// bind
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// 데이터 통신에 사용할 변수
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

		// 스레드 생성
		clientCnt++;
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// 저장할 파일의 데이터를 담을 변수
	char fileName[50];		// 파일 이름

	// 새로운 파일을 생성한다.
	MakeNewFile(client_sock, fileName);

	// 전송되는 데이터를 파일에 저장한다.
	SaveFile(client_sock, fileName);

	// closesocekt()
	closesocket(client_sock);

	return 0;
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;		// 데이터 크기

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			// 실패시 SOCKET_ERROR을 리턴한다.
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		// 읽어온 데이터 크기 만큼 남은 데이터에서 뺀다.
		left -= received;
		// 읽어온 데이터 크기만큼 버퍼의 포인터를 이동시킨다.
		ptr += received;
	}

	// 성공시 받은 바이트 수 / 0(종료)을 리턴한다.
	return (len - left);
}


void MakeNewFile(SOCKET client_sock, char fileName[50])
{
	int retval;
	int nameLen;

	// 파일 이름의 길이를 받는다
	retval = recvn(client_sock, (char*)&nameLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return;
	}

	// 파일 이름을 받는다
	retval = recvn(client_sock, fileName, nameLen, 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return;
	}
	// 쓰레기값 제거
	for (int i = nameLen; i < 50; ++i)
		fileName[nameLen] = '\0';

	// 파일 생성
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
		printf("파일이 존재하지 않습니다");
		return;
	}

	// 파일의 전체 크기를 받는다.
	retval = recvn(client_sock, (char*)&fileSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return;
	}
	left = fileSize;


	// 클라이언트와 데이터 통신
	while (1) {
		// 데이터 받기(고정길이)
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 데이터 받기(가변길이)
		retval = recvn(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_quit((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 저장
		fwrite(buf, 1, len, fp);
		left -= len;

		// 수신률 출력
		rate[client_id - 1] = (int)((1 - (float)left / (float)fileSize) * 100);
		SendMessage(hProgressBar[client_id - 1], PBM_SETPOS, (UINT)rate[client_id-1],0);

		if (left <= 0)
			break;
	}
	fclose(fp);
}