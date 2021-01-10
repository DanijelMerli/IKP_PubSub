#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>

#define SERVER_ADDR "127.0.0.1"
#define PUB_PORT "10000"
#define SUB_PORT "10001"
#define PUB_PORT_INT 10000
#define SUB_PORT_INT 10001

// connect to PubSub Engine
// port - port of server socket (publisher-10000 / subscriber-10001)
// returns true if successful 
// returns false if an error occured
bool connect(PCSTR port);

// Initialize windows sockets library
bool initializeWindowsSockets(); 
