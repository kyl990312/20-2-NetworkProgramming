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
	//#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	MessageBox(NULL, (LPCSTR)"���� �ʱ�ȭ ����", (LPCSTR)	"�˸�", MB_OK);

	// ���� ���� 2(���� 8��Ʈ).2(���� 8��Ʈ)
	// WIN32 ��� => DWORD :4Byte(32-bit) / WORD�� 2Byte(16-bit)
	// LOBYTE : ���� ���� 1����Ʈ�� ����
	// HIBYTE : ���� ���� 1����Ʈ�� ����
	std::cout << "wVersion: " << (int)LOBYTE(wsa.wVersion) <<"."<<(int)HIBYTE(wsa.wVersion)<< std::endl;
	// ���� ���̺귯���� ����� �� �ִ� ���� ���� ����
	std::cout << "wHighVersion: " << (int)LOBYTE(wsa.wHighVersion) << "." << (int)HIBYTE(wsa.wHighVersion) << std::endl;
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