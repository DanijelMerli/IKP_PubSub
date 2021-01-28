#pragma once
#include <string.h>
#include "list.h"

#define TABLE_SIZE 2000
#define MAX_KEYLEN 100

typedef struct TableItem_st
{
    char* key;
    List* list;
} TableItem;

typedef struct Hashtable_st
{
    TableItem* items;
} HashTable;

unsigned int hash(char* key);

HashTable* table_init();

bool table_add(HashTable* table, char* key, char* value);

List* table_get(HashTable* table, char* key);

bool table_dump(HashTable* table);

void table_print(HashTable* table);