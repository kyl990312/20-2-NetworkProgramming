#pragma comment (lib,"ws2_32")
#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>

void err_display(char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void DataFromDomain(char* name) {
	printf("%s의 호스트 정보\n", name);
	// 도메인으로부터 호스트 정보를 받아온다.
	HOSTENT* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display((char*)"gethostbyaddr()");
		return;
	}
	for (int i = 0; ptr->h_aliases[i] != NULL; ++i) {
		// hostent 구조체 내에 별명정보를 담고있는 포인터 배열에 접근한다.
		// 별명 배열의 끝은 NULL이므로 NULL이 나올때까지 이동해 각 배열이 담고있는 별명에 접근해 그 내용을 출력한다.
		printf("h_aliases[%d]: %s\n", i, (LPCTSTR)ptr->h_aliases[i]);
	}
	for (int i = 0; ptr->h_addr_list[i] != NULL; ++i) {
		// IP접근 법은 별명 접근법과 같다.
		// 단, 가져온 데이터 그대로 출력하면 숫자 형태로 나오지 않으므로 addr구조체로 주소를 저장한뒤
		// inet_ntoa함수를 이용해 IP주소를 문자형태로 출력시킨다.
		IN_ADDR addr;
		memcpy(&addr, ptr->h_addr_list[i], ptr->h_length);
		printf("h_addr_list[%d]: %s\n", i, inet_ntoa(addr));
	}
	printf("\n");
}

void DataFromIP(char* name) {
	IN_ADDR addr;
	addr.s_addr = inet_addr(name);
	// gethostbyaddr에 char* 형식의 IP 주소를 인자로 넘기기 위해 addr 구조체를 선언한 뒤
	// inet_addr을 이용해 10진수 형태의 IP를 변경해 addr에 저장시킨다
	// gethostbyaddr을 이용해 IP주소로부터 호스트의 정보를 얻어온다.
	HOSTENT* ptr = gethostbyaddr((char*)&addr,sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display((char*)"gethostbyaddr()");
		return;
	}
	for (int i = 0; ptr->h_aliases[i] != NULL; ++i) {
		printf("h_aliases[%d]: %s\n", i, (LPCTSTR)ptr->h_aliases[i]);
	}
	for (int i = 0; ptr->h_addr_list[i] != NULL; ++i) {
		IN_ADDR addr;
		memcpy(&addr, ptr->h_addr_list[i], ptr->h_length);
		printf("h_addr_list[%d]: %s\n", i, inet_ntoa(addr));
	}
}

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	if (argc <= 1) {
		printf("주소가 입력되지 않았습니다.\n");
		return 0;
	}

	for (int i = 1; i < argc; ++i) {
		char* name = argv[i];
		if (isalpha(name[0]))
			DataFromDomain(name);
		else
			DataFromIP(name);
	}

	WSACleanup();
	return 0;
}