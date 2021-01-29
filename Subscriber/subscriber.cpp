#include "../Common/connect.h"

int main()
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    char* sendbuff = (char*)malloc(DEFAULT_BUFLEN);
    // variable used to store function return value
    int iResult;

    if ((connectSocket = connect(SUB_PORT_INT)) == INVALID_SOCKET)
    {
        // not logging anything cause the function itself logs errors 
        WSACleanup();
        return 1;
    }

    printf("Topic to subscribe to: \n");

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

    char* recvBuff = (char*)malloc(DEFAULT_BUFLEN);

    while (true)
    {
        iResult = recv(connectSocket, recvBuff, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            break;
        }

        if (strcmp(recvBuff, "") == 0)
        {
            printf("Requested topic not found.\n");
            break;
        }
        else
        {
            printf("Recieved message:\n%s\n", recvBuff);
        }
    }

    getchar();

    closesocket(connectSocket);
    WSACleanup();
	return 0;
}