#pragma once

#include "sqlite3.h"

#include "memdb_file_data.h"



typedef struct file_object file_object;

struct file_object
{
    sqlite3_file base;
    memdb_file_data* pData;
};


sqlite3_io_methods* get_io_methods();
