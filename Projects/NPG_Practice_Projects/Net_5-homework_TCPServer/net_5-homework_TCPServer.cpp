#pragma comment(lib,"ws2_32")
#include<winSock2.h>
#include<stdlib.h>
#include<stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// cd E:\Git\20-2-NetworkProgramming\Projects\release_5\client
// Net_5-homework_TCPClient.exe test_client.txt
// Net_5-homework_TCPClient.exe suri.jpg
// Net_5-homework_TCPClient.exe katana_dance.mp4
// cd E:\Git\NGP\20-2-NetworkProgramming\Projects

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

// 사용자 정의 데이터 수신 함수
// recv()는 len으로 지정한 크기보다 적은 데이터가 응용 프로그램 버퍼에 복사될 수 있다.
// 자신이 받을 데이터의 크기를 미리 알고 있다면 그만큼 받을 때까지 recv()를 계속 호출해야함
int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;

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

int main(int argc, char* argv[])
{
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
	char buf[BUFSIZE + 1];			// 데이터를 BUFSIZE 만큼 전송 받은 후 마지막에 '\0'추가를 위해 +1
	int len;						

	// 저장할 파일의 데이터를 담을 변수
	char fileName[50];		// 파일 이름
	int nameLen;			// 파일 이름 길이
	int dataSize;			// 파일 전체 크기
	char* mode = (char*)"wb";	

	FILE* fp;
	int left;			// 전체 파일의 크기중 앞으로 받아야 할 크기

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 파일 이름의 길이를 받는다
		retval = recvn(client_sock, (char*)&nameLen, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}

		printf("파일의 이름 길이: %d\n", nameLen);
		// 파일 이름을 받는다
		retval = recvn(client_sock, fileName, nameLen, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		// 쓰레기값 제거
		for (int i = nameLen; i < 50; ++i)
			fileName[nameLen] = '\0';
		printf("파일의 이름: %\s\n", fileName);


		// 파일의 전체 크기를 받는다.
		int dataSize;
		retval = recvn(client_sock, (char*)&dataSize, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		printf("파일의 크기: %d\n", dataSize);
		left = dataSize;

		fp = fopen(fileName, "a+b");
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
				err_display((char*)"recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 저장
			if (fp == NULL) {
				// 이어서 저장할 파일이 없는경우 wb로 데이터를 저장할 파일을 새로 생성해야 한다.
				printf("새로운 파일을 생성합니다.\n");
				fp = fopen(fileName, "wb");
				fwrite(buf, 1, len, fp);
				fclose(fp);
				printf("파일을 저장했습니다!\n");
				// 새로 생성한 파일에 뒤이어 올 데이터들을 이어서 저장할 것이다.
				fp = fopen(fileName, "a+b");
			}
			else {
				fwrite(buf, 1, len, fp);
				printf("파일을 이어 저장했습니다\n");
			}
			left -= len;

			printf("[전송률] %d%% \n", (int)((1.0f - (float)left / (float)dataSize) * 100) );
		}

		// closesocekt()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);
	if(fp!=NULL) fclose(fp);

	// 윈속 종료
	WSACleanup();
	return 0;
}