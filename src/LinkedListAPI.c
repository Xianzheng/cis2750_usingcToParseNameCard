#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinkedListAPI.h"

Node *initializeNode(void *data){
  Node *node=malloc(sizeof(Node));
  if(node!=NULL){
    node->data=data;
    node->next=NULL;
    node->previous=NULL;
  }
  return node;
}

List initializeList(char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second))
{
  List list;
  list.head=NULL;
  list.tail=NULL;
  list.length=0;
  list.deleteData=deleteFunction;
  list.printData=printFunction;
  list.compare=compareFunction;
  return list;

}

int getLength(List list)
{
	return list.length;
}

void insertBack(List *list, void *toBeAdded){
    if(list==NULL){
        return;
    }
  Node *temp = list->head;
  Node *new = initializeNode(toBeAdded);
  if(temp==NULL){
    list->head=new;
    list->tail=new;
    return;
  }
  while(temp->next!=NULL)temp=temp->next;
  temp->next=new;
  new->previous=temp;
  list->tail=new;
}

void insertFront(List *list, void *toBeAdded){
    if(list==NULL||toBeAdded==NULL){
        return;
    }
  Node *new =initializeNode(toBeAdded);
  Node *temp = list->head;
//  printf("InserFront: %d\n",list->length);
  if(list->head==NULL){
	  list->head=new;
      list->tail=new;
	  list->length++;
	  return;
  }
  list->head ->previous= new;
  new->next=list->head;
  list->head = new;
  while(temp->next!=NULL)temp=temp->next;
  list->tail=temp;
  list->length++;
}

void* getFromFront(List list)
{
    if(list.length==0)
    return NULL;
	return list.head->data;
}

void *getFromBack(List list){
    if(list.length==0)
    return NULL;
    return list.tail->data;
}

void* deleteDataFromList(List *list, void *toBeDeleted){
    if (list == NULL || toBeDeleted == NULL){
		return NULL;
	}

	Node* tmp = list->head;

	while(tmp != NULL){
		if (list->compare(toBeDeleted, tmp->data) == 0){
			//Unlink the node
			Node* delNode = tmp;

			if (tmp->previous != NULL){
				tmp->previous->next = delNode->next;
			}else{
				list->head = delNode->next;
			}

			if (tmp->next != NULL){
				tmp->next->previous = delNode->previous;
			}else{
				list->tail = delNode->previous;
			}

			void* data = delNode->data;
			free(delNode);
            list->length--;
			return data;

		}else{
			tmp = tmp->next;
		}
	}

	return NULL;
}

void clearList(List *list){
    if (list == NULL){
        return;
    }

    if (list->head == NULL && list->tail == NULL){
        return;
    }

    Node* tmp;

    while (list->head != NULL){
        list->deleteData(list->head->data);
        tmp = list->head;
        list->head = list->head->next;
        free(tmp);
    }

    list->head = NULL;
    list->tail = NULL;
}

char* toString(List list)
{
    ListIterator iter = createIterator(list);
	char* str;

	str = (char*)malloc(10000);
	strcpy(str, "");

	void* elem;
	while( (elem = nextElement(&iter)) != NULL){
		char* currDescr = list.printData(elem);
		int newLen = strlen(str)+50+strlen(currDescr);
		//str = (char*)realloc(str, newLen);
		strcat(str, "\n");
		strcat(str, currDescr);

		free(currDescr);
	}
    return str;
}
void* findElement(List list, bool (*customCompare)(const void* first,const void* second), const void* searchRecord)
{
    ListIterator iter = createIterator(list);
    void *data;
    while((data=nextElement(&iter))!=NULL){
        if(customCompare(data,searchRecord))
        return data;
    }
    return NULL;
}

void* nextElement(ListIterator* iter)
{
    Node* tmp = iter->current;

    if (tmp != NULL){
        iter->current = iter->current->next;
        return tmp->data;
    }else{
        return NULL;
    }
}
void insertSorted(List* list, void* toBeAdded)
{
    if (list == NULL || toBeAdded == NULL){
		return;
	}

	if (list->head == NULL){
		insertBack(list, toBeAdded);
		return;
	}

	if (list->compare(toBeAdded, list->head->data) <= 0){
		insertFront(list, toBeAdded);
		return;
	}

	if (list->compare(toBeAdded, list->tail->data) > 0){
		insertBack(list, toBeAdded);
		return;
	}

	Node* currNode = list->head;

	while (currNode != NULL){
		if (list->compare(toBeAdded, currNode->data) <= 0){

			char* currDescr = list->printData(currNode->data);
			char* newDescr = list->printData(toBeAdded);

			printf("Inserting %s before %s\n", newDescr, currDescr);

			free(currDescr);
			free(newDescr);

			Node* newNode = initializeNode(toBeAdded);
			newNode->next = currNode;
			newNode->previous = currNode->previous;
			currNode->previous->next = newNode;
			currNode->previous = newNode;

			return;
		}

		currNode = currNode->next;
	}

	return;
}
ListIterator createIterator(List list){
    ListIterator iter;

    iter.current = list.head;

    return iter;
}
