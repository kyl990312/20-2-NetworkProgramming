#pragma comment(lib,"ws2_32")
#include<winsock2.h>
#include<stdio.h>

/*=========================================================================================================
	# ����Ʈ ����
	- Big / Little Endian 
	# ����Ʈ ���� �Լ� ����
	* ���н� ȣȯ
	- u_shrot htons(u_short hostshort)		host -> net (short)
	- u_short ntohs(u_short netshrot)		net -> host (short)
	- u_long htonl(u_long hostlong)			host -> net (long)
	- u_long ntohl(u_long netlong)			net -> host (long)

	* ���� Ȯ��
	- int WSAHtons(SOCKET s, u_short hostshort, u_short *lpnetshrot)
	- int WSANtohs(SOCKET s, u_short netshort, u_short *lpnetshrot)
	- int WSAHtonl(SOCKET s, u_long hostlong, u_long *lpnetshrot)
	- int WSAHtonl(SOCKET s, u_long netlong, u_long *lpnetshrot)
==========================================================================================================*/

#define TESTNAME "www.example.com"

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
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

// domain -> IPv4
BOOL GetIPAddr(char* name, IN_ADDR* addr) {
	
	/* typedef struct hostent{
		char* h_name;			// ���� ������ �̸�
		char** h_aliases;		// ��Ī ������ �̸���
		short h_addrtype;		// �ּ�ü�� �����
		short h_length;			// IP�ּ� ����
		char** h_addr_list;		// ȣ��Ʈ�� ������ IP�� ������ ���
	#define h_addr h_addr_list[0]		// ��ǥ IP�ּ�
	} HOSTENT;
	*/

	// struct hostent *gethostbyname(char* name)

	HOSTENT* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display((char*)"gethostbyname()");
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET) return FALSE;
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return TRUE;
}

// Ipv4 -> domain
BOOL GetDomainName(IN_ADDR addr, char* name, int namelen) {

	/* struct hostent *gethostbyaddr(
		(const char*)addr,		IP �ּ�
		int len,				�ּ� ����
		int type				�ּ� ü��
	) */

	HOSTENT* ptr = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display((char*)"gethostbyaddr()");
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET) return FALSE;
	strncpy(name, ptr->h_name, namelen);
	return TRUE;
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	printf("������ �̸�(��ȯ��) = %s\n", TESTNAME);
	
	// ������ -> IPv4
	IN_ADDR addr;
	if (GetIPAddr((char*)TESTNAME, &addr)) {
		// �����̸� ��� ���
		printf("IP �ּ�(��ȯ ��) = %s\n", inet_ntoa(addr));

		// IP -> ������
		char name[256];
		if (GetDomainName(addr, name, sizeof(name))) {
			// �����̸� ��� ���
			printf("������ �̸�(�ٽ� ��ȯ��) = %s\n",name);
		}
	}

	WSACleanup();
	return 0;
}