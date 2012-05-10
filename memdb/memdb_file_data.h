#pragma once

#include "sqlite3.h"



typedef struct memdb_file_data memdb_file_data;

struct memdb_file_data
{
    char* zName;
    memdb_file_data* pNext;
    int nRef;
    int iDeleted;
    sqlite3_int64 nSize;
    sqlite3_int64 nLenght;
    void* pBuffer;
};


 int create_file_data(memdb_file_data* pData, const char* zName);

memdb_file_data* find_file_data(sqlite3_vfs* pVfs, const char* zName);

int delete_file_data(memdb_file_data* pData);

int destroy_file(memdb_file_data* pData);
