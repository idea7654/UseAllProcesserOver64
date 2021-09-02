// TestConsoleApp.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma warning(disable:4996)
#include <iostream>
#include <windows.h>
#include <WinBase.h>
#include <string.h>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[])
{
	const char *target_cmd = "ThreadProcessing.exe";
	char command[25];
	strcpy_s(command, "ThreadProcessing.exe ");
	strcat_s(command, argv[1]);
	cout << command << endl;

	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	BOOL rv = ::CreateProcess(
		target_cmd,          // LPCTSTR lpApplicationName
		command,                // LPTSTR lpCommandLine
		NULL,                // LPSECURITY_ATTRIBUTES lpProcessAttributes
		NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
		true,               // BOOL bInheritHandles
		INHERIT_PARENT_AFFINITY,    // DWORD dwCreationFlags
		NULL,                // LPVOID lpEnvironment
		NULL,                // LPCTSTR lpCurrentDirectory
		&startup_info,       // LPSTARTUPINFO lpStartupInfo
		&process_info        // LPPROCESS_INFORMATION lpProcessInformation
	);

	::CloseHandle(process_info.hThread);

	::WaitForSingleObject(process_info.hProcess, INFINITE);

	::CloseHandle(process_info.hProcess);
}