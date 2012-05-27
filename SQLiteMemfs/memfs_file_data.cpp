#pragma unmanaged

#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

#include "memfs_file_data.h"



void clear_file(memfs_file_data* pData);


int create_file_data(memfs_file_data* pData, const char* zName)
{
    int nName;
    memset(pData, 0, sizeof(memfs_file_data));

    nName = strlen(zName) + 1;
    pData->zName = (char*)malloc(nName);
    if (pData->zName == NULL) { clear_file(pData); return SQLITE_NOMEM; }
    strcpy_s(pData->zName, nName, zName);

    pData->nLenght = 8192;
    pData->pBuffer = malloc((size_t)(pData->nLenght));
    if (pData->pBuffer == NULL) { clear_file(pData); return SQLITE_NOMEM; }

    return SQLITE_OK;
}


memfs_file_data* find_file_data(sqlite3_vfs* pVfs, const char* zName)
{
    memfs_file_data* pData = (memfs_file_data*)(pVfs->pAppData);
    while (pData != NULL)
    {
        if (strcmp(pData->zName, zName) == 0)
            return pData;
        pData = pData->pNext;
    }
    return NULL;
}


int delete_file_data(memfs_file_data* pData)
{
    pData->iDeleted = 1;

    pData->nSize = 0;
    pData->nLenght = 0;
    if (pData->pBuffer != NULL)
        free(pData->pBuffer);
    pData->pBuffer = NULL;

    return SQLITE_OK;
}


int destroy_file(memfs_file_data* pData)
{
    clear_file(pData);
    free(pData);
    return SQLITE_OK;
}


void clear_file(memfs_file_data* pData)
{
    if (pData->pBuffer != NULL)
        free(pData->pBuffer);
    pData->pBuffer = NULL;

    if (pData->zName != NULL)
        free(pData->zName);
    pData->zName = NULL;
}
