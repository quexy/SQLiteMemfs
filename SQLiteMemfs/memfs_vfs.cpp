#pragma unmanaged

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sqlite3.h"

#include "file_object.h"
#include "memfs_file_data.h"
#include "memfs_file_io.h"
#include "memfs_vfs.h"
#include "critical_section.h"



/* own function implementations */

int memfs_vfs_Open(sqlite3_vfs* pVfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags)
{
    int result = SQLITE_OK;
    file_object* pObject = (file_object*)pFile;

    memset(pObject, 0, sizeof(file_object));

    if (pOutFlags != NULL) *pOutFlags = 0;
    if (zName == 0) return SQLITE_IOERR;

    pObject->pData = find_file_data(pVfs, zName);
    if ((pObject->pData == NULL || pObject->pData->iDeleted == 1)
        && (flags != 0 && (flags & SQLITE_OPEN_CREATE) == 0))
        return SQLITE_IOERR;

    if (pObject->pData == NULL)
    {
        void* pSyncRoot = ((file_list_item*)pVfs->pAppData)->pObject;
        enter_critical_section(pSyncRoot);

        pObject->pData = find_file_data(pVfs, zName);
        if (pObject->pData == NULL)
        {
            pObject->pData = (memfs_file_data*)malloc(sizeof(memfs_file_data));
            if (pObject->pData == NULL) result = SQLITE_NOMEM;

            if (result == SQLITE_OK) result = init_file_data(pObject->pData, zName);
            if (result == SQLITE_OK) result = add_link((file_list_item*)pVfs->pAppData, pObject->pData);

            if (result != SQLITE_OK) { destroy_file(pObject->pData); }
        }
        
        leave_critical_section(pSyncRoot);
        if (result != SQLITE_OK) return result;
    }
    pFile->pMethods = get_io_methods();
    pObject->pData->iDeleted = 0;
    pObject->pData->nRef += 1;

    result = add_link(pObject->pData->pRefs, pObject);
    if (result != SQLITE_OK) destroy_file(pObject->pData);

    return result;
}


int memfs_vfs_Delete(sqlite3_vfs* pVfs, const char* zName, int syncDir)
{
    memfs_file_data* pFile = find_file_data(pVfs, zName);
    while (pFile == NULL) return SQLITE_OK;

    assert(pFile->nRef == 0);
    del_link((file_list_item*)pVfs->pAppData, pFile);
    destroy_file(pFile);

    return SQLITE_OK;
}


int memfs_vfs_Access(sqlite3_vfs* pVfs, const char* zName, int flags, int* pResOut)
{
    *pResOut = 1; // if not a file exsistence check, then it's okay
    if (flags == SQLITE_ACCESS_EXISTS) *pResOut = (find_file_data(pVfs, zName) != NULL);
    return SQLITE_OK;
}


int memfs_vfs_FullPathname(sqlite3_vfs* pVfs, const char* zName, int nOut, char* zOut)
{
    strcpy_s(zOut, nOut, zName);
    return SQLITE_OK;
}



/* system API calls: pVfs->pNext delegation */

int memfs_vfs_Randomness(sqlite3_vfs* pVfs, int nByte, char* zOut)
{
    return pVfs->pNext->xRandomness(pVfs->pNext, nByte, zOut);
}


int memfs_vfs_Sleep(sqlite3_vfs* pVfs, int microsec)
{
    return pVfs->pNext->xSleep(pVfs->pNext, microsec);
}


int memfs_vfs_CurrentTime(sqlite3_vfs* pVfs, double* prNow)
{
    return pVfs->pNext->xCurrentTime(pVfs->pNext, prNow);
}


int memfs_vfs_GetLastError(sqlite3_vfs* pVfs, int nBuf, char* zBuf)
{
    return pVfs->pNext->xGetLastError(pVfs->pNext, nBuf, zBuf);
}


int memfs_vfs_CurrentTimeInt64(sqlite3_vfs* pVfs, sqlite3_int64* piNow)
{
    return pVfs->pNext->xCurrentTimeInt64(pVfs->pNext, piNow);
}



/* loadable modules: pVfs->pNext delegation */

void* memfs_vfs_DlOpen(sqlite3_vfs* pVfs, const char* zBuf)
{
    return pVfs->pNext->xDlOpen(pVfs->pNext, zBuf);
}


void memfs_vfs_DlError(sqlite3_vfs* pVfs, int nByte, char* zErrMsg)
{
    pVfs->pNext->xDlError(pVfs->pNext, nByte, zErrMsg);
}


void(*memfs_vfs_DlSym(sqlite3_vfs* pVfs, void* zBuf, const char* zName))(void)
{
    return pVfs->pNext->xDlSym(pVfs->pNext, zBuf, zName);
}


void memfs_vfs_DlClose(sqlite3_vfs* pVfs, void* zBuf)
{
    pVfs->pNext->xDlClose(pVfs->pNext, zBuf);
}
