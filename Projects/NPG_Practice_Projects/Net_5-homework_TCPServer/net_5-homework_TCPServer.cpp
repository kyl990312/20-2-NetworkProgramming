#pragma comment(lib,"ws2_32")
#include<winSock2.h>
#include<stdlib.h>
#include<stdio.h>

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

// 사용자 정의 데이터 수신 함수
// TCP의 경우 데이터 경계를 구분하지 않기 때문에 대이터가 나뉘어 이동할 수 있다.
// 따라서 수신 버퍼에 해당 데이터가 전부 저장되지 않을 수 있기 때문에 
// 자신이 받을 데이터의 크기를 미리 알고 있다면 그만큼 받을 때까지 recv()를 계속 호출해야함
int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		// recv() : 운영체제의 수신 버퍼에 도착한 데이터를 으용프로그램 버퍼에 복사
		// int recv(SOCKET s, char* buf, itn len, int flags)

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
		return ;
	}

	printf("파일의 이름 길이: %d\n", nameLen);
	// 파일 이름을 받는다
	retval = recvn(client_sock, fileName, nameLen, 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return ;
	}
	// 쓰레기값 제거
	for (int i = nameLen; i < 50; ++i)
		fileName[nameLen] = '\0';
	printf("파일의 이름: %\s\n", fileName);

	FILE* fp = fopen(fileName, "wb");

	fclose(fp);
}

void SaveData(SOCKET client_sock, char fileName[50]) {
	int retval;
	int dataSize;
	int len;
	int left;

	FILE* fp = fopen(fileName, "a+b");
	char buf[BUFSIZE + 1];

	// 파일의 전체 크기를 받는다.
	retval = recvn(client_sock, (char*)&dataSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display((char*)"recv()");
		return;
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
		fwrite(buf, 1, len, fp);
		printf("파일을 이어 저장했습니다\n");
		
		left -= len;

		printf("[전송률] %d%% \n", (int)((1.0f - (float)left / (float)dataSize) * 100));
	}
	fclose(fp);
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

	// bind() : 소켓의 지역  IP 주소와 포트번호 결졍
	/* int bind(
		SOCKET s,						// 대기소켓 핸들값
		const struct sockaddr *name,	// 소켓 주소
		int namelen						// 소켓 주소 길이
	) */
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));		// 초기화
	serveraddr.sin_family = AF_INET;					// 주소체계 지정
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		// 어떤 IP 주소로든 접근 가능하도록 
	serveraddr.sin_port = htons(SERVERPORT);			// 포트번호 지정
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));		// bind
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen() : 소켓의 TCP 포트 상태를 LISTENING으로 변경
	/* int listen(
			SOCKET s,	
			int backlog		// 클라이언트 갯수
	)  클라이언트 접속 요청을 수용하기 위해 queue를 사용 */

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
		

	// 저장할 파일의 데이터를 담을 변수
	char fileName[50];		// 파일 이름

	while (1) {
		
		// accept() : 접속한 클라이언트와 통신할 수 있도록 새로운 소켓을 생성해서 리턴
		/* SOCKET accept(
				SOCKET	s,				// 클라이언트 접속 수용 목적 대기소켓
				struct sockaddr *addr,	// 주소를 잡으면 accept()가 port와 ipaddr 메모리에 할당
				int *addrlen
		)*/

		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 새로운 파일을 생성한다.
		MakeNewFile(client_sock,fileName);

		// 전송되는 데이터를 파일에 저장한다.
		SaveData(client_sock, fileName);

		// closesocekt()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}