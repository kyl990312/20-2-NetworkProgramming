#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

/*=========================================================================================================
	# 소켓 주소 구조체
	* 네트워크 프로그램에서 필요로 하는 주소정보를 담고있는 구조체로 다양한 소켓함수 인자로 사용
	* 프로토콜 체계에 따라 다양한 형태가 존재
	* 기본형 SOCKADDR 구조체
	
	# 소켓 주소 구조체 사용
	1. 응용 프로그램이 소켓 주소 구조체를 초기화하고 소켓함수에 넘겨주는 경우
	2. 소켓 함수가 소켓 주소 구조체를 입력으로 받아 내용을 채우면 응용 프로그램이 이를 출력등의 목적으로 사용하는
	  경우
==========================================================================================================*/

int main(int argc, char* argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// ============== IPv4 변환 연습 ===================================
	// 원래의 Ipv4 주소 출력
	char* ipv4test = (char*)"147.46.114.70";
	printf("IPv4 주소(변환전) = %s\n", ipv4test);

	// inet_addr(const char* p) : 문자열 형태로 IPv4 주소입력 -> 32비트 숫자 (네트워크 바이트 정렬)로 return
	printf("IPv4 주소(변환후) = 0x%x\n", inet_addr(ipv4test));

	// inet_ntoa(struct in_addr in) : 숫자(32bit 네트워크 바이트 정렬) -> 문자
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
	printf("IPv4 주소(다시 변환 후) = %s\n", inet_ntoa(ipv4num));


	// ============== IPv6 변환 연습 ===================================
	// 원래의 IPv6 주소 출력
	char* ipv6test = (char*)"2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 주소(변환 전) = %s\n", ipv6test);

	/* 
	문자열 -> 숫자	
	WSAStringToAddress(
		LPTSTR AddressString,			// 문자열 형식의 IP 주소
		INT AddressFamily,				// AF_INET or AF_INET6
		LPWSAPROTOCOL_INFO lpProtocolInfo,	// NULL
		L[SOCKADDR lpAddress,			// IP 주소(숫자)를 저장할 구조체 : SOCKADDR_IN or SOCKADDR_IN6
		LPINT lpAddressLength			// 주소 구조체 길이
	); */

	SOCKADDR_IN6 ipv6num;
	// IPv6 소켓 주소 구조체
	/*	typedef struct sockaddr_in6 {
			short sin6_family;			// AF_INET6
			u_short sin5_port;			// 포트 번호
			u_long sin6_flowinfo;		// 대부분 0으로 설정
			struct in6_addr sin6_addr;	// Ipv6 주소
			u_long sin6_scope_id;		// 대부분 0으로 설정
		} SOCKADDR_IN;
	*/
	int addrlen = sizeof(ipv6num);
	WSAStringToAddress(ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num, &addrlen);

	/* typedef struct sockaddr{
		u_short sa_family;			// 주소체계
		char sa_data[4];			// 주소정보
	} SOCKADDR ;*/

	printf("IPv6 주소(변환 후) = 0x");
	for (int i = 0; i < 16; ++i) {
		printf("%02x", ipv6num.sin6_addr.u.Byte[i]);
	}
	printf("\n");

	/* 숫자 -> 문자열
	WSAAddressToString(
		LPSOCKADDR lpsaAddress,				// 숫자 형식의 IP 주소: SOCKADDR_IN or SOCKADDR_IN6
		DWORD dwAddressLength,				// 주소 구조체의 길이
		LPWSAPROTOCOL_INFO lpProtocolInfo,	// NULL
		LPTSTR lpszAddressString,			// IP 주소(문자열)를 저장할 버퍼
		LPDWORD lpdwAddressStringLength		// 버퍼의 길이
	); */

	char ipaddr[50];
	DWORD ipaddrlen = sizeof(ipaddr);
	WSAAddressToString((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
	printf("IPv6 주소(다시 변환 후) = %s\n", ipaddr);

	WSACleanup();
	return 0;
}