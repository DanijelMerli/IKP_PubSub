#include "../Common/connect.h"
#include "publish.h"

int main()
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    char* sendbuff = (char*)malloc(DEFAULT_BUFLEN);
    // variable used to store function return value
    int iResult;

    if ((connectSocket = connect(PUB_PORT_INT)) == INVALID_SOCKET)
    {
        // not logging anything cause the function itself logs errors 
        WSACleanup();
        return 1;
    }

    while (TRUE)
    {
        printf("Topic: \n");
        if (fgets(sendbuff, DEFAULT_BUFLEN, stdin) == NULL)
        {
            printf("fgets failed with error.\n");
            closesocket(connectSocket);
            return -1;
        }
        
        // trim newline at end
        if (sendbuff[strlen(sendbuff) - 1] == '\n')
            sendbuff[strlen(sendbuff) - 1] = 0;

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