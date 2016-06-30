/************************************************
* This file contains methods used for a doubly 
* linked circular list implementation
************************************************/
#include<stdio.h>
#include "Common.h"

/* Initialise the list */
void ListInit(LinkedList *head)
{
	head->next = head;
	head->prev = head;
}

/**************************************
* Insert entry at the tail of the list 
***************************************/
void ListInsertTail(LinkedList *head, LinkedList *entry)
{
	if (!entry)
		return;

	entry->prev = head->prev;
	entry->next = head;

	(head->prev)->next = entry;
	head->prev = entry;
}

/***************************************
* Return the first element on the list 
****************************************/
LinkedList *ListFirst(LinkedList *head)
{
	if (head->next == head)
		return NULL;
	return head->next;
}

/********************************************************** 
* Return the next element to entry. Return null if entry is
* the last element on thne list 
***********************************************************/
LinkedList *ListNext(LinkedList *head, LinkedList *entry)
{
	if (head->next == head)
		return NULL;
	if (entry == NULL)
		return NULL;
	if (entry->next == head)
		return NULL;
	else
		return (entry->next);
}

/**********************************************************
* Remove the first element from the list and return it 
***********************************************************/
LinkedList *ListRemoveHead(LinkedList *head)
{
	LinkedList *temp = ListFirst(head);

	if (temp)
	{
		head->next = temp->next;
		(temp->next)->prev = head;
	}
	return temp;
}

