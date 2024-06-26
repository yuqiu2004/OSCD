#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_kmAddr(void){
  printf("init addr is: %p, sizeof struct node is %d \n",getAddr(), getSizeofStruct());
  return 0;
}

uint64
sys_d1(void){
  printf("dalloc a int mem to a...\n");
  int *a = (int*)dalloc(sizeof(int));
  printf("addr of a is %p\n",a);
  printf("dfree addr of a...\n");
  dfree(a);
  return 0;
}

uint64
sys_d2(void){
  printf("dalloc a long mem to b...\n");
  long *b = (long*)dalloc(sizeof(long));
  printf("addr of b is %p\n",b);
  return 0;
}

uint64
sys_d3(void){
  printf("---------------------------------------------------------------------------------\n");
  void *p1 = dalloc(2048*4096-1024), *p2 = dalloc(16), *p3 = dalloc(16);
  printf("addresses of them are:\np1-%p\np2-%p\np3-%p\n",p1,p2,p3);
  
  printf("kfree p1...\n");
  dfree(p1);

  printf("dalloc big block...");
  p1 = dalloc(2048*4096-2048);
  printf("addr of p1 is %p\n",p1);

  printf("dfree all...\n");
  dfree(p1);
  dfree(p2);
  dfree(p3);
  printf("---------------------------------------------------------------------------------\n");
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
