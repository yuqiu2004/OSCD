// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

/**
 * 动态分区的结点 节点本身占用16字节
 */ 
struct node{
  unsigned long size;
  struct node *next;
};

struct kheap{
  struct spinlock lock;
  struct node *head;
};

struct kheap heap;

/**
 * 初始化内存 同时初始化自旋锁 
 */
void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&heap.lock,"kheap");
  freerange(end, (void*)PHYSTOP);
}

/**
 * 此处进行初始化内存空间的时候堆内存要单独处理 不然就会也被加入到固定分配的链表中
*/
void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end - 2048*PGSIZE; p += PGSIZE) // 一页4KB 吸血吸8MB=2K页用于动态内存分区
    kfree(p);
  memset(p,1,2048*PGSIZE);
  acquire(&heap.lock);
  heap.head = (struct node*)p;
  heap.head->next = heap.head;
  heap.head->size = 2048*PGSIZE - 16;
  release(&heap.lock);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist; // 头插法将当前块插入到空闲块链表中
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

void *
dalloc(uint size){
  size += sizeof(struct node*);
  struct node *t, *pre, *p;
  pre = t = heap.head;
  for(; pre<=t; pre=t,t=t->next){
    if(t->size>=size) break;
  }
  if(t->size<size) return 0;
  if(t->size>size) {
    t->size -= size;
    char *temp = (char *)t;
    temp = temp + t->size;
    p = (struct node *)temp;
    p->size = size - sizeof(struct node*);
  }else{
    t->size = 0;
    p = t;
    p->size = size - sizeof(struct node*);
  }
  return (void*)(p+1);
}

void
dfree(void* p){
  struct node *t, *fr;
  fr = (struct node*) p - 1;
  for(t=heap.head; !(fr>t && fr<t->next); t=t->next){
    if(t>=t->next && (fr>t || fr<t->next)) break;
  }
  char *ct = (char *)t, *cfr = (char *)fr;
  if(cfr + fr->size == (char*)t->next){ // 空闲块和下一块紧邻 合并
    fr->size += (t->next->size+sizeof(struct node*));
    fr->next = t->next->next;
  } else
    fr->next = t->next; // 和下一块不紧邻 指针指到下一块
  if(ct + t->size == (char*)fr){ // 和上一块紧邻 合并
    t->size += (fr->size + sizeof(struct node*));
  } else
    t->next = fr; // 和上一块不紧邻 上一块的指针重定向到新回收的空闲块
}