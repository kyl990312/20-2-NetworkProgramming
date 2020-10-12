#pragma comment(lib,"ws2_32")
#include<winSock2.h>
#include<stdlib.h>
#include<stdio.h>

// cd E:\Git\NGP\20-2-NetworkProgramming\Projects
// Net_5-homework_TCPClient.exe test_client.txt
// Net_5-homework_TCPClient.exe suri.jpg
// Net_5-homework_TCPClient.exe katana_dance.mp4
// Net_5-homework_TCPClient.exe katana_dance2.mp4
// Net_5-homework_TCPClient.exe test_video2.mp4

#define SERVERPORT 9000
#define BUFSIZE 512

#define NOTICE_Y 20

int threadIds[64];			// � �������� ���α׷��� ������ �����ϱ� ����. id�� ���� Ŀ���� ��ġ�� ���Ѵ�.
int clientCnt = 0;

int noticeY = 1;

// ���� �Լ� ���� ��� �� ����
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


void MoveCursor(int y) {
	COORD pos = { 0,y }; //x, y ��ǥ ����
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); //Ŀ�� ����
}

void DrawProgressBar(char fileName[50],int total, int val, int y) {
	int drawVal = ((float)val / (float)total) * 20;			// '��' ����
	//printf("cursor y: %d\n", y);

	// Ŀ�� �̵�
	MoveCursor(y);

	// ���
	printf("[ %s ] ",fileName);
	for (int j = 0; j < drawVal; j++)
		printf("��");
	for (int j = 0; j < 20 - drawVal; ++j)
		printf("��");
	printf("\t%d%%\n", (int)(((float)val / (float)total) * 100));
}


void MakeNewFile(SOCKET client_sock, char fileName[50])
{
	int retval;
	int nameLen;

	// ���� �̸��� ���̸� �޴´�
	retval = recvn(client_sock, (char*)&nameLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		MoveCursor(NOTICE_Y+noticeY);
		noticeY++;
		err_display((char*)"recv()");
		return;
	}

	// ���� �̸��� �޴´�
	retval = recvn(client_sock, fileName, nameLen, 0);
	if (retval == SOCKET_ERROR) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
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

	FILE* fp = fopen(fileName, "a+b");
	if (fp == NULL) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
		printf("������ �������� �ʽ��ϴ�");
		return;
	}

	// ������ ��ü ũ�⸦ �޴´�.
	retval = recvn(client_sock, (char*)&fileSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
		err_display((char*)"recv()");
		return;
	}
	left = fileSize;

	threadIds[clientCnt++] = GetCurrentThreadId();

	// Ŭ���̾�Ʈ�� ������ ���
	while (1) {
		// ������ �ޱ�(��������)
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			MoveCursor(NOTICE_Y + noticeY);
			noticeY++;
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ������ �ޱ�(��������)
		retval = recvn(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			MoveCursor(NOTICE_Y + noticeY);
			noticeY++;
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ����
		fwrite(buf, 1, len, fp);
		left -= len;

		// ���α׷��� �� ���
		for (int i = 0; i < clientCnt; ++i) {
			if (GetCurrentThreadId() == threadIds[i]) {
				DrawProgressBar(fileName, fileSize, fileSize - left,i);
				break;
			}
		}		
	}
	fclose(fp);
}

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// ������ ������ �����͸� ���� ����
	char fileName[50];		// ���� �̸�

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	// ������ �Լ��� ���ϸ� ������ ��쿡�� ������ �ּ������� �����Ƿ� ������ ���� �ּ� ������ ��� ����� �ʿ�
	// ���� ������ ����ü�� ����� ���� IP �ּҿ� ���� ��Ʈ ��ȣ ����
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// ���ο� ������ �����Ѵ�.
	MakeNewFile(client_sock, fileName);

	// ���۵Ǵ� �����͸� ���Ͽ� �����Ѵ�.
	SaveFile(client_sock, fileName);

	// closesocekt()
	closesocket(client_sock);

	return 0;
}


int main(int argc, char* argv[])
{
	int retval;

	//MoveCursor(NOTICE_Y);
	//printf("Notice\n");

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
		err_quit((char*)"socket()");
	}

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));		// �ʱ�ȭ
	serveraddr.sin_family = AF_INET;					// �ּ�ü�� ����
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		// � IP �ּҷε� ���� �����ϵ��� 
	serveraddr.sin_port = htons(SERVERPORT);			// ��Ʈ��ȣ ����
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));		// bind
	if (retval == SOCKET_ERROR) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
		err_quit((char*)"bind()");
	}

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) { 
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
		err_quit((char*)"listen()");
	}

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
			MoveCursor(NOTICE_Y + noticeY);
			noticeY++;
			err_display((char*)"accept()");
			break;
		}
		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();

	//printf("client Cnt :%d\n", clientCnt);
	return 0;
}