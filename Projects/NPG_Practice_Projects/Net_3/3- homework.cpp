#pragma comment(lib,"ws2_32")
#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>

union DATA {
	WORD data;
	struct {
		BYTE hibyte;
		BYTE lobyte;
	};
};

BOOL IsLittleEndian() {
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

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	if (IsLittleEndian()) {
		printf("LittleEndian\n");
	}
	if (IsBigEndian()) {
		printf("BigEndian\n");
	}

	WSACleanup();
	return 0;
}