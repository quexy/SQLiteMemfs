#include <stdlib.h>
#include <string.h>

#include "file_object.h"
#include "memdb_file_io.h"


static sqlite3_io_methods memdb_io_methods = {
    1,
    memdb_io_Close,
    memdb_io_Read,
    memdb_io_Write,
    memdb_io_Truncate,
    memdb_io_Sync,
    memdb_io_FileSize,
    memdb_io_Lock,
    memdb_io_Unlock,
    memdb_io_CheckReservedLock,
    memdb_io_FileControl,
    memdb_io_SectorSize,
    memdb_io_DeviceCharacteristics
};

sqlite3_io_methods* get_io_methods()
{
    return &memdb_io_methods;
}
