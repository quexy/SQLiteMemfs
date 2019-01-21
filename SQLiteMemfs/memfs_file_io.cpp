#pragma unmanaged

#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

#include "file_object.h"
#include "file_list_item.h"

int max_lock(int nLock);



int memfs_io_Close(sqlite3_file* pFile)
{
    ((file_object*)pFile)->pData->nRef -= 1;
    del_link(((file_object*)pFile)->pData->pRefs, pFile);
    return SQLITE_OK;
}


int memfs_io_Read(sqlite3_file* pFile, void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    int nAmount = 0;
    memfs_file_data* pData = ((file_object*)pFile)->pData;
    if (pData->iDeleted == 1) return SQLITE_IOERR;
    if (iAmt <= 0) return SQLITE_OK;

    nAmount = (int)(pData->nSize - iOfst);
    if (nAmount <= 0) return SQLITE_IOERR_SHORT_READ;

    memcpy_s(pBuf, iAmt, ((char*)(pData->pBuffer)) + iOfst, ((iAmt < nAmount) ? iAmt : nAmount));
    return (iAmt <= nAmount) ? SQLITE_OK : SQLITE_IOERR_SHORT_READ;
}


int memfs_io_Write(sqlite3_file* pFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    sqlite3_int64 newSize, newLength;
    memfs_file_data* pData = ((file_object*)pFile)->pData;
    if (pData->iDeleted == 1) return SQLITE_IOERR;

    newSize = iOfst + iAmt;
    newLength = (sqlite3_int64)(newSize * 1.7);
    if (pData->nLength < newSize)
    {
        void* newBuffer = (void*)malloc((size_t)newLength);
        if (newBuffer == NULL) return SQLITE_NOMEM;
        if (pData->pBuffer != NULL)
        {
            memcpy_s(newBuffer, (rsize_t)newLength, pData->pBuffer, (rsize_t)pData->nSize);
            free(pData->pBuffer);
        }
        pData->pBuffer = newBuffer;
        pData->nLength = newLength;
    }

    memcpy_s(((char*)(pData->pBuffer)) + iOfst, (rsize_t)(pData->nLength - iOfst), pBuf, iAmt);
    pData->nSize = (pData->nSize < newSize) ? newSize : pData->nSize;

    return SQLITE_OK;
}


int memfs_io_Truncate(sqlite3_file* pFile, sqlite3_int64 size)
{
    memfs_file_data* pData = ((file_object*)pFile)->pData;
    if (pData->nSize > size) return SQLITE_INTERNAL;
    ((file_object*)pFile)->pData->nSize = size;
    return SQLITE_OK;
}


int memfs_io_Sync(sqlite3_file* pFile, int flags)
{
    return SQLITE_OK;
}


int memfs_io_FileSize(sqlite3_file* pFile, sqlite3_int64 *pSize)
{
    *pSize = ((file_object*)pFile)->pData->nSize;
    return SQLITE_OK;
}


int memfs_io_Lock(sqlite3_file* pFile, int nLock)
{
    file_object* pObject;
    file_list_item* pHead;
    int maxLock = max_lock(nLock);

    pObject = (file_object*)pFile;
    // same or higher level lock already held
    if (((file_object*)pFile)->nLock >= nLock) return SQLITE_OK;

    pHead = pObject->pData->pRefs;
    for (file_list_item* ptr = pHead->pNext; ptr != pHead; ptr = ptr->pNext)
    {
        file_object* pObj = (file_object*)ptr->pObject;
        if (pObj != pObject && pObj->nLock > maxLock)
            return SQLITE_BUSY;
    }

    // no conflict; execute
    pObject->nLock = nLock;

    return SQLITE_OK;
}


int memfs_io_Unlock(sqlite3_file* pFile, int nLock)
{
    file_object* pObject = (file_object*)pFile;
    if (pObject->nLock < nLock) return SQLITE_ERROR;
    pObject->nLock = nLock;
    return SQLITE_OK;
}


int memfs_io_CheckReservedLock(sqlite3_file* pFile, int *pResOut)
{
    *pResOut = 0;
    file_list_item* pHead = ((file_object*)pFile)->pData->pRefs;
    for (file_list_item* ptr = pHead->pNext; ptr != pHead; ptr = ptr->pNext)
    {
        file_object* pObj = (file_object*)ptr->pObject;
        if (pObj->nLock >= SQLITE_LOCK_RESERVED) *pResOut += 1;
    }
    return SQLITE_OK;
}


int memfs_io_FileControl(sqlite3_file* pFile, int op, void *pArg)
{
    return SQLITE_OK;
}


int memfs_io_SectorSize(sqlite3_file* pFile)
{
    return 0;
}


int memfs_io_DeviceCharacteristics(sqlite3_file* pFile)
{
    return 0;
}


int max_lock(int nLock)
{
    switch (nLock)
    {
        case SQLITE_LOCK_EXCLUSIVE:
            return SQLITE_LOCK_NONE;
        case SQLITE_LOCK_PENDING:
            return SQLITE_LOCK_RESERVED;
        case SQLITE_LOCK_RESERVED:
            return SQLITE_LOCK_SHARED;
        case SQLITE_LOCK_SHARED:
            return SQLITE_LOCK_SHARED;
        case SQLITE_LOCK_NONE:
            return SQLITE_LOCK_EXCLUSIVE;
        default: return SQLITE_LOCK_NONE;
    }
}
