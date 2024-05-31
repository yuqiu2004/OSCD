#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

/**
 * 释放之前分配的内存
 * 传入要释放的内存块的指针
*/
void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1; // ap是指向数据部分的 转换为header指针同时减去1个header移动到指向header结构的位置
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr) // 寻找到需要合并的空闲块
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr)) // 遍历到链表尾部了也中止遍历
      break;
  if(bp + bp->s.size == p->s.ptr){ // 空闲块和下一块紧邻 合并
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr; // 和下一块不紧邻 指针指到下一块
  if(p + p->s.size == bp){ // 和上一块紧邻 合并
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp; // 和上一块不紧邻 上一块的指针重定向到新回收的空闲块
  freep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

/**
 * 用户动态内存分配实现
 * 参数：请求字节数
 */
void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;//新的总大小

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
