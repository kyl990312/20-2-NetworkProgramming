#pragma comment(lib,"ws2_32")
#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>

#define PRACTICE_NUMBER 2

#if PRACTICE_NUMBER == 2
#include <ws2tcpip.h>
#endif // PRACTICE_NUMBER == 2

#if PRACTICE_NUMBER == 3
#define TESTNAME "www.naver.com"

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

// ������ �̸� -> IPV4�ּ�
BOOL GetIPAddr(char* name, IN_ADDR* addr) {
	HOSTENT* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display((char*)"gethostbyname()");
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET)
		return FALSE;
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return TRUE;
}

// IPv4 �ּ� -> ������ �̸�
BOOL GetDomainName(IN_ADDR addr, char* name, int namelen) {
	HOSTENT* ptr = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display((char*)"gethostbyaddr()");
		return FALSE;
	}
	if (ptr->h_addrtype != AF_INET) {
		return FALSE;
	}
	strncpy(name, ptr->h_name, namelen);
	return TRUE;
}
#endif

#if	PRACTICE_NUMBER == 4		// �������� 1(����)
union DATA {
	WORD data;
	struct {
		BYTE hibyte;
		BYTE lobyte;
	};
};

BOOL IsLittleEndian() {
	//int x = 1;
	//char* y = (char*)&x;

	//for (int i = 0; i <= 48; ++i) {
	//	printf("%c", *y + i);
	//}
	////printf("%c", *y + 48);
	//if(*y + 48 == '1')
	//	return TRUE;
	//return FALSE;
	DATA x;
	x.data = 0x1234;
	if (x.lobyte == 0x12) return TRUE;
	return FALSE;
}

BOOL IsBigEndian() {
	DATA x;
	x.data = 0x1234;
	if (x.hibyte == 0x12) return TRUE;
	return FALSE;
}
#endif

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

#if PRACTICE_NUMBER == 1

	u_short x1 = 0x1234;	// 16bit
	u_long y1 = 0x12345678;	// 32bit
	u_short x2;
	u_long y2;

	// ȣ��Ʈ ����Ʈ -> ��Ʈ��ũ ����Ʈ
	printf("[ȣ��Ʈ����Ʈ -> ��Ʈ��ũ ����Ʈ]\n");
	printf("0x%x -> 0x%x\n", x1, x2 = htons(x1));
	printf("0x%x -> 0x%x\n", y1, y2 = htonl(y1));

	// ��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ
	printf("\n[��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ]\n");
	printf("0x%x -> 0x%x\n", x2, ntohs(x2));
	printf("0x%x -> 0x%x\n", y2, ntohl(y2));

	// �߸��� ��� �� : u_long�� �Է����� �޴� �Լ��� u_short ���� ����� �� �߻��ϴ� ������
	printf("\n[�߸��� ��� ��]\n");
	printf("0x%x -> 0x%x\n", x1, htonl(x1));
#endif // 3 - 1

#if PRACTICE_NUMBER == 2
	// IPv4 ��ȯ ����
	// ������ IPv4 �ּ� ���
	char* ipv4test = (char*)"147.46.114.70";
	printf("IPv4 �ּ�(��ȯ��) = %s\n", ipv4test);
	
	// inet_addr() �Լ� ����
	printf("IPv4 �ּ�(��ȯ��) = 0x%x\n", inet_addr(ipv4test));

	// inet_ntoa() �Լ� ����
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);
	printf("IPv4 �ּ�(�ٽú�ȯ��) = %s\n", inet_ntoa(ipv4num));

	printf("\n");

	// IPv6 ��ȯ ����
	// ������ IPv6 �ּ� ���
	char* ipv6test = (char*)"2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 �ּ�(��ȯ��) = %s\n", ipv6test);

	// WSAStringToAddress() �Լ� ����
	SOCKADDR_IN6 ipv6num;
	int addrlen = sizeof(ipv6num);
	WSAStringToAddress(ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num,&addrlen);
	printf("IPv6 �ּ�(��ȯ��) = 0x");
	for(int i = 0 ; i < 16; ++i)
		printf("%02x", ipv6num.sin6_addr.u.Byte[i]);
	printf("\n");

	//WSAAddressToString() �Լ� ����
	char ipaddr[50];
	DWORD ipaddrlen = sizeof(ipaddr);
	WSAAddressToString((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
	printf("IPv6 �ּ�(�ٽú�ȯ��) = %s\n", ipaddr);	
#endif // 3 - 2
#if PRACTICE_NUMBER == 3
	printf("�������̸�(��ȯ��) = %s\n", TESTNAME);

	// ������ �̸� -> IP
	IN_ADDR addr;
	if (GetIPAddr((char*)TESTNAME, &addr)) {
		// �����̸� ��� ���
		printf("IP �ּ�(��ȯ��) = %s\n", inet_ntoa(addr));

		// IP �ּ� -> ������ �̸�
		char name[256];
		if (GetDomainName(addr, name, sizeof(name))) {
			// �����̸� ��� ���
			printf("�������̸�(�ٽú�ȯ��) = %s\n", name);
		}
	}

#endif
#if	PRACTICE_NUMBER == 4		// �������� 1(����)
	if (IsLittleEndian()) {
		printf("LittleEndian\n");
	}
	if (IsBigEndian()) {
		printf("BigEndian\n");
	}
#endif
	WSACleanup();
	return 0;
}