#include "../Common/connect.h"
#include "publish.h"

int main()
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    char* sendbuff = (char*)malloc(DEFAULT_BUFLEN);
    
    char temp3[] = "Poruka";
    PublisherData* pd = (PublisherData*)malloc(sizeof(pd));
   
    char* temp = (char*)malloc(sizeof(pd->message));
    char* temp1 = (char*)malloc(sizeof(pd->topic));

    //pd->message = temp3;
    //pd->topic = temp3;

    
    

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
        if (fgets(temp, sizeof(pd->topic), stdin) == NULL)
        {
            printf("fgets failed with error.\n");
            closesocket(connectSocket);
            return -1;
        }
        
        // trim newline at end
        if (temp[strlen(temp) - 1] == '\n')
            temp[strlen(temp) - 1] = 0;

        pd->topic = temp;
        

        printf("Message: \n");
        if (fgets(temp1, sizeof(pd->message), stdin) == NULL) 
        {
            printf("fgets failed with error.\n");
            closesocket(connectSocket);
            
        }
        if (temp1[strlen(temp1) - 1] == '\n')
            temp1[strlen(temp1) - 1] == 0;

        pd->message = temp1;

        


        iResult = send(connectSocket, (char*)pd, sizeof(pd), 0);
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