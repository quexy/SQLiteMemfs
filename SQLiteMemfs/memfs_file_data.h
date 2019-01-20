#pragma once

#include "sqlite3.h"

#include "file_list_item.h"


typedef struct memfs_file_data memfs_file_data;
struct memfs_file_data
{
    char* zName;
    int nRef;
    file_list_item* pRefs;
    int iDeleted;
    sqlite3_int64 nSize;
    sqlite3_int64 nLength;
    void* pBuffer;
};


int create_file_data(memfs_file_data* pData, const char* zName);

memfs_file_data* find_file_data(sqlite3_vfs* pVfs, const char* zName);

int delete_file_data(memfs_file_data* pData);

int destroy_file(memfs_file_data* pData);
