#include "user/user.h"
#include "kernel/types.h"

int main(int argc, char *argv[]){
    int count = getprocs();
    printf("There are %d active processes.\n",count);
    exit(0);
}