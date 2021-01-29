#include "pubSubLibrary.h"
#include "../Publisher/publish.h"


DWORD WINAPI serverWorkerThread(LPVOID lpParam);

int main()
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    HANDLE threadHandle;
    ThreadArgs args;
    
    args.table = table_init();
    if (args.table == NULL)
    {
        printf("table_init failed\n");
        return 1;
    }

    if (initializeWindowsSockets() == false)
    {
        // not logging anything cause the function itself logs errors 
        return 1;
    }

    args.completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (args.completionPort == NULL)
    {
        printf("CreateIoCompletionPort failed with error: %d\n", GetLastError());
        WSACleanup();
        return 1;
    }

    for (int i = 0; i < systemInfo.dwNumberOfProcessors; i++)
    {
        if ((threadHandle = CreateThread(NULL, 0, serverWorkerThread, (LPVOID)&args, 0, NULL)) == NULL)
        {
            printf("CreateThread failed with error: %d", GetLastError());
            return 1;
        }

        CloseHandle(threadHandle);
    }

    HANDLE pubAcceptThread = CreateThread(NULL, 0, pubAccept, args.completionPort, 0, NULL);
    HANDLE subAcceptThread = CreateThread(NULL, 0, subAccept, args.completionPort, 0, NULL);

    getchar();

    CloseHandle(pubAcceptThread);
    CloseHandle(subAcceptThread);
    WSACleanup();
    return 0;
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
    
	while (TRUE)
	{
		if (GetQueuedCompletionStatus(completionPort, &BytesTransferred, (PULONG_PTR)&perHandleData, (LPOVERLAPPED*)&perIoData, INFINITE) == 0)
		{
			printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
			return 0;
		}

        // PUBLISHER
        if (perHandleData->type == Publisher)
        {
            PublisherData* pubdata = (PublisherData*)perIoData->Buffer;
            printf("Message from Publisher %d:\n Topic: %s\n Message: %s\n", perHandleData->socket, pubdata->topic, pubdata->message);
        }
        // SUBSCRIBER
        else if (perHandleData->type == Subscriber)
        {
            printf("Subscriber %d requesting subscription to topic %s\n", perHandleData->socket, perIoData->Buffer);
            if (table_hasKey(table, perIoData->Buffer))
            {
                if (table_add(table, perIoData->Buffer, perHandleData->socket))
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

                if (send(perHandleData->socket, "", 1, 0) == SOCKET_ERROR)
                {
                    printf("error while sending report to subscriber %d\n", perHandleData->socket);
                }
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

		if (WSARecv(perHandleData->socket, &(perIoData->DataBuf), 1, &RecvBytes, &Flags,
					&(perIoData->Overlapped), NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				printf("WSARecv() failed with error %d\n", WSAGetLastError());
				return 0;
			}
		}
	}
}