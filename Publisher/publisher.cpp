#include "../Common/connect.h"
#include "publish.h"

int main()
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;

    PublisherData pd;
    
    char* temp = (char*)malloc(sizeof(char*));
    char* temp1 = (char*)malloc(sizeof(char*));

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
        if (fgets(temp, sizeof(char*), stdin) == NULL)
        {
            printf("fgets failed with error.\n");
            closesocket(connectSocket);
            return -1;
        }
        
        // trim newline at end
        if (temp[strlen(temp) - 1] == '\n')
            temp[strlen(temp) - 1] = 0;

        strcpy_s(pd.topic, temp);
        

        printf("Message: \n");
        if (fgets(temp1, sizeof(char*), stdin) == NULL) 
        {
            printf("fgets failed with error.\n");
            closesocket(connectSocket);
            
        }
        if (temp1[strlen(temp1) - 1] == '\n')
            temp1[strlen(temp1) - 1] == 0;

        strcpy_s(pd.message, temp1);


        iResult = sendto(connectSocket, (char*)&pd, sizeof(pd), 0,(LPSOCKADDR)&connectSocket, sizeof(connectSocket));
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