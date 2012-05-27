#pragma unmanaged

#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

#include "file_object.h"
#include "memfs_file_io.h"



static sqlite3_io_methods memfs_io_methods = {
    1,
    memfs_io_Close,
    memfs_io_Read,
    memfs_io_Write,
    memfs_io_Truncate,
    memfs_io_Sync,
    memfs_io_FileSize,
    memfs_io_Lock,
    memfs_io_Unlock,
    memfs_io_CheckReservedLock,
    memfs_io_FileControl,
    memfs_io_SectorSize,
    memfs_io_DeviceCharacteristics
};

sqlite3_io_methods* get_io_methods()
{
    return &memfs_io_methods;
}
