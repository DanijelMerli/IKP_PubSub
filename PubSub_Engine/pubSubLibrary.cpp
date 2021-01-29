#include "pubSubLibrary.h"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             


DWORD WINAPI pubAccept(LPVOID completionPortId)
{
    HANDLE completionPort = (HANDLE)completionPortId;
    char* dataBuff = (char*)malloc(DEFAULT_BUFLEN);

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
        return 1;
    }

    // Create a SOCKET for publishers to connect to server
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP publisher listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
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
        return 1;
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
            return 1;
        }

        printf("Publisher connected.\n");

        if ((perHandleData = (PerHandleData*)GlobalAlloc(GPTR, sizeof(PerHandleData))) == NULL)
        {
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            WSACleanup();
            return 1;
        }

        printf("Socket %d connected\n", acceptSocket);
        perHandleData->socket = acceptSocket;
        perHandleData->type = Publisher;

        if (CreateIoCompletionPort((HANDLE)acceptSocket, completionPort, (ULONG_PTR)perHandleData, 0) == NULL)
        {
            printf("CreateIoCompletionPort failed with error %d\n", GetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            GlobalFree(perHandleData);
            WSACleanup();
            return 1;
        }

        PerIoData* perIoData;

        // Create per I/O socket information structure to associate with the WSARecv call below
        if ((perIoData = (PerIoData*)GlobalAlloc(GPTR, sizeof(PerIoData))) == NULL)
        {
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            GlobalFree(perHandleData);
            WSACleanup();
            return 1;
        }

        ZeroMemory(&(perIoData->Overlapped), sizeof(OVERLAPPED));
        perIoData->BytesSEND = 0;
        perIoData->BytesRECV = 0;
        perIoData->DataBuf.len = DEFAULT_BUFLEN;
        perIoData->DataBuf.buf = perIoData->Buffer;
        DWORD Flags = 0;
        DWORD RecvBytes;

        if (WSARecv(acceptSocket, &(perIoData->DataBuf), 1, &RecvBytes, &Flags, &(perIoData->Overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
            {
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                closesocket(listenSocket);
                closesocket(acceptSocket);
                GlobalFree(perHandleData);
                GlobalFree(perIoData);
                WSACleanup();
                return 1;
            }
        }
	}
}

DWORD WINAPI subAccept(LPVOID completionPortId)
{
    HANDLE completionPort = (HANDLE)completionPortId;
    char* dataBuff = (char*)malloc(DEFAULT_BUFLEN);

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
        return 1;
    }

    // Create a SOCKET for publishers to connect to server
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP publisher listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
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
        return 1;
    }

    printf("Accepting new subscriber connections...\n");

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
            return 1;
        }

        printf("Subscriber connected.\n");

        if ((perHandleData = (PerHandleData*)GlobalAlloc(GPTR, sizeof(PerHandleData))) == NULL)
        {
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            WSACleanup();
            return 1;
        }

        printf("Socket %d connected\n", acceptSocket);
        perHandleData->socket = acceptSocket;
        perHandleData->type = Subscriber;

        if (CreateIoCompletionPort((HANDLE)acceptSocket, completionPort, (ULONG_PTR)perHandleData, 0) == NULL)
        {
            printf("CreateIoCompletionPort failed with error %d\n", GetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            GlobalFree(perHandleData);
            WSACleanup();
            return 1;
        }

        PerIoData* perIoData;

        // Create per I/O socket information structure to associate with the WSARecv call below
        if ((perIoData = (PerIoData*)GlobalAlloc(GPTR, sizeof(PerIoData))) == NULL)
        {
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            GlobalFree(perHandleData);
            WSACleanup();
            return 1;
        }

        ZeroMemory(&(perIoData->Overlapped), sizeof(OVERLAPPED));
        perIoData->BytesSEND = 0;
        perIoData->BytesRECV = 0;
        perIoData->DataBuf.len = DEFAULT_BUFLEN;
        perIoData->DataBuf.buf = perIoData->Buffer;
        DWORD Flags = 0;
        DWORD RecvBytes;

        if (WSARecv(acceptSocket, &(perIoData->DataBuf), 1, &RecvBytes, &Flags, &(perIoData->Overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
            {
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                closesocket(listenSocket);
                closesocket(acceptSocket);
                GlobalFree(perHandleData);
                GlobalFree(perIoData);
                WSACleanup();
                return 1;
            }
        }
    }
}

DWORD WINAPI serverWorkerThread(LPVOID args)
{
    HANDLE completionPort = ((ThreadArgs*)args)->completionPort;
    HashTable* table = ((ThreadArgs*)args)->table;
    DWORD BytesTransferred;
    PerHandleData* perHandleData;
    PerIoData* perIoData;
    DWORD SendBytes, RecvBytes;
    DWORD Flags;
    bool keyExists;
    List* subscribers;

    while (TRUE)
    {
        if (GetQueuedCompletionStatus(completionPort, &BytesTransferred, (PULONG_PTR)&perHandleData, (LPOVERLAPPED*)&perIoData, INFINITE) == 0)
        {
            printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
            return 1;
        }

        // PUBLISHER
        if (perHandleData->type == Publisher)
        {
            PublisherData* pubdata = (PublisherData*)perIoData->Buffer;
            printf("Message from Publisher %d:\n Topic: %s\n Message: %s\n", perHandleData->socket, pubdata->topic, pubdata->message);

            if (!table_hasKey(table, pubdata->topic))
            {
                printf("First message for topic %s, creating topic...\n", pubdata->topic);

                if (!table_addList(table, pubdata->topic))
                {
                    printf("Failed to create topic %s.\n", pubdata->topic);
                }
            }
            else
            {
                if ((subscribers = table_get(table, pubdata->topic)) == NULL)
                {
                    printf("Error reading list of subscribers.\n");
                }

                int i = 0;
                ListItem* current = subscribers->head;
                while (current != NULL)
                {
                    ZeroMemory(&(perIoData->Overlapped), sizeof(OVERLAPPED));
                    perIoData->BytesRECV = 0;
                    perIoData->DataBuf.len = strlen(pubdata->message) + 1;
                    perIoData->DataBuf.buf = pubdata->message;

                    if (send(current->data, pubdata->message, strlen(pubdata->message) + 1, 0) == SOCKET_ERROR)
                    {
                        printf("send failed with error %d\n", WSAGetLastError());
                        return 1;
                    }

                    current = current->next;
                    i++;
                }

                printf("Successfully sent message to %d subscribers.\n", i);
            }
        }
        // SUBSCRIBER
        else if (perHandleData->type == Subscriber)
        {
            printf("Subscriber %d requesting subscription to topic %s\n", perHandleData->socket, perIoData->Buffer);
            if (table_hasKey(table, perIoData->Buffer))
            {
                if (table_addSocket(table, perIoData->Buffer, perHandleData->socket))
                {
                    printf("Subscrbier %d subscribed to topic %s successfully\n", perHandleData->socket, perIoData->Buffer);
                }
                else
                {
                    printf("Failed to subscribe subscriber %d to topic %s\n", perHandleData->socket, perIoData->Buffer);
                }
            }
            else
            {
                printf("Specified topic does not exist. Sending report to subscriber %d\n", perHandleData->socket);

                ZeroMemory(&(perIoData->Overlapped), sizeof(OVERLAPPED));
                perIoData->BytesRECV = 0;
                perIoData->DataBuf.len = 1;
                strcpy_s(perIoData->DataBuf.buf, 1, "");

                if (WSASend(perHandleData->socket, &(perIoData->DataBuf), 1, &SendBytes, 0, &(perIoData->Overlapped), NULL) == SOCKET_ERROR)
                {
                    if (WSAGetLastError() != ERROR_IO_PENDING)
                    {
                        printf("WSASend failed with error %d\n", WSAGetLastError());
                        return 1;
                    }
                }

                continue;
            }
        }
        else
        {
            printf("Sender of message did not specify type!\n");
        }


        // Post another WSARecv() request
        Flags = 0;
        ZeroMemory(&(perIoData->Overlapped), sizeof(OVERLAPPED));
        perIoData->BytesRECV = 0;
        perIoData->DataBuf.len = DEFAULT_BUFLEN;
        perIoData->DataBuf.buf = perIoData->Buffer;

        if (WSARecv(perHandleData->socket, &(perIoData->DataBuf), 1, &RecvBytes, &Flags, &(perIoData->Overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
            {
                printf("WSARecv failed with error %d\n", WSAGetLastError());
                return 1;
            }
        }
    }
}