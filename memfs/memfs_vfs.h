#pragma once

#include "sqlite3.h"


/* own function implementations */
int memfs_vfs_Open(sqlite3_vfs* pVfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags);
int memfs_vfs_Delete(sqlite3_vfs* pVfs, const char* zName, int syncDir);
int memfs_vfs_Access(sqlite3_vfs* pVfs, const char* zName, int flags, int* pResOut);
int memfs_vfs_FullPathname(sqlite3_vfs* pVfs, const char* zName, int nOut, char* zOut);

/* system API calls: pVfs->pNext delegation */
int memfs_vfs_Randomness(sqlite3_vfs* pVfs, int nByte, char *zOut);
int memfs_vfs_Sleep(sqlite3_vfs* pVfs, int microsec);
int memfs_vfs_CurrentTime(sqlite3_vfs* pVfs, double* dTime);
int memfs_vfs_GetLastError(sqlite3_vfs* pVfs, int, char* zErr);
int memfs_vfs_CurrentTimeInt64(sqlite3_vfs* pVfs, sqlite3_int64* hTime);

/* loadable modules: pVfs->pNext delegation */
void* memfs_vfs_DlOpen(sqlite3_vfs* pVfs, const char* zPath);
void memfs_vfs_DlError(sqlite3_vfs* pVfs, int nByte, char* zErrMsg);
void (*memfs_vfs_DlSym(sqlite3_vfs* pVfs, void* pH, const char* z))(void);
void memfs_vfs_DlClose(sqlite3_vfs* pVfs, void* pHandle);
