#include "mystack.h"
#include "logging.h"
#include <string.h>
/* The head is an abstract data type.
 * this means that the internal structures are
 * never exposed to the users of the library.
 */

/* Note: the stacks have no knowledge of what types
 * they are storing. This is not a concern of the head
 */


typedef struct stackObject
{
	void* obj;
	struct stackObject* next;
} StackObject_t;

typedef struct stackMeta
{
	StackObject_t* head;
	size_t objsize;
	int numelem;
	int handle;
	struct stackMeta* next;
} StackMeta_t;

StackMeta_t* gStackList = NULL;


int MystackCreate(size_t objsize)
{
    /* Hint: use gStackList */
    int handle = 1;
    if(gStackList==NULL)
    {
        gStackList = malloc(sizeof(StackMeta_t));
        
        
        gStackList->head = malloc(sizeof(StackObject_t)); 
        gStackList->objsize = objsize;
        gStackList->numelem =0;
        gStackList->handle = handle;
        gStackList->next = NULL;
        
        printf("here\n\n");
    } 
    else 
    {
        StackMeta_t *stackListIterator = gStackList;
        while(stackListIterator->next!=NULL)
        {
            stackListIterator=stackListIterator->next;
            handle++;
        }
        StackObject_t *stack = malloc(sizeof(StackObject_t));
        StackMeta_t *newStack = malloc(sizeof(StackMeta_t));
        newStack->head = stack;
        newStack->objsize = objsize;
        newStack->numelem =0;
        newStack->handle = handle;
        newStack->next = NULL;
        stackListIterator->next = newStack;
    }

    DBG_PRINTF("Created head with handle: %d and objsize %zu bytes\n", handle, objsize);
    return handle;
}

int MystackPush(int handle, void* obj)
{
    StackObject_t *newObj = malloc(sizeof(StackObject_t));
    
    StackMeta_t *stackListIterator = gStackList;
    while(stackListIterator->next!=NULL&&stackListIterator->handle!=handle) //keeps cycling, stops when it finds handle
    {
        stackListIterator=stackListIterator->next;
    }
    if(stackListIterator->handle!=handle) return 1;
    newObj->obj = malloc(stackListIterator->objsize);
    newObj->next = NULL;
    memcpy(newObj->obj, obj, stackListIterator->objsize);
    printf("here\n\n");

    if(stackListIterator->head != NULL){
        newObj->next = stackListIterator->head;
    }
    
    stackListIterator->head = newObj;
    stackListIterator->numelem++;

    DBG_PRINTF("handle: %d\n, obj: %p, Val: %p\n", handle, obj,newObj->obj); 

    return 0;
}

int MystackPop(int handle, void* obj)
{
    

    StackMeta_t *stackListIterator = NULL;
    stackListIterator = gStackList;
    while(stackListIterator->next!=NULL) //keeps cycling, stops when it finds handle
    {
        if(stackListIterator->handle==handle) 
        {
            break;
        }
        stackListIterator=stackListIterator->next;
    }
    if(stackListIterator->handle!=handle)
    {
        return 1;
    }
    //printf("%i\n",stackListIterator->handle);
    if(stackListIterator->head !=NULL){
        //obj = stackListIterator->head->obj;
        memcpy(obj,stackListIterator->head->obj, stackListIterator->objsize);
        // memcpy(obj,charPtr,sizeof(char));
        StackObject_t *prevObject;
        prevObject = stackListIterator->head->next;
        free(stackListIterator->head->obj);
        stackListIterator->head->obj = NULL;
        free(stackListIterator->head);
        stackListIterator->head = NULL;
        stackListIterator->head = prevObject;
    } else {
        //printf("Here");
        obj = NULL;
        return -1;
    }

    /*
    StackObject_t stackInterator = stackListIterator->head;
    while(stackInterator->next!=NULL){
        stackInterator=stackInterator->next;
    }

    obj=stackInterator->obj;
    
    StackObject_t stackInterator2 = stackListIterator->head;
    while(stackInterator2->next!=stackInterator){
        stackInterator2=stackInterator2->next;
    }


    free(stackInterator);
    stackInterator2->next = NULL;
    */
    stackListIterator->numelem--;
    // int* debuggor = malloc(sizeof(int));
    // *debuggor=100;
    //obj=debuggor;

    //DBG_PRINTF("handle: %d\n, obj: %p, return Val: %s\n", handle, obj, (char*)obj); 	
    return 0;
}

int MystackDestroy(int handle)
{
    StackMeta_t *stackListIterator = gStackList;
    while(stackListIterator->next!=NULL&&stackListIterator->handle!=handle) 
    {
        stackListIterator=stackListIterator->next;
    }
    if(stackListIterator->handle!=handle) 
    {
        return 1;
    }
        

    
    StackObject_t *stackIterator = stackListIterator->head; 
    StackObject_t *toDestroy = NULL;
    
    while(stackIterator->next!=NULL)
    {
        if(toDestroy!=NULL)
        {
            free(toDestroy->obj);
            toDestroy->obj = NULL;
            free(toDestroy);
            toDestroy=NULL;
        }
        toDestroy=stackIterator;
        stackIterator=stackIterator->next;
    }
    free(stackIterator);
    stackIterator=NULL;
    StackMeta_t *stackListIterator2 = gStackList;
    
    while(stackListIterator2->next != NULL && stackListIterator2->next!= stackListIterator) 
    {
        stackListIterator2=stackListIterator2->next;
    }
    if(stackListIterator2->handle!=handle)
    {
        return 1;
    }

    stackListIterator2->next = stackListIterator->next;
    free(stackListIterator);
    stackListIterator=NULL;

    DBG_PRINTF("handle: %d\n", handle); 	
    return 0;
}

int MyStackDestroyAll()
{
    gStackList = NULL;
    return 0;
}

int MystackNofElem(int handle)
{
    StackMeta_t *stackListIterator = gStackList;
    while(stackListIterator->next!=NULL&&stackListIterator->handle!=handle) 
    {
        stackListIterator=stackListIterator->next;
    }
    if(stackListIterator->handle!=handle) return 1;
    return stackListIterator->numelem;

    DBG_PRINTF("handle: %d\n", handle);
    return 0;
}
