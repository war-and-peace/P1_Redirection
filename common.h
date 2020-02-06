#pragma once

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>

#define BUFSIZE 512

DWORD WINAPI InstanceThreadRead(LPVOID lpvParam);
DWORD WINAPI InstanceThreadWrite(LPVOID lpvParam);
int redirectionWithPipe(HANDLE hPipe);