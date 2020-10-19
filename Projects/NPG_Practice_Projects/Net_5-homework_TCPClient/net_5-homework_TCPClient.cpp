#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>

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

void DrawProgressBar(char fileName[50], int total, int val) {
	int drawVal = ((float)val / (float)total) * 20;			// '■' 개수

	// 출력
	printf("[ %s ] ", fileName);
	for (int j = 0; j < drawVal; j++)
		printf("■");
	for (int j = 0; j < 20 - drawVal; ++j)
		printf("□");
	printf("\t%d%%\r", (int)(((float)val / (float)total) * 100));
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

	// connect() : TCP 프로토콜 수준에서 서버와 논리적 연결 설정 ( bind & connect 함께 수행)
	/* int connect(
			SOCKET s,
			const struct sockaddr *name,
			int namelen
	)*/

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"connect()");

	int nameLen = strlen(argv[1]);

	// send() : 응용 프로그램 데이터를 운영체제의 송신버처에 복사함
	// int send(SOCKET s, const char* buf, int len, int flags)

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
	fseek(in, 0, SEEK_END);				// 파일의 끝으로 포인터를 옮긴다
	dataSize = ftell(in);				// 파일의 끝의 위치를 통해 파일의 크기를 읽어온다
	fseek(in, 0, SEEK_SET);				// 파일의 처음어로 포인터를 옮긴다

	// 파일의 전체 크기를 보낸다.
	retval = send(sock, (char*)&dataSize, sizeof(int), 0);	
	if (retval == SOCKET_ERROR) {
		err_display((char*)"send()");
	}

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	int len;

	// 남은 전송데이터의 크기
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

		// 앞으로 보내야할 위치 : dataSize - rest
		fseek(in, dataSize - rest, SEEK_SET);
		fread(buf, 1, len, in);

		// 데이터 보내기 전송 데이터 크기
		retval = send(sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}

		// 데이터 보내기 전송 데이터
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}
		rest -= len;		// 보낸 데이터만큼 남은 크기에서 빼준다
		
		DrawProgressBar(argv[1], dataSize, dataSize - rest);

	}

	// closesocket()
	closesocket(sock);

	fclose(in);

	// 윈속 종료
	WSACleanup();
	return 0;
}