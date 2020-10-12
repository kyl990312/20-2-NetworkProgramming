#include<stdio.h>
#include<windows.h>

void gotoxy(int x, int y) {
	COORD pos = { x,y }; //x, y 좌표 설정
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); //커서 설정
}

int main() {
	int total = 100;
	for (int i = 0; i <= total; i++) {
		int drawVal = ((float)i / (float)total) * 20;
		gotoxy(0, 10);
		printf("test\n");
		printf("[test]  ");
		for (int j = 0;j < drawVal; j++)
			printf("■");
		for (int j = 0; j<20 - drawVal; ++j)
			printf("□");
		printf("        %f", (float) ((float)i / (float)total) * 100);
		Sleep(200);
	}
}