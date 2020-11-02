#include<windows.h>
#include<stdio.h>

struct Point3D {
	int x, y, z;
};

// 스레드 함수 형태 : DWORD WINAPI MyThread(LPVOID arg){ ... }
DWORD WINAPI MyThread(LPVOID arg)
{
	Point3D* pt = (Point3D*)arg;
	while (1) {
		printf("Running MyThread() %d: %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
		Sleep(1000);
	}
	return 0;
}

int main(int argc, char* argv[]) 
{
	// 첫번째 스레드 생성
	Point3D pt1 = { 10,20,30 };
	/* CreateThread()
	HANDLE CreateThread(
		LPSECURITY_ATTRIBUTES lpThreadAttribute,		핸들 상속값 / 보안 디스크립터 : NULL
		SIZE_T dwStackSize,								스택 사이즈(레지스터 사이즈)	- 0이면 기본크기
		LPTHREAD_START_ROUTINE lpSTartAddress,			스레드 함수 시작 주소(DWORD WINAPI) 
		LPVOID lpParameter,								스레드 함수에 전달할 인자
		DWORD dwCreationFlags,							0 or CREATE_SUSPENDED
		LPDWORD lpThreadId								스레드 ID: 필요한 경우에 id 아니면 NULL
	)*/
	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL) return 1;
	CloseHandle(hThread1);

	// 두번째 스레드 생성
	Point3D pt2 = { 40,50,60 };
	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL) return 1;
	CloseHandle(hThread2);

	while (1) {
		printf("Running main()  %d\n", GetCurrentThreadId());
		Sleep(1000);
	}
	return 0;
}

/*스레드 종료
스레드 함수 내 : ExitThread()
void ExitThread(
	DOWRD dwEixtCode		종료코드
)

다른 스레드: TerminateThread()
BOOL TerminateThread(
	HANDLE hThread,			종료할 스레드 핸들
	DWORD dwExitCode		종료 코드
)
*/