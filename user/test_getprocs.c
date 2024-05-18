#include "user/user.h"

int main(int argc, char *argv[]){
    unsigned long count = getprocs();
    printf("There are %d active processes.\n",count);
    exit(0);
}