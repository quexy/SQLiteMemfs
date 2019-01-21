#pragma once


void* create_critical_section();
void enter_critical_section(void* pSyncRoot);
void leave_critical_section(void* pSyncRoot);
void destroy_critical_section(void* pSyncRoot);
