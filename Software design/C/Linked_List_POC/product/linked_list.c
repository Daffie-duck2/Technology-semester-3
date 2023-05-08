#include "linked_list.h"

#include <stdlib.h>

static Element *head = NULL;

/* function: ListAddTail
 * pre: -
 * post: an element is added to the end of the linked list
 * returns: 0 on success, -1 on fail
 */
int ListAddTail(int address, int size)
{
    Element *newElement = malloc(sizeof(Element));
    newElement->address = address;
    newElement->size = size;
    newElement->next = NULL;
    if (head == NULL)
    {
        head = newElement;
    }
    else
    {
        Element *iterator = ListGetHead();
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
int ListAddAfter(int address, int size, Element *element)
{

    if (head == NULL)
    {
        ListAddTail(address, size);
    }
    Element *newElement = malloc(sizeof(Element));
    newElement->address = address;
    newElement->size = size;
    if (element == NULL)
    {
        newElement->next = head;
        head = newElement;
    }
    else
    {
        Element *iterator = ListGetHead();
        while (iterator->size < element->size)
        {
            iterator = iterator->next;
        }
        newElement->next = iterator->next;
        iterator->next = newElement;
    }
    return -1;
}

// /* function: ListGetHead
//  * pre: -
//  * post: first element in linked list is returned
//  * returns: first element on success, NULL if list is empty
//  */
Element *ListGetHead()
{
    return head;
}

// /* function: ListRemoveTail
//  * pre: -
//  * post: last element is removed from list
//  * returns: 0 on success, -1 on fail
//  */
int ListRemoveTail()
{
    Element *Second_Last = ListGetHead();
    if (Second_Last == NULL)
    {
        return -1;
    }
    else if (Second_Last->next == NULL)
    {
        free(head);
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
int ListRemove(Element **element) //checken of element wel in de linked list zit
{
    if (element == NULL)
    {
        return -1;
    }
    Element *temp = ListGetHead();
    while (temp->next != *element)
    {
        temp = temp->next;
    }
    //temp->next = NULL;
    /*Element *prev = ListGetHead();
    while (prev->next != temp)
    {
        prev = prev->next;
    }
    prev = temp->next->next;
    */
    free(temp->next);
    temp->next = temp->next->next;
    
    

    return -1;
}

// /* function: ListRemoveAll
//  * pre: -
//  * post: all existing elements from list are removed
//  */
void ListRemoveAll()
{
    Element *temp = ListGetHead();
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}
