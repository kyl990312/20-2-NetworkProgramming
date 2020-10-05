#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>

#define BUF_SIZE 80000

char* ReadFile(char* fileName, int* size) {
	FILE* in = fopen("test.txt", "rb");
	if (in == NULL) {
		printf("파일을 읽어올 수 없습니다.");
		return NULL;
	}
	fseek(in, 0, SEEK_END);
	*size = ftell(in);
	
	char* buf = new char[*size];

	fseek(in, 0, SEEK_SET);
	fread(buf, 1, *size, in);
	fclose(in);

	return buf;
}

void SaveFile(char* data, int size) {
	FILE* fp = fopen("homeworkTest_5_server.txt", "wb");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.\n");
		return;
	}
	fwrite(data, 1, size, fp);
	fclose(fp);
}

int main() {
	int dataSize;
	char* data = ReadFile((char*)"suri.jpg", &dataSize);
	printf("%s", data);
	SaveFile(data,dataSize);
}