#include "user/user.h"

int main(int argc, char *argv[]){
    unsigned long count = getprocs();//get the result via using system call defined.
    printf("------------------------------\n");
    switch (count)
    {
    case 1 : printf("There is 1 active process.\n",count);
        break;
    default: printf("There are %d active processes.\n",count);
        break;
    }
    printf("------------------------------\n");
    exit(0);
}