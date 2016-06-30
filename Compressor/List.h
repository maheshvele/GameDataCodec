typedef struct LinkedList{

	struct LinkedList *next;
	struct LinkedList *prev;

}LinkedList;

void ListInit(LinkedList *head);
void ListInsertTail(LinkedList *head, LinkedList *entry);
LinkedList *ListFirst(LinkedList *head);
LinkedList *ListNext(LinkedList *head, LinkedList *entry);
int ListGetCnt(LinkedList *head);