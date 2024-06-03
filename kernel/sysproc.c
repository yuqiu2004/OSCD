#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_test(void){
  printf("init addr is: %p, sizeof struct node is %d \n\n",getAddr(), getSizeofStruct());

  int *a = (int*)dalloc(sizeof(int));
  *a = 1;
  printf("dalloc int  a:\n\tvalue:%d\t\taddr:%p\tsize:%d\n",*a,a,sizeof(int));

  short *b = (short*)dalloc(sizeof(short));
  *b = 8;
  printf("dalloc short b:\n\tvalue:%d\t\taddr:%p\tsize:%d\n",*b,b,sizeof(short));

  long *c = (long*)dalloc(sizeof(long));
  *c = 24;
  printf("dalloc long b:\n\tvalue:%d\taddr:%p\tsize:%d\n",*c,c,sizeof(long));

  dfree(c);
  printf("dfree memory of c...\n");

  int *d = (int*)dalloc(sizeof(int));
  *d = 23;
  printf("dalloc int  a:\n\tvalue:%d\taddr:%p\tsize:%d\n",*d,d,sizeof(int));

  return 0;
}

uint64
sys_getprocs(void){
  return getprocs();
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
