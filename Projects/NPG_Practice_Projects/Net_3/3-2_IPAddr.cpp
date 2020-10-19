#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

/*=========================================================================================================
	# ���� �ּ� ����ü
	* ��Ʈ��ũ ���α׷����� �ʿ�� �ϴ� �ּ������� ����ִ� ����ü�� �پ��� �����Լ� ���ڷ� ���
	* �������� ü�迡 ���� �پ��� ���°� ����
	* �⺻�� SOCKADDR ����ü
	
	# ���� �ּ� ����ü ���
	1. ���� ���α׷��� ���� �ּ� ����ü�� �ʱ�ȭ�ϰ� �����Լ��� �Ѱ��ִ� ���
	2. ���� �Լ��� ���� �ּ� ����ü�� �Է����� �޾� ������ ä��� ���� ���α׷��� �̸� ��µ��� �������� ����ϴ�
	  ���
==========================================================================================================*/

int main(int argc, char* argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// ============== IPv4 ��ȯ ���� ===================================
	// ������ Ipv4 �ּ� ���
	char* ipv4test = (char*)"147.46.114.70";
	printf("IPv4 �ּ�(��ȯ��) = %s\n", ipv4test);

	// inet_addr(const char* p) : ���ڿ� ���·� IPv4 �ּ��Է� -> 32��Ʈ ���� (��Ʈ��ũ ����Ʈ ����)�� return
	printf("IPv4 �ּ�(��ȯ��) = 0x%x\n", inet_addr(ipv4test));

	// inet_ntoa(struct in_addr in) : ����(32bit ��Ʈ��ũ ����Ʈ ����) -> ����
	IN_ADDR ipv4num;
	/*typedef struct in_addr{
		union{
			struct { u_char s_b1, s_b2, s_b3, s_b4;} S_un_b;
			struct {u_short s_w1, s_w2;} S_un_w;
			u_long S_addr;
		}S_un;
		#define s_addr S_un.S_addr
	} IN_ADDR; */
	ipv4num.s_addr = inet_addr(ipv4test);
	printf("IPv4 �ּ�(�ٽ� ��ȯ ��) = %s\n", inet_ntoa(ipv4num));


	// ============== IPv6 ��ȯ ���� ===================================
	// ������ IPv6 �ּ� ���
	char* ipv6test = (char*)"2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 �ּ�(��ȯ ��) = %s\n", ipv6test);

	/* 
	���ڿ� -> ����	
	WSAStringToAddress(
		LPTSTR AddressString,			// ���ڿ� ������ IP �ּ�
		INT AddressFamily,				// AF_INET or AF_INET6
		LPWSAPROTOCOL_INFO lpProtocolInfo,	// NULL
		L[SOCKADDR lpAddress,			// IP �ּ�(����)�� ������ ����ü : SOCKADDR_IN or SOCKADDR_IN6
		LPINT lpAddressLength			// �ּ� ����ü ����
	); */

	SOCKADDR_IN6 ipv6num;
	// IPv6 ���� �ּ� ����ü
	/*	typedef struct sockaddr_in6 {
			short sin6_family;			// AF_INET6
			u_short sin5_port;			// ��Ʈ ��ȣ
			u_long sin6_flowinfo;		// ��κ� 0���� ����
			struct in6_addr sin6_addr;	// Ipv6 �ּ�
			u_long sin6_scope_id;		// ��κ� 0���� ����
		} SOCKADDR_IN;
	*/
	int addrlen = sizeof(ipv6num);
	WSAStringToAddress(ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num, &addrlen);

	/* typedef struct sockaddr{
		u_short sa_family;			// �ּ�ü��
		char sa_data[4];			// �ּ�����
	} SOCKADDR ;*/

	printf("IPv6 �ּ�(��ȯ ��) = 0x");
	for (int i = 0; i < 16; ++i) {
		printf("%02x", ipv6num.sin6_addr.u.Byte[i]);
	}
	printf("\n");

	/* ���� -> ���ڿ�
	WSAAddressToString(
		LPSOCKADDR lpsaAddress,				// ���� ������ IP �ּ�: SOCKADDR_IN or SOCKADDR_IN6
		DWORD dwAddressLength,				// �ּ� ����ü�� ����
		LPWSAPROTOCOL_INFO lpProtocolInfo,	// NULL
		LPTSTR lpszAddressString,			// IP �ּ�(���ڿ�)�� ������ ����
		LPDWORD lpdwAddressStringLength		// ������ ����
	); */

	char ipaddr[50];
	DWORD ipaddrlen = sizeof(ipaddr);
	WSAAddressToString((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
	printf("IPv6 �ּ�(�ٽ� ��ȯ ��) = %s\n", ipaddr);

	WSACleanup();
	return 0;
}