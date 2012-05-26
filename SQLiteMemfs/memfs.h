#pragma once

#define MEMFS_EXTERN
// #define MEMFS_EXTERN extern "C" __declspec(dllexport)


MEMFS_EXTERN int memfs_init();
MEMFS_EXTERN int memfs_destroy();

MEMFS_EXTERN __int64 memfs_getsize(const char* zName);
MEMFS_EXTERN void memfs_setsize(const char* zName, __int64 nSize);

MEMFS_EXTERN int memfs_readdata(const char* zName, void* data, int nSize, __int64 iOfst);
MEMFS_EXTERN int memfs_writedata(const char* zName, void* data, int nSize, __int64 iOfst);

MEMFS_EXTERN int memfs_delfile(const char* zName);
