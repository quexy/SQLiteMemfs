#pragma once

#include "sqlite3.h"

#include "file_list_item.h"


#define CHUNK_BASE 8192

#define MAX_SHIFT 8


typedef struct data_chunk data_chunk;
struct data_chunk
{
    int nSize;
    void* pBuffer;
    data_chunk* pNext;
};

typedef struct memfs_file_data memfs_file_data;
struct memfs_file_data
{
    char* zName;
    int nRef;
    file_list_item* pRefs;
    int iDeleted;
    sqlite3_int64 nSize;
    data_chunk* pChunks;
};


int init_file_data(memfs_file_data* pData, const char* zName);

memfs_file_data* find_file_data(sqlite3_vfs* pVfs, const char* zName);

int destroy_file(memfs_file_data* pData);
