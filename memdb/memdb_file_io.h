#pragma once

#include "sqlite3.h"


int memdb_io_Close(sqlite3_file*);
int memdb_io_Read(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
int memdb_io_Write(sqlite3_file*, const void*, int iAmt, sqlite3_int64 iOfst);
int memdb_io_Truncate(sqlite3_file*, sqlite3_int64 size);
int memdb_io_Sync(sqlite3_file*, int flags);
int memdb_io_FileSize(sqlite3_file*, sqlite3_int64 *pSize);

int memdb_io_Lock(sqlite3_file*, int);
int memdb_io_Unlock(sqlite3_file*, int);
int memdb_io_CheckReservedLock(sqlite3_file*, int *pResOut);
int memdb_io_FileControl(sqlite3_file*, int op, void *pArg);
int memdb_io_SectorSize(sqlite3_file*);
int memdb_io_DeviceCharacteristics(sqlite3_file*);
