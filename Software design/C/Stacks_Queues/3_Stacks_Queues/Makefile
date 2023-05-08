ASSIGMENT=stack
ASSIGMENT_TEST=$(ASSIGMENT)_test

UNITY_FOLDER=./mystacklib/Unity
INC_DIRS=-Iproduct -Imystacklib/src
TEST_INC_DIRS=-Imystacklib/test -I$(UNITY_FOLDER)

TEST_FILES=$(UNITY_FOLDER)/unity.c \
			./mystacklib/test/mystack_test.c

MAIN_FILE=./mystack/product/main.c \

HEADER_FILES=mystack/product/*.h \
			 mystacklib/src/*.h \
			 mystacklib/src/*.h

LIB_FILES=./mystacklib/src/mystack.c

TARGET_LIB=libmystack.so

CC = gcc
CFLAGS=-Wall -Wextra -Werror -std=c99 -DDEBUG=1 -g -O0

LIB_CFLAGS = $(CFLAGS) -fPIC
LIB_LDFLAGS = -shared 

all: $(ASSIGMENT) $(TARGET_LIB) $(ASSIGMENT_TEST)

$(TARGET_LIB): Makefile $(LIB_FILES)
	$(CC) $(LIB_CFLAGS) $(LIB_LDFLAGS) $(LIB_FILES) -o $(TARGET_LIB)

$(ASSIGMENT): Makefile $(MAIN_FILE) $(HEADER_FILES)
	$(CC) $(INC_DIRS) $(CFLAGS) $(MAIN_FILE) -Wl,-rpath=. -L. -o $(ASSIGMENT) -lmystack -lm

$(ASSIGMENT_TEST) : Makefile $(TEST_FILES) $(HEADER_FILES)
	$(CC) $(INC_DIRS) $(TEST_INC_DIRS) $(CFLAGS) $(TEST_FILES) -Wl,-rpath=. -L. -o $(ASSIGMENT_TEST) -lmystack

stacklib: $(TARGET_LIB)

main: $(ASSIGMENT)
	@valgrind ./$(ASSIGMENT)

test: $(ASSIGMENT_TEST)
	./$(ASSIGMENT_TEST)

vtest: $(ASSIGMENT_TEST)
	   @valgrind ./$(ASSIGMENT_TEST)

clean:
	@rm -f $(ASSIGNMENT) $(ASSIGNMENT_TEST) $(TARGET_LIB)
	@rm -rf kwinject.out .kwlp .kwps