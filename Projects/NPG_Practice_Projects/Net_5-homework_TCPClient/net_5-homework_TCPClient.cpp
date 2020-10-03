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

char* ReadFile(char* fileName) {
	FILE* fp = fopen(fileName,"r");
	if (fp == NULL) {
		printf("%s 파일을 읽어올 수 없습니다.", fileName);
		return NULL;
	}
	char fileData[500];
	for (int i = 0; ;++i) {
		char ch = fgetc(fp);
		if (ch == EOF) {
			printf("파일 읽기 완료");
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

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	int len;

	// 서버와 데이터 통신
	for (int i = 1; i < agrc; ++i) {
		// 데이터 입력(시뮬레이션)
		char* data = ReadFile(argv[i]);
		len = strlen(data);
		strncpy(buf, data, len);

		// 데이터 보내기(고정길이)
		retval = send(sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"send()");
			break;
		}

		// 데이터 보내기(가변길이)
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			printf("가변길이가 문제임");
			err_display((char*)"send()");
			break;
		}

		printf("[TCP 클라이언트] %d + %d바이트를 보냈습니다.\n", sizeof(int), retval);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}