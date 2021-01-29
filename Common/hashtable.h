#pragma once
#include <string.h>
#include "list.h"

#define TABLE_SIZE 2000
#define MAX_KEYLEN 100

typedef struct
{
    char* key;
    List* list;
} TableItem;

typedef struct
{
    CRITICAL_SECTION cs;
    TableItem* items;
} HashTable;

// calculate hash based on input string
unsigned int hash(char* key);

// initialize new list
// returns pointer to new list or NULL if error
HashTable* table_init();

// add new list to table at specified key
// returns true if successful
// returns false if error
bool table_addList(HashTable* table, char* key);

// add socket to end of list at specified key (if it exists) 
// returns true if successful
// returns false if error
bool table_addSocket(HashTable* table, char* key, SOCKET value);

// get list at specified key
// returns NULL if error
List* table_get(HashTable* table, char* key);

// look for specified key in table
// returns true if found
// returns false if not found
bool table_hasKey(HashTable* table, char* key);

// delete table and it's contents from memory
// returns true if successful
// returns false if error
bool table_dump(HashTable* table);

// prints table and it's contents
void table_print(HashTable* table);