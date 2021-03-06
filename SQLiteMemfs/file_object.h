#pragma once

#include "sqlite3.h"

#include "memfs_file_data.h"


typedef struct file_object file_object;
struct file_object
{
    sqlite3_file base;
    memfs_file_data* pData;
    int nLock;
};

sqlite3_io_methods* get_io_methods();
