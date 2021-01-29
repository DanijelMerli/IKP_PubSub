#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>

#define SERVER_ADDR "127.0.0.1"
#define PUB_PORT "10000"
#define SUB_PORT "10001"
#define PUB_PORT_INT 10000
#define SUB_PORT_INT 10001
#define DEFAULT_BUFLEN 4096
#define MAX_TOPICLEN 512
#define MAX_MSGLEN 2048

typedef struct PublisherData_st
{
	char topic[MAX_TOPICLEN];
	char message[MAX_MSGLEN];
}PublisherData;

// connect to PubSub Engine
// port - port of server socket (publisher-10000 / subscriber-10001)
// returns socket handle if successful 
// returns INVALID_SOCKET if an error occured
SOCKET connect(DWORD port);

// Initialize windows sockets library
// returns true if successful
// returns false if error
bool initializeWindowsSockets(); 
