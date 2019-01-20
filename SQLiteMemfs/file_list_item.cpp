#pragma unmanaged

#include <stdlib.h>
#include <assert.h>

#include "sqlite3.h"

#include "file_list_item.h"



int add_link(file_list_item* pHead, void* pObject)
{
    if (pObject == NULL) return SQLITE_OK;

    file_list_item* pLink = (file_list_item*)malloc(sizeof(file_list_item));
    if (pLink == NULL) return SQLITE_NOMEM;

    pLink->pObject = pObject;

    pLink->pPrev = pHead->pPrev;
    pLink->pNext = pHead;
    pHead->pPrev->pNext = pLink;
    pHead->pPrev = pLink;

    return SQLITE_OK;
}


int del_link(file_list_item* pHead, void* pObject)
{
    if (pObject == NULL) return SQLITE_OK;
    for (file_list_item* ptr = pHead->pNext; ptr != pHead; ptr = ptr->pNext)
    {
        assert(ptr->pObject != NULL);
        if (ptr->pObject != pObject) continue;
        ptr->pPrev->pNext = ptr->pNext;
        ptr->pNext->pPrev = ptr->pPrev;

        void* temp = ptr;
        ptr = ptr->pPrev;
        free(temp);
    }
    return SQLITE_OK;
}


int destroy_list(file_list_item* pHead)
{
    file_list_item* ptr = NULL;
    while (pHead->pNext != pHead->pPrev)
    {
        assert(false);
        ptr = pHead->pNext;
        pHead->pNext = ptr->pNext;
        free(ptr);
    }
    free(pHead);

    return SQLITE_OK;
}
