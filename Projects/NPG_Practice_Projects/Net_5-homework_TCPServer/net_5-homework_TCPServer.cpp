#pragma comment(lib,"ws2_32")
#include<winSock2.h>
#include<stdlib.h>
#include<stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// cd E:\Git\20-2-NetworkProgramming\Projects\release_5\client
// Net_5-homework_TCPClient test_client.txt
// Net_5-homework_TCPClient suri.jpg
// Net_5-homework_TCPClient katana_dance.mp4

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
	int left = len;

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

int main(int argc, char* argv[])
{
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
	char buf[BUFSIZE + 1];
	int len;

	// ������ ������ �����͸� ���� ����
	char fileName[50];
	int nameLen;
	int dataSize;
	char* mode = (char*)"wb";	

	FILE* fp = fopen(fileName, "a+b");;
	int rest;

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// ���� �̸��� ���̸� �޴´�
		retval = recvn(client_sock, (char*)&nameLen, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}

		printf("������ �̸� ����: %d\n", nameLen);
		// ���� �̸��� �޴´�
		retval = recvn(client_sock, fileName, nameLen, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		// �����Ⱚ ����
		for (int i = nameLen; i < 50; ++i)
			fileName[nameLen] = '\0';
		printf("������ �̸�: %\s\n", fileName);


		// ������ ��ü ũ�⸦ �޴´�.
		int dataSize;
		retval = recvn(client_sock, (char*)&dataSize, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		printf("������ ũ��: %d\n", dataSize);
		rest = dataSize;

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
				err_display((char*)"recv()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ����
			if (fp == NULL) {
				fclose(fp);
				fp = fopen(fileName, "wb");
				fwrite(buf, 1, len, fp);
				fclose(fp);
				fp = fopen(fileName, "a+b");
			}
			else {
				fwrite(buf, 1, len, fp);
			}
			rest -= len;
			if (rest == 0)
				break;
			printf("[���۷�] %d / %d\n", rest,dataSize );
		}

		// closesocekt()
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);
	fclose(fp);

	// ���� ����
	WSACleanup();
	return 0;
}