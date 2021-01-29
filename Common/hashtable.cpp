#include "hashtable.h"

unsigned int hash(char* key)
{
    unsigned int value = 0;
    unsigned int key_len = strlen(key);

    for (int i = 0; i < key_len; ++i)
    {
        value = value * 37 + key[i];
    }

    value = value % TABLE_SIZE;

    return value;
}

HashTable* table_init()
{
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));

    table->items = (TableItem*)malloc(sizeof(TableItem) * TABLE_SIZE);
    
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table->items[i].list = NULL;
        table->items[i].key = (char*)malloc(MAX_KEYLEN);
    }

    InitializeCriticalSection(&table->cs);

    return table;
}

bool table_addList(HashTable* table, char* key)
{
    int index = hash(key);

    EnterCriticalSection(&table->cs);
    TableItem* item = &(table->items[index]);
    int iResult;

    if (item->list == NULL)
    {
        item->list = list_init(DEFAULT_LIST_LIMIT);
        if (item->list == NULL)
        {
            printf("Adding new list to hash table failed.\n");
            return false;
        }

        if ((iResult = strcpy_s(item->key, MAX_KEYLEN, key)) != 0)
        {
            printf("strcpy failed with error: %d\n", iResult);
            return false;
        }
    }
    else
    {
        printf("Specified key already exists in hash table.\n");
        return false;
    }

    LeaveCriticalSection(&table->cs);
    return true;
}

bool table_addSocket(HashTable* table, char* key, SOCKET value)
{
    int index = hash(key);

    EnterCriticalSection(&table->cs);

    TableItem* item = &(table->items[index]);
    int iResult;
    
    if (item->list == NULL)
    {
        printf("Key doesn't exist in hasht table.\n");
        return false;
    }

    bool retVal = list_addAtEnd(item->list, value);
    
    LeaveCriticalSection(&table->cs);
    return retVal;
}

List* table_get(HashTable* table, char* key)
{
    int index = hash(key);

    EnterCriticalSection(&table->cs);

    TableItem* item = &(table->items[index]);

    if (item == NULL)
    {
        return NULL;
    }

    LeaveCriticalSection(&table->cs);

    return item->list;
}

bool table_hasKey(HashTable* table, char* key)
{
    int index = hash(key);

    EnterCriticalSection(&table->cs);

    TableItem* item = &(table->items[index]);

    if (item->list == NULL)
    {
        return false;
    }

    LeaveCriticalSection(&table->cs);

    return true;
}

bool table_dump(HashTable* table)
{
    if (table == NULL)
    {
        printf("null arguement error\n");
        return false;
    }

    EnterCriticalSection(&table->cs);

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        TableItem* item = &table->items[i];

        if (item->list == NULL)
        {
            continue;
        }
        else
        {
            list_dump(item->list);
        }

        if (item->key != NULL)
        {
            free(item->key);
        }
    }

    LeaveCriticalSection(&table->cs);
    free(table->items);
    DeleteCriticalSection(&table->cs);
    free(table);
    return true;
}

void table_print(HashTable* table)
{
    int j;
    TableItem* item;
    ListItem* current;

    EnterCriticalSection(&table->cs);

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        item = &(table->items[i]);

        if (item == NULL || item->list == NULL || item->list->len == 0)
        {
            continue;
        }

        j = 0;

        current = item->list->head;

        printf("Table row %d:\n", i);
        while (current != NULL)
        {
            printf("Socket: %d\n", current->data);
            current = current->next;
        }
        printf("\n");
    }

    LeaveCriticalSection(&table->cs);
}