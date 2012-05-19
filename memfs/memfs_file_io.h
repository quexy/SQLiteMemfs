#pragma once

#include "sqlite3.h"


int memfs_io_Close(sqlite3_file*);
int memfs_io_Read(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
int memfs_io_Write(sqlite3_file*, const void*, int iAmt, sqlite3_int64 iOfst);
int memfs_io_Truncate(sqlite3_file*, sqlite3_int64 size);
int memfs_io_Sync(sqlite3_file*, int flags);
int memfs_io_FileSize(sqlite3_file*, sqlite3_int64 *pSize);

int memfs_io_Lock(sqlite3_file*, int);
int memfs_io_Unlock(sqlite3_file*, int);
int memfs_io_CheckReservedLock(sqlite3_file*, int *pResOut);
int memfs_io_FileControl(sqlite3_file*, int op, void *pArg);
int memfs_io_SectorSize(sqlite3_file*);
int memfs_io_DeviceCharacteristics(sqlite3_file*);
