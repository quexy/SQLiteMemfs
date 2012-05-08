#pragma once

EXTERNAL int memdb_init();
EXTERNAL int memdb_destroy();
EXTERNAL int memdb_getdatasize(char* file, int* size);
EXTERNAL int memdb_getdata(char* file, void** data, int* size);
