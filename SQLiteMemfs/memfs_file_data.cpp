#pragma unmanaged

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sqlite3.h"

#include "memfs_file_data.h"
#include "file_list_item.h"

void clear_file(memfs_file_data* pData);



int init_file_data(memfs_file_data* pData, const char* zName)
{
    size_t nName;
    memset(pData, 0, sizeof(memfs_file_data));

    nName = strlen(zName) + 1;
    pData->zName = (char*)malloc(nName);
    if (pData->zName == NULL) { clear_file(pData); return SQLITE_NOMEM; }
    strcpy_s(pData->zName, nName, zName);

    pData->pChunks = (data_chunk*)malloc(sizeof(data_chunk));
    if (pData->pChunks == NULL) { clear_file(pData); return SQLITE_NOMEM; }
    pData->pChunks->nSize = CHUNK_BASE; pData->pChunks->pNext = NULL;
    pData->pChunks->pBuffer = malloc((pData->pChunks->nSize));
    if (pData->pChunks->pBuffer == NULL) { clear_file(pData); return SQLITE_NOMEM; }

    pData->pRefs = create_list(); // initialize file list
    if (pData->pRefs == NULL) { clear_file(pData); return SQLITE_NOMEM; }

    return SQLITE_OK;
}


memfs_file_data* find_file_data(sqlite3_vfs* pVfs, const char* zName)
{
    file_list_item* pHead = (file_list_item*)(pVfs->pAppData);
    for (file_list_item* ptr = pHead->pNext; ptr != pHead; ptr = ptr->pNext)
    {
        memfs_file_data* pData = (memfs_file_data*)ptr->pObject;
        if (strcmp(pData->zName, zName) == 0) return pData;
    }
    return NULL;
}


int destroy_file(memfs_file_data* pData)
{
    if (pData != NULL)
    {
        clear_file(pData);
        free(pData);
    }

    return SQLITE_OK;
}


void clear_file(memfs_file_data* pData)
{
    if (pData == NULL) return;

    while (pData->pChunks != NULL)
    {
        data_chunk* pChunk = pData->pChunks;
        pData->pChunks = pData->pChunks->pNext;
        free(pChunk->pBuffer);
        free(pChunk);
    }

    if (pData->zName != NULL)
        free(pData->zName);
    pData->zName = NULL;

    if (pData->pRefs != NULL)
    {
        assert(pData->pRefs->pNext == pData->pRefs->pPrev);
        destroy_list(pData->pRefs);
        pData->pRefs = NULL;
    }
}
