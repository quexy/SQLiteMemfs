#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

#include "memdb.h"
#include "vfs_object.h"
#include "file_object.h"


static int refCount = 0;


MEMDB_EXTERN int memdb_init()
{
    ++refCount;
    if (refCount == 1)
        sqlite3_vfs_register(get_vfs_object(), 1);
    return refCount - 1;
}


MEMDB_EXTERN int memdb_destroy()
{
    memdb_file_data* pData;

    --refCount;
    if (refCount == 0)
    {
        sqlite3_vfs* pVfs = get_vfs_object();
        sqlite3_vfs_unregister(pVfs);

        while(pVfs->pAppData != NULL)
        {
            pData = (memdb_file_data*)(pVfs->pAppData);
            pVfs->pAppData = pData->pNext;

            pData->pNext = NULL;
            if (pData->nRef == 0) destroy_file(pData);
            else delete_file_data(pData);
        }
    }
    return refCount;
}


MEMDB_EXTERN int memdb_getsize(const char* zName)
{
    memdb_file_data* pData = find_file_data(get_vfs_object(), zName);
    if (pData == NULL || pData->iDeleted == 1)
        return 0;
    else // found and not deleted
        return (int)pData->nSize;
}


MEMDB_EXTERN int memdb_getdata(const char* zName, void* data, int nSize)
{
    memdb_file_data* pData = find_file_data(get_vfs_object(), zName);
    if (pData == NULL || pData->iDeleted == 1) return 0;

    memcpy_s(data, nSize, pData->pBuffer, (rsize_t)(pData->nSize));
    return (int)((nSize < pData->nSize) ? pData->nSize : nSize);
}


MEMDB_EXTERN int memdb_setdata(const char* zName, void* data, int nSize)
{
    int result, flags;

    file_object* pFile = (file_object*)malloc(sizeof(file_object));
    if (pFile == NULL) return SQLITE_NOMEM;

    result = get_vfs_object()->xOpen(get_vfs_object(), zName, (sqlite3_file*)pFile, 0, &flags); 
    if (result != SQLITE_OK) return result;

    result = pFile->base.pMethods->xWrite((sqlite3_file*)pFile, data, nSize, 0);
    if (result != SQLITE_OK) return result;

    result = pFile->base.pMethods->xClose((sqlite3_file*)pFile);
    if (result != SQLITE_OK) return result;

    free(pFile); return SQLITE_OK;     
}
