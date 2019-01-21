#pragma unmanaged

#include <stdlib.h>

#include <Windows.h>

#include "memfs_sync.h"
#include "critical_section.h"

static VOID* pMemfsMutex = NULL;



void memfs_sync_enter()
{
    enter_critical_section(pMemfsMutex);
}


void memfs_sync_exit()
{
    leave_critical_section(pMemfsMutex);
}



BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason, LPVOID lpReserved)
{
    switch (ul_reason)
    {
        case DLL_PROCESS_ATTACH:
            pMemfsMutex = create_critical_section();
            return TRUE;
        case DLL_THREAD_ATTACH:
            return TRUE;
        case DLL_THREAD_DETACH:
            return TRUE;
        case DLL_PROCESS_DETACH:
            destroy_critical_section(pMemfsMutex);
            pMemfsMutex = NULL;
            return TRUE;
    }
    return TRUE;
}
