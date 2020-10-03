#include<stdio.h>
#include<stdlib.h>
#include <winsock2.h>

void SaveFile(char* data, int version) {
	char* filename = (char*)"homeworkTest_5_server.txt";
	FILE* fp = fopen("homeworkTest_5_server.txt", "w");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.\n");
		return;
	}
	int len = strlen(data);
	for (int i = 0; i < len; ++i) {
		fputc(data[i], fp);
	}
	fclose(fp);
	return;
}

int main() {
	char* data = (char*)"no stuty, no homework, no graduation work";
	SaveFile(data, 0);
}