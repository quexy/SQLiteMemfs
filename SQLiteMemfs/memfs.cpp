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


MEMFS_EXTERN void memfs_setsize(const char* zName, __int64 nSize)
{
    memfs_file_data* pData = find_file_data(get_vfs_object(), zName);
    if (pData != NULL && pData->iDeleted == 0)
        pData->nSize = nSize;
}


MEMFS_EXTERN int memfs_readdata(const char* zName, void* data, int nSize, __int64 iOfst)
{
    int result, flags;
    file_object* pFile = NULL;
    flags = 0; result = 0;

    pFile = (file_object*)malloc(sizeof(file_object));
    if (pFile == NULL) return 0;

    result = get_vfs_object()->xOpen(get_vfs_object(), zName, (sqlite3_file*)pFile, SQLITE_OPEN_READONLY, &flags);
    if (result != SQLITE_OK) { free(pFile); return 0; }

    pFile->base.pMethods->xRead((sqlite3_file*)pFile, data, nSize, iOfst);

    result = (int)(pFile->pData->nSize - iOfst);
    if (result > nSize) result = nSize;
    if (result < 0) result = 0;

    pFile->base.pMethods->xClose((sqlite3_file*)pFile);
    free(pFile);

    return result;
}


MEMFS_EXTERN int memfs_writedata(const char* zName, void* data, int nSize, __int64 iOfst)
{
    int result, flags;
    file_object* pFile = NULL;
    flags = 0; result = 0;

    pFile = (file_object*)malloc(sizeof(file_object));
    if (pFile == NULL) return result;

    get_vfs_object()->xOpen(get_vfs_object(), zName, (sqlite3_file*)pFile, SQLITE_OPEN_CREATE, &flags);
    if (result != SQLITE_OK) { free(pFile); return result; }

    pFile->base.pMethods->xWrite((sqlite3_file*)pFile, data, nSize, iOfst);
    if (result == SQLITE_OK) { result = nSize; }

    pFile->base.pMethods->xClose((sqlite3_file*)pFile);
    free(pFile);

    return result;
}


MEMFS_EXTERN int memfs_delfile(const char* zName)
{
    sqlite3_vfs* pVfs = get_vfs_object();
    return pVfs->xDelete(pVfs, zName, 0);
}
