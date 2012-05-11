#include <stdlib.h>
#include <string.h>

#include "file_object.h"



int memdb_io_Close(sqlite3_file* pFile)
{
    ((file_object*)pFile)->pData->nRef -= 1;
    return SQLITE_OK;
}

int memdb_io_Read(sqlite3_file* pFile, void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    memdb_file_data* pData = ((file_object*)pFile)->pData;
    if (pData->iDeleted == 1) return SQLITE_IOERR;

    memcpy_s(pBuf, iAmt, ((char*)(pData->pBuffer)) + iOfst, (rsize_t)(pData->nSize - iOfst));
    return SQLITE_OK;
}

int memdb_io_Write(sqlite3_file* pFile, const void* pBuf, int iAmt, sqlite3_int64 iOfst)
{
    sqlite3_int64 newSize, newLength;
    memdb_file_data* pData = ((file_object*)pFile)->pData;
    if (pData->iDeleted == 1) return SQLITE_IOERR;

    newSize = iOfst + iAmt;
    newLength = (sqlite3_int64)(newSize * 1.1);
    if (pData->nLenght < newSize)
    {
        void* newBuffer = (void*)sqlite3_malloc((size_t)newLength);
        if (newBuffer == NULL) return SQLITE_NOMEM;
        memcpy_s(newBuffer, (rsize_t)newLength, pData->pBuffer, (rsize_t)pData->nSize);
        sqlite3_free(pData->pBuffer);
        pData->pBuffer = newBuffer;
        pData->nLenght = newLength;
    }
    
    memcpy_s(((char*)(pData->pBuffer)) + iOfst, (rsize_t)(pData->nLenght - iOfst), pBuf, iAmt);
    pData->nSize = (pData->nSize < newSize) ? newSize : pData->nSize;

    return SQLITE_OK;
}

int memdb_io_Truncate(sqlite3_file* pFile, sqlite3_int64 size)
{
    ((file_object*)pFile)->pData->nSize = size;
    return SQLITE_OK;
}

int memdb_io_Sync(sqlite3_file* pFile, int flags)
{
    return SQLITE_OK;
}

int memdb_io_FileSize(sqlite3_file* pFile, sqlite3_int64 *pSize)
{
    *pSize = ((file_object*)pFile)->pData->nSize;
    return SQLITE_OK;
}

int memdb_io_Lock(sqlite3_file* pFile, int nLock)
{
    return SQLITE_OK;
}

int memdb_io_Unlock(sqlite3_file* pFile, int nLock)
{
    return SQLITE_OK;
}

int memdb_io_CheckReservedLock(sqlite3_file* pFile, int *pResOut)
{
    *pResOut = 0;
    return SQLITE_OK;
}

int memdb_io_FileControl(sqlite3_file* pFile, int op, void *pArg)
{
    return SQLITE_OK;
}

int memdb_io_SectorSize(sqlite3_file* pFile)
{
    return 0;
}

int memdb_io_DeviceCharacteristics(sqlite3_file* pFile)
{
    return 0;
}
