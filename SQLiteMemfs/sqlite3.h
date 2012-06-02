#pragma once

#define SQLITE_ENABLE_COLUMN_METADATA 1
#define SQLITE_ENABLE_RTREE 1
#define SQLITE_EXTERN
#define SQLITE_API extern "C" __declspec(dllimport)

#include "..\dependencies\sqlite3.h"
