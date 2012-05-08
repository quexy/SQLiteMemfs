#include "StdAfx.h"
#include <stdlib.h>

#include "memdb.h"

#include "sqlite3.h"


static sqlite3_vfs* vfsObject = NULL;
static int refCount = 0;


EXTERNAL int memdb_init()
{
    ++refCount;
    if (refCount == 0)
    {
        //vfsObject = create_vfs_object();
        //sqlite3_vfs_register(vfsObject, true);
    }
    return refCount - 1;
}


EXTERNAL int memdb_getdatasize(char* file, int* size)
{
    *size = 2;
    return 0;
}


EXTERNAL int memdb_getdata(char* file, void** data, int* size)
{
    *size = 2;
    (char)(((char*)(*data))[0]) = 1;
    (char)(((char*)(*data))[1]) = 2;
    return (*size);
}


EXTERNAL int memdb_destroy()
{
    --refCount;
    if (refCount == 0)
    {
        //sqlite3_vfs_unregister(vfsObject);
        vfsObject = NULL;
    }
    return refCount;
}
