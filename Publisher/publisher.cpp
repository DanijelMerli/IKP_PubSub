#include "../Common/connect.h"

int main()
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    PublisherData pd;
    
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
        if (fgets(pd.topic, MAX_TOPICLEN, stdin) == NULL)
        {
            printf("fgets failed with error.\n");
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
        
        // trim newline at end
        if (pd.topic[strlen(pd.topic) - 1] == '\n')
            pd.topic[strlen(pd.topic) - 1] = 0;
        

        printf("Message: \n");
        if (fgets(pd.message, MAX_MSGLEN, stdin) == NULL) 
        {
            printf("fgets failed with error.\n");
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
        if (pd.message[strlen(pd.message) - 1] == '\n')
            pd.message[strlen(pd.message) - 1] = 0;

        iResult = send(connectSocket, (char*)&pd, sizeof(PublisherData), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    }

    closesocket(connectSocket);
    WSACleanup();
    return 0;
}