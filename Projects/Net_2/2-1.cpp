#pragma comment(lib, "ws2_32")
#include <winsock.h>
#include <iostream>

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char* argv[]) {
	// 윈속 초기화
	WSADATA	wsa;
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
		return 1;
	MessageBox(NULL, (LPCSTR)"윈속 초기화 성공", (LPCSTR)	"알림", MB_OK);

	// 윈속 버전
	std::cout << "wVersion: " << wsa.wVersion << std::endl;
	// 윈속 라이브러리가 사용할 수 있는 가장 높은 버전
	std::cout << "wHighVersion: " << wsa.wHighVersion << std::endl;
	// 윈속 구현에 관련된 정보
	// 윈속 DLL에서 소켓에 관련된 설명 문자열 카피
	std::cout << "szDescription: " << wsa.szDescription << std::endl;
	// 시스템의 각종 상태를 알 수 있도록 해줌
	std::cout << "szSystemStatus: " << wsa.szSystemStatus << std::endl;

	// socket()
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) err_quit((char*)"socket()");
	MessageBox(NULL, (LPCSTR)"소켓 생성 성공", (LPCSTR)"알림", MB_OK);

	// closesocket()
	closesocket(tcp_sock);


	// 윈속 종료
	WSACleanup();
	return 0;
}