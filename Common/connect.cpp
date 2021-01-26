#pragma once
#include "connect.h"

SOCKET connect(DWORD port)
{
    SOCKET connectSocket = INVALID_SOCKET;

    if (initializeWindowsSockets() == false)
    {
        // not logging anything cause the function itself logs errors 
        return INVALID_SOCKET;
    }

    // create a socket
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serverAddress.sin_port = htons(port);

    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server. Error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        return INVALID_SOCKET;
    }

    return connectSocket;
}

bool initializeWindowsSockets()
{
    WSADATA wsaData;
    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}
