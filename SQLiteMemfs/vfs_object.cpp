#pragma unmanaged

#include <stdlib.h>

#include "sqlite3.h"

#include "file_object.h"
#include "memfs_vfs.h"
#include "vfs_object.h"



static sqlite3_vfs vfs_object = {
    1,
    sizeof(file_object),
    255,
    NULL,
    "memfs",
    NULL,
    memfs_vfs_Open,
    memfs_vfs_Delete,
    memfs_vfs_Access,
    memfs_vfs_FullPathname,
    memfs_vfs_DlOpen,
    memfs_vfs_DlError,
    memfs_vfs_DlSym,
    memfs_vfs_DlClose,
    memfs_vfs_Randomness,
    memfs_vfs_Sleep,
    memfs_vfs_CurrentTime,
};

sqlite3_vfs* get_vfs_object()
{
    if (vfs_object.pAppData == NULL)
        vfs_object.pAppData = create_list();
    return &vfs_object;
}
