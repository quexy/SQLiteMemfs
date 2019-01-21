#pragma unmanaged

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sqlite3.h"

#include "file_object.h"
#include "file_list_item.h"

int max_lock(int nLock);



int memfs_io_Close(sqlite3_file* pFile)
{
    memfs_file_data* pData = ((file_object*)pFile)->pData;
    pData->nRef -= 1; del_link(pData->pRefs, pFile);
    return SQLITE_OK;
}


int memfs_io_Read(sqlite3_file* pFile, void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    int nDone;
    sqlite3_uint64 nOffset;
    memfs_file_data* pData = ((file_object*)pFile)->pData;
    if (pData->iDeleted == 1) return SQLITE_IOERR;
    if (iAmt <= 0) return SQLITE_OK;

    memset(pBuf, 0, iAmt);
    if (pData->nSize - iOfst <= 0) return SQLITE_IOERR_SHORT_READ;

    nOffset = iOfst;
    if (nOffset < 0)nOffset = 0;

    nDone = 0;
    for (data_chunk* ptr = pData->pChunks; ptr != NULL && nDone < iAmt; ptr = ptr->pNext)
    {
        if (nOffset >= ptr->nSize) nOffset -= ptr->nSize;
        else /* offset within this chunk */
        {
            int nSource = iAmt - nDone;
            int nTarget = ptr->nSize - (int)nOffset;
            int nAmount = (nSource < nTarget) ? nSource : nTarget;
            memcpy_s((char*)pBuf + nDone, nAmount, ((char*)ptr->pBuffer) + nOffset, nAmount);
            nDone += nAmount; nOffset = 0;
        }
    }
    assert(iAmt >= nDone);
    return (iAmt == nDone) ? SQLITE_OK : SQLITE_IOERR_SHORT_READ;
}


int memfs_io_Write(sqlite3_file* pFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    int nDone, nShift;
    sqlite3_int64 nOffset;
    memfs_file_data* pData = ((file_object*)pFile)->pData;
    if (pData->iDeleted == 1) return SQLITE_IOERR;
    if (iAmt <= 0) return SQLITE_OK;

    nOffset = iOfst;
    if (nOffset < 0) nOffset = 0;
    if (nOffset > LLONG_MAX - iAmt) return SQLITE_IOERR;

    nDone = 0; nShift = 0;
    for (data_chunk* ptr = pData->pChunks; ptr != NULL && nDone < iAmt; ptr = ptr->pNext)
    {
        if (nShift < MAX_SHIFT) nShift += 1;
        if (nOffset >= ptr->nSize) nOffset -= ptr->nSize;
        else /* offset within this chunk */
        {
            int nSource = iAmt - nDone;
            int nTarget = ptr->nSize - (int)nOffset;
            int nAmount = (nSource < nTarget) ? nSource : nTarget;

            if (pBuf == NULL) memset(ptr->pBuffer, 0, nTarget);
            else memcpy_s((char*)ptr->pBuffer + nOffset, nAmount, (char*)pBuf + nDone, nAmount);
            nDone += nAmount; nOffset = 0;
        }

        if (nDone < iAmt && ptr->pNext == NULL)
        {
            data_chunk* pNext = (data_chunk*)malloc(sizeof(data_chunk));
            if (pNext == NULL) { return SQLITE_IOERR_NOMEM; }

            pNext->pNext = NULL;
            pNext->nSize = CHUNK_BASE << nShift;
            pNext->pBuffer = malloc(pNext->nSize);
            if (pNext->pBuffer == NULL) { free(pNext); return SQLITE_IOERR_NOMEM; }
            if (pBuf == NULL) memset(pNext->pBuffer, 0, pNext->nSize);
            ptr->pNext = pNext;
        }
    }

    if (pData->nSize < iOfst + iAmt)
        pData->nSize = iOfst + iAmt;

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
    // no opcodes recognised
    return SQLITE_NOTFOUND;
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
