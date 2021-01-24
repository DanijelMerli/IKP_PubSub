#pragma once
#include "list.h"
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>

List* initList(int limit)
{
	List* lista = NULL;
	lista = (List*)malloc(sizeof(lista));
	lista->head = NULL;
	lista->limit = limit;
	lista->len = 0;

	return lista;
}

bool addAtEnd(List* list, ListItem* item)
{
	ListItem* novi = NULL;

	List* lista;
	novi = (ListItem*)malloc(sizeof(ListItem));
	lista = (List*)malloc(sizeof(lista));
	lista = list;
	novi->data = item->data;
	novi->next = NULL;

	if (novi == NULL)
	{
		printf("nije moguce \n");
		return false;
	}


	ListItem* current = lista->head;


	if (lista->len < lista->limit)
	{
		if (current == NULL)
		{
			current = novi;
			lista->head = current;
			lista->len++;
			return true;
		}
		else
		{
			while (current->next != NULL)
			{
				current = current->next;
			}

			current->next = novi;
			lista->len++;

			printf("Uspesno dodato na kraj liste\n");
		}
	}
	else {
		printf("Lista je puna!\n");
		return false;
	}

	return true;
}

bool deleteAt(List* list, int index)
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

bool deleteList(List* list)
{
	List* lista = list;
	ListItem* current = lista->head;
	ListItem* temp;

	while (current != NULL)
	{
		temp = current->next;
		free(current);
		current = temp;
	}
	list->head = NULL;


	return true;
}
