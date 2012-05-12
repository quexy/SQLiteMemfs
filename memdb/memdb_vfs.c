#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Windows.h>

#include "memdb_file_data.h"
#include "memdb_file_io.h"
#include "file_object.h"
#include "memdb_vfs.h"


/* own function implementations */

int memdb_vfs_Open(sqlite3_vfs* pVfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags)
{
    int result;
    file_object* pObject = (file_object*)pFile;

    memset(pObject, 0, sizeof(file_object));

    *pOutFlags = 0;
    if (zName == 0) return SQLITE_IOERR;

    pObject->base.pMethods = get_io_methods();
    pObject->pData = find_file_data(pVfs, zName);
    if (pObject->pData == NULL)
    {
        pObject->pData = (memdb_file_data*)malloc(sizeof(memdb_file_data));
        if(pObject->pData == NULL) return SQLITE_NOMEM;

        result = create_file_data(pObject->pData, zName);
        if (result != SQLITE_OK) return result;

        pObject->pData->pNext = (memdb_file_data*)pVfs->pAppData;
        pVfs->pAppData = pObject->pData;
    }
    pObject->pData->nRef += 1;
    
    return SQLITE_OK;
}

int memdb_vfs_Delete(sqlite3_vfs* pVfs, const char* zName, int syncDir)
{
    memdb_file_data* pPrev;
    memdb_file_data* pFile;

    pPrev = NULL;
    pFile = (memdb_file_data*)pVfs->pAppData;
    while(pFile != NULL)
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

int memdb_vfs_Access(sqlite3_vfs* pVfs, const char* zName, int flags, int* pResOut)
{
    *pResOut = 1;
    return SQLITE_OK;
}

int memdb_vfs_FullPathname(sqlite3_vfs* pVfs, const char* zName, int nOut, char* zOut)
{
    strcpy_s(zOut, nOut, zName);
    return SQLITE_OK;
}



/* system API calls: pVfs->pNext delegation */

int memdb_vfs_Randomness(sqlite3_vfs* pVfs, int nByte, char* zOut)
{
    return pVfs->pNext->xRandomness(pVfs->pNext, nByte, zOut);
}

int memdb_vfs_Sleep(sqlite3_vfs* pVfs, int microsec)
{
    return pVfs->pNext->xSleep(pVfs->pNext, microsec);
}

int memdb_vfs_CurrentTime(sqlite3_vfs* pVfs, double* prNow)
{
    return pVfs->pNext->xCurrentTime(pVfs->pNext, prNow);
}

int memdb_vfs_GetLastError(sqlite3_vfs* pVfs, int nBuf, char* zBuf)
{
    return pVfs->pNext->xGetLastError(pVfs->pNext, nBuf, zBuf);
}

int memdb_vfs_CurrentTimeInt64(sqlite3_vfs* pVfs, sqlite3_int64* piNow)
{
    return pVfs->pNext->xCurrentTimeInt64(pVfs->pNext, piNow);
}



/* loadable modules: pVfs->pNext delegation */

void* memdb_vfs_DlOpen(sqlite3_vfs* pVfs, const char* zBuf)
{
    return pVfs->pNext->xDlOpen(pVfs->pNext, zBuf);
}

void memdb_vfs_DlError(sqlite3_vfs* pVfs, int nByte, char* zErrMsg)
{
    pVfs->pNext->xDlError(pVfs->pNext, nByte, zErrMsg);
}

void (*memdb_vfs_DlSym(sqlite3_vfs* pVfs, void* zBuf, const char* zName))(void)
{
    return pVfs->pNext->xDlSym(pVfs->pNext, zBuf, zName);
}

void memdb_vfs_DlClose(sqlite3_vfs* pVfs, void* zBuf)
{
    pVfs->pNext->xDlClose(pVfs->pNext, zBuf);
}
