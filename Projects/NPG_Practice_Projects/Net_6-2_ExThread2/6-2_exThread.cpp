#include<windows.h>
#include<stdio.h>


DWORD WINAPI MyThread(LPVOID arg) {
	while (1);
	return 0;
}

int main() {
	// CPU 개수를 알아낸다
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	printf("CPU 개수: %d\n", (int)si.dwNumberOfProcessors);

	// CPU 개수만큼 스레드를 생성한다.
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++) {
		HANDLE hThread = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);
		if (hThread == NULL) return 1;
		// 최고 우선순위로 변경한다

		/* 우선순위 레벨 조작 함수 : SetThreadPriority()
		BOOL SetThreadPriority(
			HANDLE hThread,			스레드 핸들
			int nPriority			우선순위 '레벨'
		)

		int GetThreadPriority( HANDLE hThread )
		*/
		SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
		CloseHandle(hThread);
	}

	Sleep(1000);
	while (1) {
		printf("주 스래드 실행!\n");
		break;
	}

	return 0;
}