#include "user.h"

int main(int argc, char *argv[]){
    int* a = (int*)dalloc(4);
    int b =  16;
    a = &b;
    printf("the addr of a is %d, the value of a is %d\n",a, *a);
    dfree(a);
    exit(0);
}
