#pragma comment(lib, "ws2_32")
#include <winsock.h>
#include <iostream>

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg) {
	LPVOID lpMsgBuf;
	/* DWORD FormatMessage(
		DWORD dwFlag,			// �ɼ�
		LPCVOID lpSource,		// NULL
		DOWRD dwMessageId,		// �����ڵ�
		DWORD dwLanguageId,		// ���
		LPTSTR lpBuffer,		// ���� ���ڿ� �����ּ�
		DWORD nSize,			// 0
		va_list* Arguments		// NULL
	)*/
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char* argv[]) {
	// ���� �ʱ�ȭ
	WSADATA	wsa;
	/* int WSAStartup(
		WORD wVersionRequested,
		LPWSADATA IpWSAData
	)
	���� : 0, ����: ���� �ڵ�  */	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)     
		return 1;
	//#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
	// ���� ���� 2(���� 8��Ʈ).2(���� 8��Ʈ)
	// WIN32 ��� => DWORD :4Byte(32-bit) / WORD�� 2Byte(16-bit)
	// LOBYTE : ���� ���� 1����Ʈ(8bit)
	// HIBYTE : ���� ���� 1����Ʈ(8bit)
	std::cout << "wVersion: " << (int)LOBYTE(wsa.wVersion) << "." << (int)HIBYTE(wsa.wVersion) << std::endl;
	// ���� ���̺귯���� ����� �� �ִ� ���� ���� ����
	std::cout << "wHighVersion: " << (int)LOBYTE(wsa.wHighVersion) << "." << (int)HIBYTE(wsa.wHighVersion) << std::endl;
	// ���� ������ ���õ� ����
	// ���� DLL���� ���Ͽ� ���õ� ���� ���ڿ� ī��
	std::cout << "szDescription: " << wsa.szDescription << std::endl;
	// �ý����� ���� ���¸� �� �� �ֵ��� ����
	std::cout << "szSystemStatus: " << wsa.szSystemStatus << std::endl;

	MessageBox(NULL, (LPCSTR)"���� �ʱ�ȭ ����", (LPCSTR)	"�˸�", MB_OK);


	// ���� ����
	/* SOCKET socket(
		int at,			// �ּ�ü��  : AF_INET
		int type,		// ����Ÿ��  : SOCKET_STREAM(TCP) / STOCKET_DGRAM(UDP)
		int protocol	// ��������	: 0	
	)		���� : ���ο� ���� / ���� : INVALID_SOCKET */
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) err_quit((char*)"socket()");
	MessageBox(NULL, (LPCSTR)"���� ���� ����", (LPCSTR)"�˸�", MB_OK);

	// closesocket()
	closesocket(tcp_sock);


	// ���� ����
	WSACleanup();
	return 0;
}
