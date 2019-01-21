#pragma unmanaged

#include <stdlib.h>

#include "sqlite3.h"

#include "critical_section.h"



void* create_critical_section()
{
    return sqlite3_mutex_alloc(SQLITE_MUTEX_RECURSIVE);
}


void enter_critical_section(void* pSyncRoot)
{
    sqlite3_mutex_enter((sqlite3_mutex*)pSyncRoot);
}


void leave_critical_section(void* pSyncRoot)
{
    sqlite3_mutex_leave((sqlite3_mutex*)pSyncRoot);
}


void destroy_critical_section(void* pSyncRoot)
{
    sqlite3_mutex_free((sqlite3_mutex*)pSyncRoot);
}
