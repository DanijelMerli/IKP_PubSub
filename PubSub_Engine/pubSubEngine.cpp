#include "pubSubLibrary.h"

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