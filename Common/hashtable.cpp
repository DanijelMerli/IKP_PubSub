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

    return table;
}

bool table_add(HashTable* table, char* key, SOCKET value)
{
    int index = hash(key);

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
            return NULL;
        }
    }

    return list_addAtEnd(item->list, value);
}

List* table_get(HashTable* table, char* key)
{
    int index = hash(key);

    TableItem* item = &table->items[index];

    if (item == NULL)
    {
        return NULL;
    }

    return item->list;
}

bool table_hasKey(HashTable* table, char* key)
{
    int index = hash(key);

    TableItem* item = &table->items[index];

    if (item->list == NULL)
    {
        return false;
    }

    return true;
}

bool table_dump(HashTable* table)
{
    if (table == NULL)
    {
        printf("null arguement error\n");
        return false;
    }

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

    free(table->items);
    free(table);
    return true;
}

void table_print(HashTable* table)
{
    int j;
    TableItem* item;
    ListItem* current;

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
}