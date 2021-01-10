#include "../Common/connect.h"
#include <WS2tcpip.h>

int main()
{
    SOCKET pubListenSocket = INVALID_SOCKET;
    SOCKET subListenSocket = INVALID_SOCKET;

    // store function return value
    int iResult;

    if (initializeWindowsSockets() == false)
    {
        // not logging anything cause the function itself logs errors 
        return 1;
    }

    // Prepare address information structures
    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port for publishers
    iResult = getaddrinfo(NULL, PUB_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    // Create a SOCKET for publishers to connect to server
    pubListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pubListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }
    // Setup the TCP publisher listening socket - bind port number and local address to socket
    iResult = bind(pubListenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(pubListenSocket);
        WSACleanup();
        return 1;
    }

    // Resolve the server address and port for subscribers
    iResult = getaddrinfo(NULL, SUB_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo 2 failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    // Create a SOCKET for subscribers to connect to server
    subListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (subListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }
    // Setup the TCP subscriber listening socket - bind port number and local address to socket
    iResult = bind(subListenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(pubListenSocket);
        closesocket(subListenSocket);
        WSACleanup();
        return 1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    unsigned long int nonBlocking = 1;

    // Set pubListenSocket in listening mode 
    iResult = listen(pubListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(pubListenSocket);
        closesocket(subListenSocket);
        WSACleanup();
        return 1;
    }
    // Set pubListenSocket to nonblocking
    iResult = ioctlsocket(pubListenSocket, FIONBIO, &nonBlocking);
    if (iResult == SOCKET_ERROR)
    {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(pubListenSocket);
        closesocket(subListenSocket);
        WSACleanup();
        return 1;
    }

    // Set pubListenSocket in listening mode
    iResult = listen(subListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(pubListenSocket);
        closesocket(subListenSocket);
        WSACleanup();
        return 1;
    }
    // Set subListenSocket to nonblocking
    iResult = ioctlsocket(subListenSocket, FIONBIO, &nonBlocking);
    if (iResult == SOCKET_ERROR)
    {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(pubListenSocket);
        closesocket(subListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept publisher socket connection
    SOCKET pubSocket = accept(pubListenSocket, NULL, NULL);
    if (WSAGetLastError() != WSAEWOULDBLOCK && pubSocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(pubListenSocket);
        closesocket(subListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept subscriber socket connection
    SOCKET subSocket = accept(subListenSocket, NULL, NULL);
    if (WSAGetLastError() != WSAEWOULDBLOCK && subSocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(pubListenSocket);
        closesocket(subListenSocket);
        WSACleanup();
        return 1;
    }

    getchar();

    closesocket(pubListenSocket);
    closesocket(subListenSocket);
    WSACleanup();
    return 0;
}