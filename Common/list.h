#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <WinSock2.h>

#define DEFAULT_LIST_LIMIT 1000

typedef struct ListItem_st
{
	SOCKET data;
	struct ListItem_st* next;
} ListItem;

typedef struct List_st
{
	ListItem* head;
	int len;
	int limit;
} List;

// initialize new list
// limit - max ammount of elements in list
// returns pointer to new list
List* list_init(int limit);

// add item to end of list if under the size limit
// returns true if successful
// returns false if error
bool list_addAtEnd(List* list, SOCKET data);

SOCKET* list_getAt(List* list, int index);

// deletes item from list at specified index
// returns true if successful
// returns false if error
bool list_deleteAt(List* list, int index);

// free list and all elements from memory
// returns true if successful
// returns false if error
bool list_dump(List* list);