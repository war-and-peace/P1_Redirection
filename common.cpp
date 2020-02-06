#include "common.h"

DWORD WINAPI InstanceThreadRead(LPVOID lpvParam)
{
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	if (lpvParam == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf("   InstanceThread exitting.\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	printf("InstanceThreadRead created, receiving and processing messages.\n"); 

	hPipe = (HANDLE)lpvParam;

	while (1)
	{
		fSuccess = ReadFile(
			hPipe,        // handle to pipe 
			pchRequest,    // buffer to receive data 
			sizeof(TCHAR), // size of buffer 
			&cbBytesRead, // number of bytes read 
			NULL);        // not overlapped I/O 

		if (!fSuccess || cbBytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				_tprintf(TEXT("InstanceThread: client disconnected.\n"));
			}
			else
			{
				_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
			}
			break;
		}

		printf("%s", pchRequest);
	}


	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);

	printf("InstanceThreadRead exiting.\n");
	return 1;
}

DWORD WINAPI InstanceThreadWrite(LPVOID lpvParam)
{
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	if (lpvParam == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;
	}

	printf("InstanceThreadWrite created, receiving and processing messages.\n");

	hPipe = (HANDLE)lpvParam;

	while (1)
	{
	
		TCHAR chInput = (TCHAR)getc(stdin);
		printf("Character received: %c\n", (char)chInput);

		pchReply[0] = chInput;
		pchReply[1] = '\0';

		printf("Writing to the pipe...\n");

		fSuccess = WriteFile(
			hPipe,        // handle to pipe 
			pchReply,     // buffer to write from 
			sizeof(TCHAR), // number of bytes to write 
			&cbWritten,   // number of bytes written 
			NULL);        // not overlapped I/O 

		printf("Finished writing!\n");

		if (!fSuccess || cbReplyBytes != cbWritten)
		{
			_tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
			break;
		}
	}

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchReply);

	printf("InstanceThreadWrite exiting.\n");
	return 1;
}

int redirectionWithPipe(HANDLE hPipe)
{
	HANDLE hThreadRead = nullptr, hThreadWrite = nullptr;
	DWORD dwThreadReadId = 0, dwThreadWriteId = 0;

	hThreadRead = CreateThread(
		NULL,              // no security attribute 
		0,                 // default stack size 
		InstanceThreadRead,    // thread proc
		(LPVOID)hPipe,    // thread parameter 
		0,                 // not suspended 
		&dwThreadReadId);      // returns thread ID 

	if (hThreadRead == NULL) {
		_tprintf(TEXT("CreateThread for Read Thread failed, GLE=%d.\n"), GetLastError());
		return -1;
	}

	hThreadWrite = CreateThread(
		NULL,
		0,
		InstanceThreadWrite,
		(LPVOID)hPipe,
		0,
		&dwThreadWriteId
	);

	if (hThreadWrite == NULL) {
		_tprintf(TEXT("CreateThread for Write Thread failed, GLE=%d.\n"), GetLastError());
		return -1;
	}

	printf("Read and Write Threads has been successfully created!\n");
	WaitForSingleObject(hThreadWrite, INFINITE);
	WaitForSingleObject(hThreadRead, INFINITE);
	/*CloseHandle(hThreadRead);
	CloseHandle(hThreadWrite)*/;

	return 0;
}
