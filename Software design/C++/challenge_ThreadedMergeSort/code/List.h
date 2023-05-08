#ifndef LIST_H
#define LIST_H

#include "Log.h"
#include "Value.h"
#include <thread>

class List
{
public:
    List(Log& log);
    // post: List is properly initialised, however the key value
    //       is illegal (empty)

    virtual ~List();
    // post: recursively deletes all keys and values

    List(const List&) = delete;
    List& operator=(const List&) = delete;

    void Add(const std::string& word);
    // post: a new word is added to the list

    Value* GetHead();
    // post: pointer to this key's head is returned

    void SetHead(Value* value);
    // post: pointer to this key's head is set

    void Sort();
    // post: all values are sorted, using merge sort

private:
    Value* head;
    Log& log;

    static Value* SortList(Value* currentHead, Value* listPtr);
    static void SplitList(Value* head, Value** left, Value** right);
    static Value* MergeLists(Value* left, Value* right);
};

#endif
