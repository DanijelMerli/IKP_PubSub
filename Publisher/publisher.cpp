#include "../Common/connect.h"
#include "publish.h"

int main()
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    char* sendbuff = (char*)malloc(DEFAULT_BUFLEN);
    // variable used to store function return value
    int iResult;

    if (initializeWindowsSockets() == false)
    {
        // not logging anything cause the function itself logs errors 
        return 1;
    }

    // create a socket
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serverAddress.sin_port = htons(PUB_PORT_INT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    while (TRUE)
    {
        printf("Send message: \n");
        fgets(sendbuff, DEFAULT_BUFLEN, stdin);
        iResult = send(connectSocket, sendbuff, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
        }
    }

    getchar();

    closesocket(connectSocket);
    WSACleanup();
    return 0;
}