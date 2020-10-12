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

int threadIds[64];			// 어떤 스레드의 프로그레스 바인지 구분하기 위함. id를 통해 커서의 위치를 구한다.
int clientCnt = 0;

int noticeY = 1;

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


void MoveCursor(int y) {
	COORD pos = { 0,y }; //x, y 좌표 설정
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); //커서 설정
}

void DrawProgressBar(char fileName[50],int total, int val, int y) {
	int drawVal = ((float)val / (float)total) * 20;			// '■' 개수
	//printf("cursor y: %d\n", y);

	// 커서 이동
	MoveCursor(y);

	// 출력
	printf("[ %s ] ",fileName);
	for (int j = 0; j < drawVal; j++)
		printf("■");
	for (int j = 0; j < 20 - drawVal; ++j)
		printf("□");
	printf("\t%d%%\n", (int)(((float)val / (float)total) * 100));
}


void MakeNewFile(SOCKET client_sock, char fileName[50])
{
	int retval;
	int nameLen;

	// 파일 이름의 길이를 받는다
	retval = recvn(client_sock, (char*)&nameLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		MoveCursor(NOTICE_Y+noticeY);
		noticeY++;
		err_display((char*)"recv()");
		return;
	}

	// 파일 이름을 받는다
	retval = recvn(client_sock, fileName, nameLen, 0);
	if (retval == SOCKET_ERROR) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
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

	FILE* fp = fopen(fileName, "a+b");
	if (fp == NULL) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
		printf("파일이 존재하지 않습니다");
		return;
	}

	// 파일의 전체 크기를 받는다.
	retval = recvn(client_sock, (char*)&fileSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		MoveCursor(NOTICE_Y + noticeY);
		noticeY++;
		err_display((char*)"recv()");
		return;
	}
	left = fileSize;

	threadIds[clientCnt++] = GetCurrentThreadId();

	// 클라이언트와 데이터 통신
	while (1) {
		// 데이터 받기(고정길이)
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			MoveCursor(NOTICE_Y + noticeY);
			noticeY++;
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 데이터 받기(가변길이)
		retval = recvn(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			MoveCursor(NOTICE_Y + noticeY);
			noticeY++;
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 저장
		fwrite(buf, 1, len, fp);
		left -= len;

		// 프로그래스 바 출력
		for (int i = 0; i < clientCnt; ++i) {
			if (GetCurrentThreadId() == threadIds[i]) {
				DrawProgressBar(fileName, fileSize, fileSize - left,i);
				break;
			}
		}		
	}
	fclose(fp);
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// 저장할 파일의 데이터를 담을 변수
	char fileName[50];		// 파일 이름

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	// 스레드 함수에 소켓만 전달한 경우에는 별도의 주소정보가 없으므로 소켓을 통해 주소 정보를 얻는 기능이 필요
	// 소켓 데이터 구조체에 저장된 원격 IP 주소와 원격 포트 번호 리턴
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// 새로운 파일을 생성한다.
	MakeNewFile(client_sock, fileName);

	// 전송되는 데이터를 파일에 저장한다.
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

	// 윈속 초기화
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
	ZeroMemory(&serveraddr, sizeof(serveraddr));		// 초기화
	serveraddr.sin_family = AF_INET;					// 주소체계 지정
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		// 어떤 IP 주소로든 접근 가능하도록 
	serveraddr.sin_port = htons(SERVERPORT);			// 포트번호 지정
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
			MoveCursor(NOTICE_Y + noticeY);
			noticeY++;
			err_display((char*)"accept()");
			break;
		}
		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	//printf("client Cnt :%d\n", clientCnt);
	return 0;
}