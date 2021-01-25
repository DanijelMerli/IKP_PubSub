#include "pubSubLibrary.h"


DWORD WINAPI pubAccept(LPVOID completionPortId)
{
    HANDLE completionPort = (HANDLE)completionPortId;

    // Prepare address information structures
    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    // store function return value
    int iResult;

    SOCKET acceptSocket = INVALID_SOCKET;
    SOCKET listenSocket = INVALID_SOCKET;

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
        return -1;
    }

    // Create a SOCKET for publishers to connect to server
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return -1;
    }

    // Setup the TCP publisher listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    // Set listenSocket in listening mode 
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    printf("Accepting new publisher connections...\n");

	while (TRUE)
	{
        PerHandleData* perHandleData = NULL;
        SOCKADDR_IN sockAddr;
        int addrLen = sizeof(sockAddr);

        // Accept publisher socket connection
        acceptSocket = accept(listenSocket, (sockaddr*)&sockAddr, &addrLen);
        if (WSAGetLastError() != WSAEWOULDBLOCK && acceptSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            WSACleanup();
            return -1;
        }

        // perHandleData = (PerHandleData*)GlobalAlloc(GPTR, sizeof(PerHandleData))
        perHandleData = (PerHandleData*)malloc(sizeof(PerHandleData));
        printf("Publisher connected.\n");

        perHandleData->socket = acceptSocket;
        memcpy(&perHandleData->clientAddr, &sockAddr, addrLen);

        if (CreateIoCompletionPort((HANDLE)acceptSocket, completionPort, (DWORD)perHandleData, 0) == NULL)
        {
            printf("CreateIoCompletionPort failed with error %d\n", GetLastError());
            return -1;
        }
	}
}

DWORD WINAPI subAccept(LPVOID completionPortId)
{
    HANDLE completionPort = (HANDLE)completionPortId;

    // Prepare address information structures
    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    // store function return value
    int iResult;

    SOCKET acceptSocket = INVALID_SOCKET;
    SOCKET listenSocket = INVALID_SOCKET;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port for publishers
    iResult = getaddrinfo(NULL, SUB_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return -1;
    }

    // Create a SOCKET for subscribers to connect to server
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return -1;
    }

    // Setup the TCP subscriber listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    // Set listenSocket in listening mode 
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    printf("Accepting new subscriber connections...\n");

    while (TRUE)
    {
        PerHandleData* perHandleData = NULL;
        SOCKADDR_IN sockAddr;
        int addrLen = sizeof(sockAddr);

        // Accept subscribersocket connection
        acceptSocket = accept(listenSocket, (sockaddr*)&sockAddr, &addrLen);
        if (acceptSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            WSACleanup();
            return -1;
        }
            

        // perHandleData = (PerHandleData*)GlobalAlloc(GPTR, sizeof(PerHandleData))
        perHandleData = (PerHandleData*)malloc(sizeof(PerHandleData));
        printf("Subscriber connected.\n");

        perHandleData->socket = acceptSocket;
        memcpy(&perHandleData->clientAddr, &sockAddr, addrLen);

        if (CreateIoCompletionPort((HANDLE)acceptSocket, completionPort, (DWORD)perHandleData, 0) == NULL)
        {
            printf("CreateIoCompletionPort failed with error %d\n", GetLastError());
            return -1;
        }
    }
}