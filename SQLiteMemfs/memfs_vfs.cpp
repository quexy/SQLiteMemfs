#pragma unmanaged

#include <stdlib.h>
#include <string.h>

#include <Windows.h>

#include "sqlite3.h"

#include "file_object.h"
#include "memfs_file_data.h"
#include "memfs_file_io.h"
#include "memfs_vfs.h"



/* own function implementations */

int memfs_vfs_Open(sqlite3_vfs* pVfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags)
{
    int result;
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
        pObject->pData = (memfs_file_data*)malloc(sizeof(memfs_file_data));
        if (pObject->pData == NULL) return SQLITE_NOMEM;

        result = create_file_data(pObject->pData, zName);
        if (result != SQLITE_OK) { free(pObject->pData); return result; }

        pObject->pData->pNext = (memfs_file_data*)pVfs->pAppData;
        pVfs->pAppData = pObject->pData;
    }
    pObject->base.pMethods = get_io_methods();
    pObject->pData->iDeleted = 0;
    pObject->pData->nRef += 1;

    return SQLITE_OK;
}


int memfs_vfs_Delete(sqlite3_vfs* pVfs, const char* zName, int syncDir)
{
    memfs_file_data* pPrev;
    memfs_file_data* pFile;

    pPrev = NULL;
    pFile = (memfs_file_data*)pVfs->pAppData;
    while (pFile != NULL)
    {
        if (strcmp(zName, pFile->zName) == 0)
        {
            if (pFile->nRef == 0)
            {
                if (pPrev != NULL)
                    pPrev->pNext = pFile->pNext;
                else // no prev
                    pVfs->pAppData = pFile->pNext;
                destroy_file(pFile);
            }
            else
            {
                delete_file_data(pFile);
            }
            return SQLITE_OK;
        }
        pPrev = pFile;
        pFile = pFile->pNext;
    }
    return SQLITE_OK;
}


int memfs_vfs_Access(sqlite3_vfs* pVfs, const char* zName, int flags, int* pResOut)
{
    if (flags == 0 || (flags & SQLITE_ACCESS_EXISTS) == SQLITE_ACCESS_EXISTS)
        *pResOut = (find_file_data(pVfs, zName) != NULL);
    else // not a file exsistence check
        *pResOut = 1;
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


void (*memfs_vfs_DlSym(sqlite3_vfs* pVfs, void* zBuf, const char* zName))(void)
{
    return pVfs->pNext->xDlSym(pVfs->pNext, zBuf, zName);
}


void memfs_vfs_DlClose(sqlite3_vfs* pVfs, void* zBuf)
{
    pVfs->pNext->xDlClose(pVfs->pNext, zBuf);
}
