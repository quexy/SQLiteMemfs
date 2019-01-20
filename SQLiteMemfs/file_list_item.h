#pragma once


typedef struct file_list_item file_list_item;
struct file_list_item
{
    file_list_item* pPrev;
    void* pObject;
    file_list_item* pNext;
};


int add_link(file_list_item* pHead, void* pObject);

int del_link(file_list_item* pHead, void* pObject);

int destroy_list(file_list_item* pHead);
