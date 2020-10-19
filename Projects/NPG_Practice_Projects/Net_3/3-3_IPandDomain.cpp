#pragma comment(lib,"ws2_32")
#include<winsock2.h>
#include<stdio.h>

/*=========================================================================================================
	# 바이트 정렬
	- Big / Little Endian 
	# 바이트 정렬 함수 종류
	* 유닉스 호환
	- u_shrot htons(u_short hostshort)		host -> net (short)
	- u_short ntohs(u_short netshrot)		net -> host (short)
	- u_long htonl(u_long hostlong)			host -> net (long)
	- u_long ntohl(u_long netlong)			net -> host (long)

	* 윈속 확장
	- int WSAHtons(SOCKET s, u_short hostshort, u_short *lpnetshrot)
	- int WSANtohs(SOCKET s, u_short netshort, u_short *lpnetshrot)
	- int WSAHtonl(SOCKET s, u_long hostlong, u_long *lpnetshrot)
	- int WSAHtonl(SOCKET s, u_long netlong, u_long *lpnetshrot)
==========================================================================================================*/

#define TESTNAME "www.example.com"

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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
		char* h_name;			// 공식 도메인 이름
		char** h_aliases;		// 별칭 도메인 이름들
		short h_addrtype;		// 주소체계 상수값
		short h_length;			// IP주소 길이
		char** h_addr_list;		// 호스트가 복수의 IP를 가지는 경우
	#define h_addr h_addr_list[0]		// 대표 IP주소
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
		(const char*)addr,		IP 주소
		int len,				주소 길이
		int type				주소 체계
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

	printf("도메인 이름(변환전) = %s\n", TESTNAME);
	
	// 도메인 -> IPv4
	IN_ADDR addr;
	if (GetIPAddr((char*)TESTNAME, &addr)) {
		// 성공이면 결과 출력
		printf("IP 주소(변환 후) = %s\n", inet_ntoa(addr));

		// IP -> 도메인
		char name[256];
		if (GetDomainName(addr, name, sizeof(name))) {
			// 성공이면 결과 출력
			printf("도메인 이름(다시 변환후) = %s\n",name);
		}
	}

	WSACleanup();
	return 0;
}