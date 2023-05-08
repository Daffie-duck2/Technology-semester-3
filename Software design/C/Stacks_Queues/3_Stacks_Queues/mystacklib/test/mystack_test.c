#include <string.h>
#include <stdio.h>

#include "mystack.h"
#include "unity.h"

// I rather dislike keeping line numbers updated, so I made my own macro to ditch the line number
#define MY_RUN_TEST(func) RUN_TEST(func, 0)

void setUp()
{
}

void tearDown()
{
    // This is run after EACH test
}

// TODO:
//  - Rename and change this test to something usefull
//  - Add more tests
//  - Remove this comment :)
// Should you need a list of all TEST_ASSERT macros: take a look
// at unity.h
void test_EmptyTest()
{
    TEST_ASSERT_EQUAL(1, 0);
}

void test_happyFlow()
{
    int stackHandle = MystackCreate(100);
    TEST_ASSERT_EQUAL(1,stackHandle);
    int *val = malloc(sizeof(int));
    *val = 100;
    MystackPush(stackHandle, val);
    /*char *retval = malloc(sizeof(char));
    *retval = 120;
    */
    char retval;
    printf("HALLO IK GA WEG DIT IS DE %d\n",retval);
    MystackPop(stackHandle, &retval);
    int itemCount = MystackNofElem(stackHandle);
    TEST_ASSERT_EQUAL(0,itemCount);
    printf("HALLO IK BEN ER WEER DIT IS DE %d\n",retval);
    TEST_ASSERT_EQUAL(100, retval);


}

void test_create_stack()
{
    int stackHandle = MystackCreate(sizeof(int));
    MystackDestroy(stackHandle);
    MyStackDestroyAll();

}

void test_push_pop()
{
    int stackHandle = MystackCreate(sizeof(int));
    // int i = 2;
    // int j = 0;
    //MystackPush(stackHandle, &i);
    // TEST_ASSERT_EQUAL_INT(0, MystackPush(stackHandle, &i));
    // TEST_ASSERT_EQUAL_INT(0, MystackPop(stackHandle, &j));
    //TEST_ASSERT_EQUAL_INT(2, j);
    MystackDestroy(stackHandle);
    MyStackDestroyAll();
}


int main()
{
    UnityBegin();

    //MY_RUN_TEST(test_EmptyTest);
    //MY_RUN_TEST(test_happyFlow);
    MY_RUN_TEST(test_create_stack);
    MY_RUN_TEST(test_push_pop);

    return UnityEnd();
}
