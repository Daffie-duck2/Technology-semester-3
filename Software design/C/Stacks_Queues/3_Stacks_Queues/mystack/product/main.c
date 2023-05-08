#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#include "mystack.h"

bool isOperator(char c){
	switch (c){
		case '+':
		case '-':
		case '*':
		case '/':
        case 'S':
        case 'R':
			return true;
		default:
			return false;
	}
}

int main ()
{
    printf(" *** Calculator ***\n");
    int stack = MystackCreate(sizeof(char));
    char operation[80];
    scanf("%s",operation);
        
    int i=0;
    int tempA=0;
    int tempB=0;
    int result = 0;
    while(operation[i]!='\0'){
        printf("Now evaluating: %c, %x  ", operation[i], operation[i]);
        if(isOperator(operation[i])){
            printf(" It's an operator, working...");
            switch(operation[i]){
                case '+': {
                    char ret;
                    MystackPop(stack,&ret);
                    tempA=ret;
                    MystackPop(stack,&ret);
                    tempB=ret;
                    result = tempA+tempB;
                    //*tempOutPtr=tempA+tempB;
                    printf(" Received %d and %d, calculating...",tempA,tempB);
                    MystackPush(stack,&result);
                } break;
		        case '-': {
                    char ret;
                    MystackPop(stack,&ret);
                    tempA=ret;
                    MystackPop(stack,&ret);
                    tempB=ret;
                    result = tempB-tempA;
                    //*tempOutPtr=tempA+tempB;
                    printf(" Received %d and %d, calculating...",tempA,tempB);
                    MystackPush(stack,&result);
                } break;
		        case '*': {
                    char ret;
                    MystackPop(stack,&ret);
                    tempA=ret;
                    MystackPop(stack,&ret);
                    tempB=ret;
                    result = tempA*tempB;
                    //*tempOutPtr=tempA+tempB;
                    printf(" Received %d and %d, calculating...",tempA,tempB);
                    MystackPush(stack,&result);
                } break;
		        case '/': {
                    char ret;
                    MystackPop(stack,&ret);
                    tempA=ret;
                    MystackPop(stack,&ret);
                    tempB=ret;
                    result = tempA/tempB;
                    //*tempOutPtr=tempA+tempB;
                    printf(" Received %d and %d, calculating...",tempA,tempB);
                    MystackPush(stack,&result);
                } break;
                case 'S': {
                    char ret;
                    MystackPop(stack,&ret);
                    tempA=ret;
                    result = tempA*tempA;
                    //*tempOutPtr=tempA+tempB;
                    printf(" Received %d, calculating...",tempA);
                    MystackPush(stack,&result);
                } break;
                case 'R': {
                    char ret;
                    MystackPop(stack,&ret);
                    tempA=ret;
                    result = sqrt(tempA);
                    //*tempOutPtr=tempA+tempB;
                    printf(" Received %d, calculating...",tempA);
                    MystackPush(stack,&result);
                } break;
            }
        } else {
            int val = operation[i]-48; 
            printf(" It's a number, storing %d...", val);
            MystackPush(stack, &val);
        }
        
        i++;
    }
    char finalResult;
    MystackPop(stack,&finalResult);

    printf("The result is %d\n\n",finalResult);
    MystackDestroy(stack);
    MyStackDestroyAll();

    return 0;
}
