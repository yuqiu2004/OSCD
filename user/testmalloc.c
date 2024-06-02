#include "user.h"

int main(int argc, char *argv[]){
    int* a = (int*)dalloc(sizeof(int));
    *a = 1;
    printf("the addr of int a is %d, the value of a is %d\n",a, *a);

    char* b = (char*)dalloc(sizeof(char));
    *b = 'b';
    printf("the addr of int a is %d, the value of a is %c\n",b, *b);

    exit(0);
}
