#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>

// test
//E:\Git\20-2-NetworkProgramming\Projects\NPG_Practice_Projects\Release
// Net_5-homework_TCPClient.exe homeworkTest_5.txt

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 512

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

int main(int agrc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"connect()");

	int nameLen = strlen(argv[1]);

	// ���� �̸��� ���̸� ������.
	retval = send(sock, (char*)&nameLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"send()");
	}

	// ������ �̸��� ������.
	retval = send(sock, argv[1], nameLen, 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"send()");
	}

	printf("[TCP Ŭ���̾�Ʈ] ������ �̸� ������ : %d + %d����Ʈ�� ���½��ϴ�.\n", sizeof(int), retval);
	
	// ������ �о�´�.
	int dataSize; 
	FILE* in = fopen(argv[1],"rb");
	fseek(in, 0, SEEK_END);
	dataSize = ftell(in);
	fseek(in, 0, SEEK_SET);		

	// ������ ��ü ũ�⸦ ������.
	retval = send(sock, (char*)&dataSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"send()");
	}

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE];
	int len;

	// ���� ����Ʈ 
	int rest = dataSize;

	// ������ ������ ���
	while (1) {
		// ���� ������ ����� ���Ѵ�.
		if (rest > BUFSIZE)
			len = BUFSIZE;
		else if (rest == 0)
			break;
		else
			len = rest;

		fseek(in, dataSize - rest, SEEK_SET);
		fread(buf, 1, len, in);

		// ������ ������(��������)
		retval = send(sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}

		// ������ ������(��������)
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}
		rest -= len;

		printf("[TCP Ŭ���̾�Ʈ] %d + %d����Ʈ�� ���½��ϴ�.\n",sizeof(int), retval);
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���ҽ��ϴ�. \n",rest);

	}

	// closesocket()
	closesocket(sock);

	fclose(in);

	// ���� ����
	WSACleanup();
	return 0;
}