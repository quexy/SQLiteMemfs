#pragma once

#include "sqlite3.h"



typedef struct memfs_file_data memfs_file_data;
struct memfs_file_data
{
    char* zName;
    memfs_file_data* pNext;
    int nRef;
    int iDeleted;
    sqlite3_int64 nSize;
    sqlite3_int64 nLenght;
    void* pBuffer;
};


int create_file_data(memfs_file_data* pData, const char* zName);

memfs_file_data* find_file_data(sqlite3_vfs* pVfs, const char* zName);

int delete_file_data(memfs_file_data* pData);

int destroy_file(memfs_file_data* pData);
