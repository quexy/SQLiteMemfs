#pragma unmanaged

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sqlite3.h"

#include "memfs.h"
#include "memfs_sync.h"
#include "vfs_object.h"
#include "file_object.h"



static int refCount = 0;


MEMFS_EXTERN int memfs_init()
{
    int result;
    memfs_sync_enter();

    ++refCount;
    if (refCount == 1)
        sqlite3_vfs_register(get_vfs_object(), 1);
    result = refCount - 1;

    memfs_sync_exit();
    return result;
}


MEMFS_EXTERN int memfs_destroy()
{
    int result;
    memfs_file_data* pData;
    memfs_sync_enter();

    --refCount;
    if (refCount == 0)
    {
        sqlite3_vfs* pVfs = get_vfs_object();
        sqlite3_vfs_unregister(pVfs);

        file_list_item* pHead = (file_list_item*)pVfs->pAppData;
        for (file_list_item* ptr = pHead->pNext; ptr != pHead; ptr = ptr->pNext)
        {
            pData = (memfs_file_data*)(ptr->pObject);
            ptr->pPrev->pNext = ptr->pNext;
            ptr->pNext->pPrev = ptr->pPrev;

            void* temp = ptr;
            ptr = ptr->pPrev;
            free(temp);

            assert(pData->nRef == 0);
            destroy_file(pData);
        }
    }
    result = refCount;

    memfs_sync_exit();
    return result;
}


MEMFS_EXTERN __int64 memfs_getsize(const char* zName)
{
    memfs_file_data* pData = find_file_data(get_vfs_object(), zName);
    if (pData == NULL || pData->iDeleted == 1)
        return 0;
    else // found and not deleted
        return pData->nSize;
}


MEMFS_EXTERN bool memfs_setsize(const char* zName, __int64 nSize)
{
    int result, flags;
    sqlite3_file* pFile = NULL;
    result = flags = 0;

    sqlite3_vfs* pVfs = get_vfs_object();
    pFile = (sqlite3_file*)malloc(pVfs->szOsFile);
    if (pFile == NULL) return false;

    result = pVfs->xOpen(pVfs, zName, pFile, SQLITE_OPEN_CREATE, &flags);
    if (result != SQLITE_OK) { free(pFile); return false; }

    while (pFile->pMethods->xLock(pFile, SQLITE_LOCK_SHARED) != SQLITE_OK) sqlite3_sleep(10);
    while (pFile->pMethods->xLock(pFile, SQLITE_LOCK_EXCLUSIVE) != SQLITE_OK) sqlite3_sleep(10);

    result = pFile->pMethods->xTruncate(pFile, nSize);

    pFile->pMethods->xUnlock(pFile, SQLITE_LOCK_SHARED);
    pFile->pMethods->xUnlock(pFile, SQLITE_LOCK_NONE);
    pFile->pMethods->xClose(pFile);
    free(pFile);

    return (result == SQLITE_OK) ? true : false;
}


MEMFS_EXTERN int memfs_readdata(const char* zName, void* data, int nSize, __int64 iOfst)
{
    int result, flags;
    sqlite3_file* pFile = NULL;
    flags = 0; result = 0;

    sqlite3_vfs* pVfs = get_vfs_object();
    pFile = (sqlite3_file*)malloc(pVfs->szOsFile);
    if (pFile == NULL) return result;

    get_vfs_object()->xOpen(get_vfs_object(), zName, (sqlite3_file*)pFile, SQLITE_OPEN_CREATE, &flags);
    if (result != SQLITE_OK) { free(pFile); return result; }

    while (pFile->pMethods->xLock(pFile, SQLITE_LOCK_SHARED) != SQLITE_OK) sqlite3_sleep(10);

    result = pFile->pMethods->xRead(pFile, data, nSize, iOfst);
    if (result == SQLITE_OK) { result = nSize; }
    else if (result == SQLITE_IOERR_SHORT_READ)
        result = (int)(((file_object*)pFile)->pData->nSize - iOfst);

    pFile->pMethods->xUnlock(pFile, SQLITE_LOCK_NONE);

    pFile->pMethods->xClose(pFile);
    free(pFile);

    return result;
}


MEMFS_EXTERN int memfs_writedata(const char* zName, void* data, int nSize, __int64 iOfst)
{
    int result, flags;
    sqlite3_file* pFile = NULL;
    flags = 0; result = 0;

    sqlite3_vfs* pVfs = get_vfs_object();
    pFile = (sqlite3_file*)malloc(pVfs->szOsFile);
    if (pFile == NULL) return result;

    get_vfs_object()->xOpen(get_vfs_object(), zName, (sqlite3_file*)pFile, SQLITE_OPEN_CREATE, &flags);
    if (result != SQLITE_OK) { free(pFile); return result; }

    while (pFile->pMethods->xLock(pFile, SQLITE_LOCK_SHARED) != SQLITE_OK) sqlite3_sleep(10);
    while (pFile->pMethods->xLock(pFile, SQLITE_LOCK_EXCLUSIVE) != SQLITE_OK) sqlite3_sleep(10);

    result = pFile->pMethods->xWrite(pFile, data, nSize, iOfst);
    if (result == SQLITE_OK) { result = nSize; }

    pFile->pMethods->xUnlock(pFile, SQLITE_LOCK_SHARED);
    pFile->pMethods->xUnlock(pFile, SQLITE_LOCK_NONE);

    pFile->pMethods->xClose(pFile);
    free(pFile);

    return result;
}


MEMFS_EXTERN int memfs_delfile(const char* zName)
{
    sqlite3_vfs* pVfs = get_vfs_object();
    return pVfs->xDelete(pVfs, zName, 0);
}
