#pragma unmanaged

#include <stdlib.h>

#include <Windows.h>

#include "memfs_sync.h"



static CRITICAL_SECTION memfs_mutex;


void memfs_sync_enter()
{
    EnterCriticalSection(&memfs_mutex); 
}


void memfs_sync_exit()
{
    LeaveCriticalSection(&memfs_mutex);
}



BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason, LPVOID lpReserved)
{
    switch (ul_reason)
    {
        case DLL_PROCESS_ATTACH:
            InitializeCriticalSection(&memfs_mutex);
            return TRUE;
        case DLL_THREAD_ATTACH:
            return TRUE;
        case DLL_THREAD_DETACH:
            return TRUE;
        case DLL_PROCESS_DETACH:
            DeleteCriticalSection(&memfs_mutex);
            return TRUE;
    }
    return TRUE;
}
