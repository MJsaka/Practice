#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

#pragma comment(lib, "ws2_32")

int const max_threads = 10;

DWORD WINAPI ThreadFunc(LPVOID p)
{
	DWORD ThreadID = (DWORD)p;
	SOCKET sock;
	WSADATA wsaData;
	int err;

	err = WSAStartup(0x0101, &wsaData);

	if(err == SOCKET_ERROR)
	{
		fprintf(stderr, "WSAStartup Failed");
		return EXIT_FAILURE;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0)
	{
		fprintf(stderr, "socket() failed");
		return EXIT_FAILURE;
	}

	SOCKADDR_IN server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	int ret = connect(sock, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));

	string s;
	for(int i=0; i<1000000; i++)
	{
		ostringstream oss;
		oss << "Thread " << ThreadID << " Msg " << i << endl;
		s = oss.str();
		ret = send(sock, s.c_str(), s.size(), 0);

		char buf[512] = {0};
		ret = recv(sock,buf,512,0);
// 		cout << buf;
	}

	closesocket(sock);

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	HANDLE hThreads[max_threads];
	DWORD threadID;
	for (int i = 0 ; i < max_threads; ++i)
	{
		hThreads[i] = CreateThread(NULL,0,ThreadFunc,(LPVOID)i,0,&threadID);
	}
	
	for (int i = 0; i < max_threads; ++i)
	{
		WaitForSingleObject(hThreads[i], INFINITE);
	}
	
	getchar();
}
