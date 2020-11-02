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

	printf("������ ���� ��. ��� ��� = %d\n", sum);

	// ������ ���� ������ �����
	/*
	- ���� ����: SuspendThread()
	DWORD SuspendThread(
		HANDLE hThread		 ������ �ڵ�
	)

	- ����� : ResumeThread()
	DWORD ResumeThread(
		HANDLE hThread
	)

	void Sleep(
		DWORD dwMilliseconds
	)
	*/
	ResumeThread(hThread);
	// ������ ���� ��ٸ���
	/*
	- WaitForSingleObject(): Ư�� ������ ������ �� ���� ��ٸ���
	DWORD WaitForSingleObject(
		HANDLE hHandle,			������ �ڵ�(������ ��� ������)
		DWORD dwMilliseconds    ���ð� : �� ���ð� ���� ���� �ȵǸ� WAIT_TIMEOUT , ���ð� �Ϸ��� ���� : WAIT_FAIL
	)
	- WaitForMutipleObjects() : �� �̻��� �����尡 ������ ������ ��ٸ���
	DWORD WaitForMultipleObjects(
		HANDLE hHandle,
		const HANDLE* lpHandles,	�迭 ���� �ּ�
		BOOL bWaitAll,				�迭�� �� �ִ� ��� �����带 ��ٸ��� ������ TRUE �ƴϸ� BOOL
		DWORD dwMillseconds			���ð�
	)
	*/

	WaitForSingleObject(hThread, INFINITE);
	printf("������ ���� ��. ��� ��� = %d\n", sum);
	CloseHandle(hThread);

	return 0;
}
