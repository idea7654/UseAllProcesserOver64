// ConsoleApplication1.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma warning(disable:4996)
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <WinBase.h>
#include <stdio.h>
#include <thread>

#pragma comment(lib, "ws2_32")
using namespace std;

#define BUFFER_SIZE 1024

void ProcessSocket()
{
	WSADATA wsaData; // 윈속 데이터 구조체.(WSAStartup() 사용할꺼!)
	SOCKET ServerSocket; // 소켓 선언
	SOCKADDR_IN ServerInfo; // 서버 주소정보 구조체
	SOCKADDR_IN FromClient; // 클라이언트에서 받는 주소정보 구조체

	int FromClient_Size; // 클라이언트로부터 받는 메시지 크기
	int Recv_Size; // 받는 사이즈
	int Send_Size; // 보내는 사이즈

	char Buffer[BUFFER_SIZE];
	short ServerPort = 9996; // 서버 포트번호

	//WSAStartup은 WS2_32.DLL을 초기화 하는 함수
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) // WSAStartup 설정에서 문제 발생하면
	{
		cout << "WinSock 초기화부분에서 문제 발생 " << endl;
		WSACleanup(); // WS2_32.DLL의 사용 끝냄
	}

	// memset : 사용자가 정한 메모리 크기만큼 메모리 영역을 특정한 값으로 채움
	memset(&ServerInfo, 0, sizeof(ServerInfo)); // 0으로 초기화
	memset(&FromClient, 0, sizeof(FromClient));
	memset(Buffer, 0, BUFFER_SIZE);

	ServerInfo.sin_family = AF_INET; // IPv4 주소체계 사용 
	ServerInfo.sin_addr.s_addr = inet_addr("127.0.0.1"); // 루프백 IP. 즉 혼자놀이용..
	ServerInfo.sin_port = htons(ServerPort); // 포트번호

	// 소켓 생성
	ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // udp용 소켓 생성. SOCK_DGRAM : UDP 사용
	if (ServerSocket == INVALID_SOCKET) // 에러 발생시
	{
		cout << "소켓을 생성할수 없습니다." << endl;
		closesocket(ServerSocket);
		WSACleanup();
	}

	// bind() - 새로 오는 클라이언트를 받을 welcome 소켓
	// 전달만 할꺼면 필요없음
	if (::bind(ServerSocket, (struct sockaddr*)&ServerInfo, sizeof(ServerInfo)) == SOCKET_ERROR)
	{
		cout << "바인드를 할 수 없습니다." << endl;
		closesocket(ServerSocket);
		WSACleanup();
	}

	while (1)
	{
		FromClient_Size = sizeof(FromClient);
		// recvfrom : UDP통신 비연결형.  패킷수신
		Recv_Size = recvfrom(ServerSocket, Buffer, BUFFER_SIZE, 0,
			(struct sockaddr*) &FromClient, &FromClient_Size);
		cout << "Size: " << Recv_Size << endl;
		if (Recv_Size < 0)
		{
			//cout << "recvfrom() error!" << endl;
			return;
		}
		cout << "Data: " << Buffer << endl;
	}
	closesocket(ServerSocket); // 소켓을 닫습니다.
	WSACleanup();
}

int main()
{
	//------------------socket
	std::thread t1(ProcessSocket);
	const char *target_cmd = "MultiProcessing.exe";
	char command[22];
	strcpy_s(command, "MultiProcessing.exe 1");
	char command2[22];
	strcpy_s(command2, "MultiProcessing.exe 2");

	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	STARTUPINFO startup_info2;
	PROCESS_INFORMATION process_info2;
	memset(&startup_info2, 0, sizeof(STARTUPINFO));
	startup_info2.cb = sizeof(STARTUPINFO);
	memset(&process_info2, 0, sizeof(PROCESS_INFORMATION));

	BOOL rv = ::CreateProcess(
		target_cmd,          // LPCTSTR lpApplicationName
		command,                // LPTSTR lpCommandLine
		NULL,                // LPSECURITY_ATTRIBUTES lpProcessAttributes
		NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
		true,               // BOOL bInheritHandles
		CREATE_NO_WINDOW,    // DWORD dwCreationFlags
		NULL,                // LPVOID lpEnvironment
		NULL,                // LPCTSTR lpCurrentDirectory
		&startup_info,       // LPSTARTUPINFO lpStartupInfo
		&process_info        // LPPROCESS_INFORMATION lpProcessInformation
	);

	BOOL rv2 = ::CreateProcess(
		target_cmd,          // LPCTSTR lpApplicationName
		command2,                // LPTSTR lpCommandLine
		NULL,                // LPSECURITY_ATTRIBUTES lpProcessAttributes
		NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
		true,               // BOOL bInheritHandles
		CREATE_NO_WINDOW,    // DWORD dwCreationFlags
		NULL,                // LPVOID lpEnvironment
		NULL,                // LPCTSTR lpCurrentDirectory
		&startup_info2,       // LPSTARTUPINFO lpStartupInfo
		&process_info2        // LPPROCESS_INFORMATION lpProcessInformation
	);
	//-------------------multiprocess

	::WaitForSingleObject(process_info.hProcess, INFINITE);
	::WaitForSingleObject(process_info2.hProcess, INFINITE);

	::CloseHandle(process_info.hProcess);
	::CloseHandle(process_info2.hProcess);
	::CloseHandle(process_info.hThread);
	::CloseHandle(process_info2.hThread);

	cout << "Create Process() Finish" << endl;
	t1.join();
	getchar();
}
