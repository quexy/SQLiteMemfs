#pragma unmanaged

#include <stdlib.h>
#include <assert.h>

#include "sqlite3.h"

#include "file_list_item.h"
#include "critical_section.h"



int add_link(file_list_item* pHead, void* pObject)
{
    if (pObject == NULL) return SQLITE_OK;

    file_list_item* pLink = (file_list_item*)malloc(sizeof(file_list_item));
    if (pLink == NULL) return SQLITE_NOMEM;

    pLink->pObject = pObject;

    enter_critical_section(pHead->pObject);
    pLink->pPrev = pHead->pPrev;
    pLink->pNext = pHead;
    pHead->pPrev->pNext = pLink;
    pHead->pPrev = pLink;
    leave_critical_section(pHead->pObject);

    return SQLITE_OK;
}


int del_link(file_list_item* pHead, void* pObject)
{
    if (pObject == NULL) return SQLITE_OK;
    for (file_list_item* ptr = pHead->pNext; ptr != pHead; ptr = ptr->pNext)
    {
        assert(ptr->pObject != NULL);
        if (ptr->pObject != pObject) continue;

        enter_critical_section(pHead->pObject);
        ptr->pPrev->pNext = ptr->pNext;
        ptr->pNext->pPrev = ptr->pPrev;
        leave_critical_section(pHead->pObject);

        void* temp = ptr;
        ptr = ptr->pPrev;
        free(temp);
    }
    return SQLITE_OK;
}


file_list_item* create_list()
{
    file_list_item* pHead = (file_list_item*)malloc(sizeof(file_list_item));
    if (pHead == NULL) return NULL;

    pHead->pObject = create_critical_section();
    if (pHead->pObject == NULL) { free(pHead); return NULL; }

    pHead->pPrev = pHead;
    pHead->pNext = pHead;
    return pHead;
}


int destroy_list(file_list_item* pHead)
{
    void* pSyncRoot = pHead->pObject;
    enter_critical_section(pSyncRoot);
    
    while (pHead->pNext != pHead)
    {
        file_list_item* ptr = pHead->pNext;
        pHead->pNext = ptr->pNext;
        free(ptr);
    }
    pHead->pObject = NULL;
    free(pHead);

    leave_critical_section(pSyncRoot);
    destroy_critical_section(pSyncRoot);
    return SQLITE_OK;
}
