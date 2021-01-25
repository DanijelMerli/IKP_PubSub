#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "../Common/connect.h"

typedef struct
{
    SOCKET socket;
} PerHandleData;

// Typedef definition
typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[DEFAULT_BUFLEN];
	DWORD BytesSEND;
	DWORD BytesRECV;
} PerIoData;

// accepts publisher connections 
// adds accepted socket handles to IOCP
// returns -1 if error
DWORD WINAPI pubAccept(LPVOID completionPort);

// accepts subscriber connections 
// adds accepted socket handles to IOCP
// returns -1 if error
DWORD WINAPI subAccept(LPVOID completionPort);