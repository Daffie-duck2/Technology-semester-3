#include "linked_list.h"
#include <stdio.h> //HAAL DIT WEG DEBIEL

static size_t MaxLength = 100;



ListAdmin* InitialiseList()
{
    ListAdmin* this = malloc(sizeof(ListAdmin));
    //this->head = malloc(MaxLength * sizeof(*(this->head)));
    this->head = NULL;
    this->count = 0;
    return this;
}


/* function: ListAddTail
 * pre: -
 * post: an element is added to the end of the linked list
 * returns: 0 on success, -1 on fail
 */
int ListAddTail(ListAdmin* this, int address, int size)
{
    if(this->count >= MaxLength)
    {
        return -1;
    }
    Element *newElement = malloc(sizeof(Element));
    newElement->address = address;
    newElement->size = size;
    newElement->next = NULL;
    if (this->head == NULL)
    {
        this->head = newElement;
    }
    else
    {
        Element *iterator = ListGetHead(this);
        while (iterator->next != NULL)
        {
            iterator = iterator->next;
        }
        iterator->next = newElement;
    }
    return 0;
}

/* function: ListAddAfter
 * pre: -
 * post: a new element with given data is added after element
 *       in the linked list. If element is NULL, the new element
 *       is added to the front of the list.
 * returns: 0 on success, -1 on fail
 */
int ListAddAfter(ListAdmin* this, int address, int size, Element *element)
{

    if (this->head == NULL)
    {
        ListAddTail(this, address, size);
    }
    Element *newElement = malloc(sizeof(Element));
    newElement->address = address;
    newElement->size = size;
    if (element == NULL)
    {
        newElement->next = this->head;
        this->head = newElement;
    }
    else
    {
        Element *iterator = ListGetHead(this);
        while (iterator->size < element->size)
        {
            iterator = iterator->next;
        }
        newElement->next = iterator->next;
        iterator->next = newElement;
    }
    return 0;
}

// /* function: ListGetHead
//  * pre: -
//  * post: first element in linked list is returned
//  * returns: first element on success, NULL if list is empty
//  */
Element *ListGetHead(ListAdmin* this)
{
    printf("HAOLLOOOLOO");
    if(this->head==NULL)printf(" HAHA NO HEAD");
    return this->head;
}

// /* function: ListRemoveTail
//  * pre: -
//  * post: last element is removed from list
//  * returns: 0 on success, -1 on fail
//  */
int ListRemoveTail(ListAdmin* this)
{
    Element *Second_Last = ListGetHead(this);
    if (Second_Last == NULL)
    {
        return -1;
    }
    else if (Second_Last->next == NULL)
    {
        free(this->head);
        return 0;
    }
    while (Second_Last->next->next != NULL)
    {
        Second_Last = Second_Last->next;
    }
    free(Second_Last->next);
    Second_Last->next = NULL;

    return 0;
}

// /* function: ListRemove
//  * pre: -
//  * post: element is removed from list, pointer from callee is made NULL
//  * returns: 0 on success, -1 on fail
//  */
int ListRemove(ListAdmin* this, Element **element){
    
    if (element == NULL)
    {
        return -1;
    }
    Element *temp = ListGetHead(this);
    if(temp->next != NULL){
        while (temp->next != *element)
        {
            temp = temp->next;
        }
        temp->next = temp->next->next;
        this->count--;
        free(temp->next);
        temp->next = NULL;
    } else {
        free(this->head);
        this->head = NULL;
        this->count=0;
    }

    return 0;
}

// /* function: ListRemoveAll
//  * pre: -
//  * post: all existing elements from list are removed
//  */
void ListRemoveAll(ListAdmin* this)
{
    Element *temp = ListGetHead(this);
    while (this->head != NULL)
    {
        temp = this->head;
        this->head = this->head->next;
        free(temp);
    }
}