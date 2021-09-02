// Todo.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma warning(disable:4996)
#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <string>
#pragma comment(lib, "ws2_32")
using namespace std;

#define BUFFER_SIZE 1024
volatile int g_num = 0;
void Foo()
{
	for (size_t i = 0; i < 10000000; ++i)
	{
		++g_num;
		--g_num;
	}
}

void ProcessSocket(string message)
{
	WSADATA wsaData; // 윈속 데이터 구조체
	SOCKET ClientSocket; // 소켓 선언
	SOCKADDR_IN ToServer; // 서버로 보내는 주소정보 구조체

	int Send_Size;
	USHORT   ServerPort = 9996; // 서버 포트번호

	double dd = 100.12;
	char Buffer[BUFFER_SIZE] = {};
	strcpy_s(Buffer, message.c_str());
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
	{
		cout << "WinSock 초기화부분에서 문제 발생 " << endl;
		WSACleanup();
	}

	memset(&ToServer, 0, sizeof(ToServer));

	ToServer.sin_family = AF_INET;
	ToServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	ToServer.sin_port = htons(ServerPort); // 포트번호

	ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//

	if (ClientSocket == INVALID_SOCKET)
	{
		cout << "소켓을 생성할수 없습니다." << endl;
		closesocket(ClientSocket);
		WSACleanup();
	}
	Send_Size = sendto(ClientSocket, Buffer, BUFFER_SIZE, 0, (struct sockaddr*) &ToServer, sizeof(ToServer));
	closesocket(ClientSocket); //소켓 닫기
	WSACleanup();
}
int main(int argc, char *argv[])
{
	/*if ((int)argv[1] == 1)
	{
		cout << "first group!" << endl;
	}
	else
	{
		cout << "second group!" << endl;
	}*/
	//실행할 스레드 내용들 실행..
	vector<thread> threads;
	int threadSize = 300;
	threads.resize(threadSize);

	for (int i = 0; i < threadSize; ++i)
	{
		threads[i] = thread(Foo);
	}

	for (int i = 0; i < threadSize; ++i)
	{
		threads[i].join();
	}

	string message = to_string(g_num);
	ProcessSocket(message);
}