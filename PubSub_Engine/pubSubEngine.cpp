#include "../Common/connect.h"
#include "pubSubLibrary.h"

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
		else
			printf("GetQueuedCompletionStatus() is OK!\n");

		// First check to see if an error has occurred on the socket and if so
		// then close the socket and cleanup the SOCKET_INFORMATION structure
		// associated with the socket
		if (BytesTransferred == 0)
		{
			printf("Closing socket %d\n", perHandleData->socket);

			if (closesocket(perHandleData->socket) == SOCKET_ERROR)
			{
				printf("closesocket() failed with error %d\n", WSAGetLastError());
				return 0;
			}
			else
				printf("closesocket() is fine!\n");

			GlobalFree(perHandleData);
			GlobalFree(perIoData);
			continue;
		}

		// Check to see if the BytesRECV field equals zero. If this is so, then
		// this means a WSARecv call just completed so update the BytesRECV field
		// with the BytesTransferred value from the completed WSARecv() call
		if (perIoData->BytesRECV == 0)
		{
			perIoData->BytesRECV = BytesTransferred;
			perIoData->BytesSEND = 0;
		}
		else
		{
			perIoData->BytesSEND += BytesTransferred;
		}

		if (perIoData->BytesRECV > perIoData->BytesSEND)
		{
			// Post another WSASend() request.
			// Since WSASend() is not guaranteed to send all of the bytes requested,
			// continue posting WSASend() calls until all received bytes are sent.
			ZeroMemory(&(perIoData->Overlapped), sizeof(OVERLAPPED));
			perIoData->DataBuf.buf = perIoData->Buffer + perIoData->BytesSEND;
			perIoData->DataBuf.len = perIoData->BytesRECV - perIoData->BytesSEND;

			if (WSASend(perHandleData->socket, &(perIoData->DataBuf), 1, &SendBytes, 0,
						&(perIoData->Overlapped), NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					printf("WSASend() failed with error %d\n", WSAGetLastError());
					return 0;
				}
			}
			else
				printf("WSASend() is OK!\n");
		}
		else
		{
			perIoData->BytesRECV = 0;

			// Now that there are no more bytes to send post another WSARecv() request
			Flags = 0;
			ZeroMemory(&(perIoData->Overlapped), sizeof(OVERLAPPED));
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
			else
				printf("WSARecv() is OK!\n");
		}
	}
}