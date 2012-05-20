#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

#include "file_object.h"
#include "memfs.h"
#include "vfs_object.h"



static int refCount = 0;


MEMFS_EXTERN int memfs_init()
{
    ++refCount;
    if (refCount == 1)
        sqlite3_vfs_register(get_vfs_object(), 1);
    return refCount - 1;
}


MEMFS_EXTERN int memfs_destroy()
{
    memfs_file_data* pData;

    --refCount;
    if (refCount == 0)
    {
        sqlite3_vfs* pVfs = get_vfs_object();
        sqlite3_vfs_unregister(pVfs);

        while(pVfs->pAppData != NULL)
        {
            pData = (memfs_file_data*)(pVfs->pAppData);
            pVfs->pAppData = pData->pNext;

            pData->pNext = NULL;
            if (pData->nRef == 0) destroy_file(pData);
            else delete_file_data(pData);
        }
    }
    return refCount;
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