#pragma once

typedef struct ListItem_st
{
	void* data;
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
List* initList(int limit);

// add item to end of list if under the size limit
// returns 1 if successful
// returns 0 if error
int addAtEnd(List* list, ListItem* item);

// deletes item from list at specified index
// returns 1 if successful
// returns 0 if error
int deleteAt(List* list, int index);

// free list and all elements from memory
// returns 1 if successful
// returns 0 if error
int deleteList(List* list);