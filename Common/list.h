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
// returns true if successful
// returns false if error
bool addAtEnd(List* list, ListItem* item);

// deletes item from list at specified index
// returns true if successful
// returns false if error
bool deleteAt(List* list, int index);

// free list and all elements from memory
// returns true if successful
// returns false if error
bool deleteList(List* list);