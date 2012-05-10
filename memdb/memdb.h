#pragma once

#define MEMDB_EXTERN extern __declspec(dllexport)


MEMDB_EXTERN int memdb_init();
MEMDB_EXTERN int memdb_destroy();

MEMDB_EXTERN int memdb_getsize(const char* zName);
MEMDB_EXTERN int memdb_getdata(const char* zName, void* data, int nSize);
MEMDB_EXTERN int memdb_setdata(const char* zName, void* data, int nSize);
