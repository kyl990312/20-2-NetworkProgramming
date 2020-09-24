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
	printf("%s�� ȣ��Ʈ ����\n", name);
	// ���������κ��� ȣ��Ʈ ������ �޾ƿ´�.
	HOSTENT* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display((char*)"gethostbyaddr()");
		return;
	}
	for (int i = 0; ptr->h_aliases[i] != NULL; ++i) {
		// hostent ����ü ���� ���������� ����ִ� ������ �迭�� �����Ѵ�.
		// ���� �迭�� ���� NULL�̹Ƿ� NULL�� ���ö����� �̵��� �� �迭�� ����ִ� ���� ������ �� ������ ����Ѵ�.
		printf("h_aliases[%d]: %s\n", i, (LPCTSTR)ptr->h_aliases[i]);
	}
	for (int i = 0; ptr->h_addr_list[i] != NULL; ++i) {
		// IP���� ���� ���� ���ٹ��� ����.
		// ��, ������ ������ �״�� ����ϸ� ���� ���·� ������ �����Ƿ� addr����ü�� �ּҸ� �����ѵ�
		// inet_ntoa�Լ��� �̿��� IP�ּҸ� �������·� ��½�Ų��.
		IN_ADDR addr;
		memcpy(&addr, ptr->h_addr_list[i], ptr->h_length);
		printf("h_addr_list[%d]: %s\n", i, inet_ntoa(addr));
	}
	printf("\n");
}

void DataFromIP(char* name) {
	IN_ADDR addr;
	addr.s_addr = inet_addr(name);
	// gethostbyaddr�� char* ������ IP �ּҸ� ���ڷ� �ѱ�� ���� addr ����ü�� ������ ��
	// inet_addr�� �̿��� 10���� ������ IP�� ������ addr�� �����Ų��
	// gethostbyaddr�� �̿��� IP�ּҷκ��� ȣ��Ʈ�� ������ ���´�.
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
		printf("�ּҰ� �Էµ��� �ʾҽ��ϴ�.\n");
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