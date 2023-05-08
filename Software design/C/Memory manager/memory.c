#include "memory.h"
#include "linked_list.h"

static const int StartAddress = 1000;

ListAdmin* freeList;
ListAdmin* allocList;

/* function: ConstructMemory
 * pre: -
 * post: memory administration is constructed
 */
void ConstructMemory(int size)
{
    freeList = InitialiseList();
    allocList = InitialiseList();
    ListAddTail(freeList,StartAddress,size);
    //ListAddTail(allocList,0,0);
}

/* function: DestructMemory
 * pre: -
 * post: memory administration is destructed
 */
void DestructMemory()
{
    ListRemoveAll(freeList);
    ListRemoveAll(allocList);
    freeList=NULL;
    allocList=NULL;
}


/* function: PrintList
 * pre: -
 * post: if stream is valid, list information is printed to stream and 0 is returned
 *       if stream is NULL, -1 is returned
 */
int PrintList(FILE* stream)
{   
    if(stream==NULL)
    {
        return -1;
    }

    printf("FreeList:\n---------\n");
    Element *iterator = ListGetHead(freeList);
    while (iterator != NULL)
    {
        printf("<Addr:%d  Size:%d>\n" , iterator->address, iterator->size);
        iterator = iterator->next;
    }
    printf("AllocList:\n----------\n");
    Element *iterator2 = ListGetHead(allocList);
    printf("JA BIJNA");
    while (iterator2 != NULL)
    {
        printf("<Addr:%d  Size:%d>\n" , iterator2->address, iterator2->size);
        iterator2 = iterator2->next;
    }
    printf("GOEDZO KNUL");
    return 0;
}

/* function: ClaimMemory
 * pre: nrofBytes > 0
 * post: if no memory block of nrofBytes available return -1
 *       otherwise the first block that is large enough is claimed and the start address is returned
 */
int ClaimMemory(int nrofBytes)
{
    printf("segfault");
    Element *iterator = ListGetHead(freeList);
    printf(" aaa ");
    int blockAllocated=0;
    while (iterator != NULL)
    {
        if(iterator->size>=nrofBytes){
            Element *iterator2 = ListGetHead(allocList);
            if(iterator2!=NULL){
                printf(" help");
                while(iterator2->address-iterator2->size!=iterator->address){
                    iterator2=iterator2->next;
                }
                printf(" HAHAHA");
                ListAddAfter(allocList,iterator->address, nrofBytes,iterator2->next);
                blockAllocated=1;
                break;
            } else {
                ListAddTail(allocList,iterator->address, nrofBytes);
                blockAllocated=1;
            }
        }

        iterator=iterator->next;
    }
    if(blockAllocated!=1) return -1;
    return iterator->address;
}

/* function: FreeMemory
 * pre: Parameter addr must match the start of an allocatd memory block. Otherwhise return -1.
 * post: Memory is freed and the number of freed bytes is returned
 *
 */
int FreeMemory(int addr)
{
    Element *iterator = ListGetHead(allocList);
    Element *iterator2 = ListGetHead(freeList);
    int deletedMemory=0;
    while (iterator != NULL){
        if(iterator->address == addr){
            while(iterator2 !=NULL){
                if(iterator->size+iterator->address+1==iterator2->address){
                    iterator2->size +=iterator->size; //add bits back to free total
                    iterator2->address = iterator->address; //set the "free from x" address back to where the iterator was.
                }
                iterator2=iterator2->next;
            }
            ListRemove(allocList,&iterator);
            deletedMemory=0;
            break;
        }
        iterator=iterator->next;
    }
    if(deletedMemory!=1)return -1; //ran out of alloclist parts without

    iterator2=ListGetHead(freeList);
    while(iterator2 != NULL){
        if(iterator2->address+iterator2->size==iterator2->next->address){
            iterator2->size += iterator2->next->size;
            ListRemove(freeList,&iterator2->next);
        }
        iterator2=iterator2->next;
    }


    return 0;
}
