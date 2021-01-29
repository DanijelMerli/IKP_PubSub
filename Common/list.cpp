#pragma once
#include "list.h"

List* list_init(int limit)
{
	List* lista = (List*)malloc(sizeof(List));
	lista->head = NULL;
	lista->limit = limit;
	lista->len = 0;

	return lista;
}

bool list_addAtEnd(List* list, SOCKET data)
{
	if (list->len < list->limit)
	{
		ListItem* item = (ListItem*)malloc(sizeof(ListItem));
		item->data = data;
		item->next = NULL;

		ListItem* current = list->head;

		if (current == NULL)
		{
			current = item;
			list->head = current;
			list->len++;
			return true;
		}
		else
		{
			while (current->next != NULL)
			{
				current = current->next;
			}

			current->next = item;
			list->len++;
		}
	}
	else {
		printf("Lista je puna!\n");
		return false;
	}

	return true;
}

SOCKET* list_getAt(List* list, int index)
{
	if (list == NULL)
	{
		printf("List uninitialized.\n");
		return NULL;
	}
	if (list->len <= index)
	{
		printf("Index out of bounds.\n");
		return NULL;
	}
	if (index == 0)
	{
		return &(list->head->data);
	}

	ListItem* current = list->head;

	for (int i = 1; i <= index; i++)
	{
		current = current->next;
	}

	return &(current->data);
}

bool list_deleteAt(List* list, int index)
{
	List* lista = list;
	ListItem* pomocni = lista->head;
	ListItem* temp = NULL;
	temp = (ListItem*)malloc(sizeof(ListItem));

	if (lista->len < index) {
		printf("Item not found!\n");
		return false;
	}


	for (int i = 1; i < index - 1; i++)
	{
		if (pomocni->next == NULL)
		{
			return false;
		}
		pomocni = pomocni->next;

	}

	temp = pomocni->next;
	pomocni->next = temp->next;

	lista->len--;
	free(temp);

	return true;
}

bool list_dump(List* list)
{
	if (list == NULL)
	{
		printf("list is nullptr\n");
		return false;
	}

	if (list->len == 0)
	{
		free(list);
		return true;
	}

	ListItem* current = list->head;
	ListItem* temp;

	while (current != NULL)
	{
		temp = current->next;
		closesocket(current->data);
		free(current);
		current = temp;
	}
	
	list->head = NULL;

	return true;
}
