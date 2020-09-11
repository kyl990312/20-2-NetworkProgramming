#pragma comment(lib, "ws2_32")
#include <winsock.h>
#include <iostream>

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char* argv[]) {
	// ���� �ʱ�ȭ
	WSADATA	wsa;
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
		return 1;
	MessageBox(NULL, (LPCSTR)"���� �ʱ�ȭ ����", (LPCSTR)	"�˸�", MB_OK);

	// ���� ����
	std::cout << "wVersion: " << wsa.wVersion << std::endl;
	// ���� ���̺귯���� ����� �� �ִ� ���� ���� ����
	std::cout << "wHighVersion: " << wsa.wHighVersion << std::endl;
	// ���� ������ ���õ� ����
	// ���� DLL���� ���Ͽ� ���õ� ���� ���ڿ� ī��
	std::cout << "szDescription: " << wsa.szDescription << std::endl;
	// �ý����� ���� ���¸� �� �� �ֵ��� ����
	std::cout << "szSystemStatus: " << wsa.szSystemStatus << std::endl;

	// socket()
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) err_quit((char*)"socket()");
	MessageBox(NULL, (LPCSTR)"���� ���� ����", (LPCSTR)"�˸�", MB_OK);

	// closesocket()
	closesocket(tcp_sock);


	// ���� ����
	WSACleanup();
	return 0;
}