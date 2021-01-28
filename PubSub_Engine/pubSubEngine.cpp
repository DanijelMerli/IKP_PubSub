#include "../Common/connect.h"
#include "pubSubLibrary.h"
#include "../Publisher/publish.h"


DWORD WINAPI serverWorkerThread(LPVOID lpParam);

int main()
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    HANDLE completionPort;
    HANDLE threadHandle;

    if (initializeWindowsSockets() == false)
    {
        // not logging anything cause the function itself logs errors 
        return 1;
    }

    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (completionPort == NULL)
    {
        printf("CreateIoCompletionPort failed with error: %d\n", GetLastError());
        WSACleanup();
        return 1;
    }

    for (int i = 0; i < systemInfo.dwNumberOfProcessors; i++)
    {
        if ((threadHandle = CreateThread(NULL, 0, serverWorkerThread, completionPort, 0, NULL)) == NULL)
        {
            printf("CreateThread failed with error: %d", GetLastError());
            return 1;
        }

        CloseHandle(threadHandle);
    }

    HANDLE pubAcceptThread = CreateThread(NULL, 0, pubAccept, completionPort, 0, NULL);
    HANDLE subAcceptThread = CreateThread(NULL, 0, subAccept, completionPort, 0, NULL);

    getchar();

    CloseHandle(pubAcceptThread);
    CloseHandle(subAcceptThread);
    WSACleanup();
    return 0;
}

DWORD WINAPI serverWorkerThread(LPVOID CompletionPortID)
{
	HANDLE CompletionPort = (HANDLE)CompletionPortID;
	DWORD BytesTransferred;
	PerHandleData* perHandleData;
	PerIoData* perIoData;
	DWORD SendBytes, RecvBytes;
	DWORD Flags;
    

    
	while (TRUE)
	{
		if (GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&perHandleData, (LPOVERLAPPED*)&perIoData, INFINITE) == 0)
		{
			printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
			return 0;
		}

        // PUBLISHER
        if (perHandleData->type == Publisher)
        {
            PublisherData* pubdata = (PublisherData*)perIoData->Buffer;
            printf("Message from Publisher %d:\n Topic: %s\n Message:\n", perHandleData->socket, pubdata->message);
        }
        // SUBSCRIBER
        else if (perHandleData->type == Subscriber)
        {
            printf("Message from Subscriber %d:\n%s\n", perHandleData->socket, perIoData->Buffer);
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