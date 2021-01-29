#pragma once
#include <WS2tcpip.h>
#include "../Common/connect.h"
#include "../Common/hashtable.h"

enum SocketType 
{
	Publisher, 
	Subscriber
};

typedef struct
{
    SOCKET socket;
	SocketType type;
} PerHandleData;

typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[DEFAULT_BUFLEN];
	DWORD BytesSEND;
	DWORD BytesRECV;
} PerIoData;

typedef struct
{
	HANDLE completionPort;
	HashTable* table;
} ThreadArgs;

// accepts publisher connections 
// adds accepted socket handles to IOCP
// returns -1 if error
DWORD WINAPI pubAccept(LPVOID completionPort);

// accepts subscriber connections 
// adds accepted socket handles to IOCP
// returns -1 if error
DWORD WINAPI subAccept(LPVOID completionPort);