#pragma once

#define MEMDB_EXTERN extern __declspec(dllexport)


MEMDB_EXTERN int memdb_init();
MEMDB_EXTERN int memdb_destroy();

MEMDB_EXTERN __int64 memdb_getsize(const char* zName);
MEMDB_EXTERN void memdb_setsize(const char* zName, __int64 nSize);

MEMDB_EXTERN int memdb_readdata(const char* zName, void* data, int nSize, __int64 iOfst);
MEMDB_EXTERN int memdb_writedata(const char* zName, void* data, int nSize, __int64 iOfst);
