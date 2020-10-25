#include<windows.h>
#include<stdio.h>

int sum = 0;

DWORD WINAPI MyThread(LPVOID arg) {
	int num = (int)arg;
	for (int i = 0; i <= num; i++) sum += i;
	return 0;
}

int main(int argc, char* argv[]) {
	int num = 100;
	HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)num, CREATE_SUSPENDED, NULL);
	if (hThread == NULL) return 1;

	printf("스레드 실행 전. 계산 결과 = %d\n", sum);

	// 스레드 실행 중지와 재실행
	/*
	- 실행 중지: SuspendThread()
	DWORD SuspendThread(
		HANDLE hThread		 스레드 핸들
	)

	- 재샐행 : ResumeThread()
	DWORD ResumeThread(
		HANDLE hThread
	)

	void Sleep(
		DWORD dwMilliseconds
	)
	*/
	ResumeThread(hThread);
	// 스레드 종료 기다리기
	/*
	- WaitForSingleObject(): 특정 스레드 종료할 때 까지 기다리기
	DWORD WaitForSingleObject(
		HANDLE hHandle,			스레드 핸들(종료할 대상 쓰레드)
		DWORD dwMilliseconds    대기시간 : 이 대기시간 동안 종료 안되면 WAIT_TIMEOUT , 대기시간 완료전 종료 : WAIT_FAIL
	)
	- WaitForMutipleObjects() : 둘 이상의 스레드가 종료할 때까지 기다리기
	DWORD WaitForMultipleObjects(
		HANDLE hHandle,
		const HANDLE* lpHandles,	배열 시작 주소
		BOOL bWaitAll,				배열에 들어가 있는 모든 쓰레드를 기다리고 싶으면 TRUE 아니면 BOOL
		DWORD dwMillseconds			대기시간
	)
	*/

	WaitForSingleObject(hThread, INFINITE);
	printf("수레드 실행 후. 계산 결과 = %d\n", sum);
	CloseHandle(hThread);

	return 0;
}
