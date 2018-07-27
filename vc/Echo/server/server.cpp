#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include "mtverify.h"

#pragma comment(lib,"WS2_32")

#define MAXINPUT 100
#define MAXLINE 1000
#define SERV_TCP_PORT 8000

struct ContextKey
{
	SOCKET sock;
	OVERLAPPED ovIn;
	OVERLAPPED ovOut;

	char InBuffer[MAXLINE];
	char OutBuffer[MAXLINE];

	int nOutBufIndex;

	DWORD dwRead;
	DWORD dwWritten;
};

HANDLE ghCompletionPort;

void CheckOsVersion()
{
	OSVERSIONINFO ver;
	BOOL bResult;

	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	bResult = GetVersionEx( (LPOSVERSIONINFO) &ver);

	if( (!bResult) ||
			(ver.dwPlatformId != VER_PLATFORM_WIN32_NT) )
	{
		fprintf(stderr, "ECHOSRV must be run under Windows NT.\n");
		exit(EXIT_FAILURE);
	}
}


void FatalError(const char *errInfo)
{
	fprintf(stderr, errInfo);
	exit(EXIT_FAILURE);
}

void IssueRead(struct ContextKey* pKey)
{
	ReadFile(
			(HANDLE) (pKey->sock),
			pKey->InBuffer,
			MAXINPUT,
			&pKey->dwRead,
			&pKey->ovIn
		);
}


DWORD WINAPI ThreadFunc(LPVOID pVoid)
{
	BOOL bResult;
	DWORD dwNumRead;
	struct ContextKey *pCntx;
	LPOVERLAPPED lpOverlapped;

	DWORD dwThreadID = (DWORD)pVoid;

	while(true)
	{
		bResult = GetQueuedCompletionStatus(
				ghCompletionPort,
				&dwNumRead,
				(LPDWORD)&pCntx,
				&lpOverlapped,
				INFINITE
				);
		if (bResult == FALSE && lpOverlapped == NULL)
		{
			fprintf(stderr, "ThreadFunc %d - Illegal call to GetQueuedCompletionStatus", dwThreadID);
		}else if (bResult == FALSE && lpOverlapped != NULL)
		{
			closesocket(pCntx->sock);
			free(pCntx);
			fprintf(stderr, "ThreadFunc %d - I/O operation failed\n", dwThreadID);
		}else if (dwNumRead == 0)
		{
			closesocket(pCntx->sock);
			free(pCntx);
			fprintf(stderr, "ThreadFunc %d - End of file.\n", dwThreadID);
		}else
		{
			if (pCntx->nOutBufIndex + pCntx->dwRead > MAXLINE)
			{
				closesocket(pCntx->sock);
				free(pCntx);
				fprintf(stderr, "ThreadFunc %d - Input too long\n", dwThreadID);
			}
			
			memcpy(pCntx->OutBuffer+pCntx->nOutBufIndex,pCntx->InBuffer,dwNumRead);
			pCntx->nOutBufIndex+=dwNumRead;
			if (pCntx->InBuffer[dwNumRead-1] == '\n')
			{
				pCntx->OutBuffer[pCntx->nOutBufIndex++] = '\0';

				for (int i = 0; i < 1000; i++)
				{
				}
				
				//fprintf(stderr, "ThreadFunc %d - Echo on socket %d :\n%s", dwThreadID, pCntx->sock, pCntx->OutBuffer);
				WriteFile(
						(HANDLE)(pCntx->sock),
						pCntx->OutBuffer,
						pCntx->nOutBufIndex,
						&pCntx->dwWritten,
						&pCntx->ovOut
					 );
				pCntx->nOutBufIndex = 0;
				memset(pCntx->OutBuffer,0,MAXLINE);
			}

			memset(pCntx->InBuffer,0,MAXINPUT);
			IssueRead(pCntx);
		}
	}
	return 0;
}

void CreateWorkerThreads()
{
	SYSTEM_INFO sysinfo;
	DWORD dwThreadID;
	DWORD dwThreads;
	DWORD i;

	GetSystemInfo(&sysinfo);
	dwThreads = sysinfo.dwNumberOfProcessors * 2 + 2;

	for (i = 0; i < dwThreads; i++)
	{
		HANDLE hThread;
		hThread = CreateThread(NULL, 0, ThreadFunc, (LPVOID)i, 0, &dwThreadID);
		CloseHandle(hThread);
	}
}


int main(int argc, char *argv[])
{
	SOCKET listener;
	SOCKET newsocket;
	WSADATA wsaData;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	int clientAddrLen;
	int err;

	CheckOsVersion();

	err = WSAStartup (0x0101, &wsaData);

	if(err == SOCKET_ERROR)
	{
		FatalError("WSAStartup Failed");
		return EXIT_FAILURE;
	}

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0)
	{
		FatalError("socket() failed");
		return EXIT_FAILURE;
	}
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERV_TCP_PORT);

	err = bind(
			listener,
			(struct sockaddr *)&serverAddr,
			sizeof(serverAddr)
		  );

	if(err < 0)
		FatalError("bind() failed");

	ghCompletionPort = CreateIoCompletionPort(
			INVALID_HANDLE_VALUE,
			NULL,
			0,
			0
			);

	if(ghCompletionPort == NULL)
		FatalError("CreateIoCompletionPort() failed");

	CreateWorkerThreads();

	listen(listener, 5);

	fprintf(stderr, "Echo Server with I/O Completion Ports\n");
	fprintf(stderr, "Running on TCP port %d\n", SERV_TCP_PORT);
	fprintf(stderr, "\nPress Ctrl+C to stop the server\n");

	while(true)
	{
		struct ContextKey *pKey;

		clientAddrLen = sizeof(clientAddr);
		newsocket = accept(listener,
				(struct sockaddr *)&clientAddr,
				&clientAddrLen);

		if (newsocket < 0)
		{
			FatalError("accept() failed");
			return EXIT_FAILURE;
		}

		pKey = (struct ContextKey *)calloc(1, sizeof(struct ContextKey));
		pKey->sock = newsocket;
		pKey->ovOut.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	       	pKey->ovOut.hEvent = (HANDLE) ((DWORD)pKey->ovOut.hEvent | 0x1); 

		CreateIoCompletionPort(
				(HANDLE)newsocket,
				ghCompletionPort,
				(DWORD)pKey,
				0
				);
		unsigned char addr[4];
		addr[0] = clientAddr.sin_addr.S_un.S_un_b.s_b1;
		addr[1] = clientAddr.sin_addr.S_un.S_un_b.s_b2;
		addr[2] = clientAddr.sin_addr.S_un.S_un_b.s_b3;
		addr[3] = clientAddr.sin_addr.S_un.S_un_b.s_b4;

		unsigned short port;
		port = clientAddr.sin_port;

		fprintf(stderr, "new connect from %d.%d.%d.%d:%d\r\n",addr[0],addr[1],addr[2],addr[3],port);

		IssueRead(pKey);
	}

	return 0;
}
