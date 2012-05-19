#include "sqlite3.h"

#include "file_object.h"
#include "memdb_vfs.h"
#include "vfs_object.h"



static sqlite3_vfs vfs_object = {
    1,
    sizeof(file_object),
    255,
    0,
    "memdb",
    0,
    memdb_vfs_Open,
    memdb_vfs_Delete,
    memdb_vfs_Access,
    memdb_vfs_FullPathname,
    memdb_vfs_DlOpen,
    memdb_vfs_DlError,
    memdb_vfs_DlSym,
    memdb_vfs_DlClose,
    memdb_vfs_Randomness,
    memdb_vfs_Sleep,
    memdb_vfs_CurrentTime,
};

sqlite3_vfs* get_vfs_object(){
    return &vfs_object;
}
