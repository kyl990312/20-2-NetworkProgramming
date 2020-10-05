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

// 소켓 함수 오류 출력 후 종료
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

	// 윈속 초기화
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

	// 파일 이름의 길이를 보낸다.
	retval = send(sock, (char*)&nameLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"send()");
	}

	// 파일의 이름을 보낸다.
	retval = send(sock, argv[1], nameLen, 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"send()");
	}

	printf("[TCP 클라이언트] 파일의 이름 데이터 : %d + %d바이트를 보냈습니다.\n", sizeof(int), retval);
	
	// 파일을 읽어온다.
	int dataSize; 
	FILE* in = fopen(argv[1],"rb");
	fseek(in, 0, SEEK_END);
	dataSize = ftell(in);
	fseek(in, 0, SEEK_SET);		

	// 파일의 전체 크기를 보낸다.
	retval = send(sock, (char*)&dataSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"send()");
	}

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	int len;

	// 남은 바이트 
	int rest = dataSize;

	// 서버와 데이터 통신
	while (1) {
		// 보낼 데이터 사이즈를 구한다.
		if (rest > BUFSIZE)
			len = BUFSIZE;
		else if (rest == 0)
			break;
		else
			len = rest;

		fseek(in, dataSize - rest, SEEK_SET);
		fread(buf, 1, len, in);

		// 데이터 보내기(고정길이)
		retval = send(sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}

		// 데이터 보내기(가변길이)
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}
		rest -= len;

		printf("[TCP 클라이언트] %d + %d바이트를 보냈습니다.\n",sizeof(int), retval);
		printf("[TCP 클라이언트] %d바이트가 남았습니다. \n",rest);

	}

	// closesocket()
	closesocket(sock);

	fclose(in);

	// 윈속 종료
	WSACleanup();
	return 0;
}