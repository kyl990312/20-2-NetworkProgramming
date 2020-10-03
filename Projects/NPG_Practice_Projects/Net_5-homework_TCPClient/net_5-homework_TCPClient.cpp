#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>

// test
//E:\Git\20-2-NetworkProgramming\Projects\NPG_Practice_Projects\Release
// Net_5-homework_TCPClient.exe homeworkTest_5.txt

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 50

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

char* ReadFile(char* fileName) {
	FILE* fp = fopen(fileName,"r");
	if (fp == NULL) {
		printf("%s ������ �о�� �� �����ϴ�.", fileName);
		return NULL;
	}
	char fileData[500];
	for (int i = 0; ;++i) {
		char ch = fgetc(fp);
		if (ch == EOF) {
			printf("���� �б� �Ϸ�");
			fclose(fp);
			fileData[i] = '\0';
			return fileData;
		}
		fileData[i] = ch;
	}
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

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE];
	int len;

	// ������ ������ ���
	for (int i = 1; i < agrc; ++i) {
		// ������ �Է�(�ùķ��̼�)
		char* data = ReadFile(argv[i]);
		len = strlen(data);
		strncpy(buf, data, len);

		// ������ ������(��������)
		retval = send(sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}

		// ������ ������(��������)
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			printf("�������̰� ������");
			err_display((char*)"send()");
			break;
		}

		printf("[TCP Ŭ���̾�Ʈ] %d + %d����Ʈ�� ���½��ϴ�.\n", sizeof(int), retval);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}