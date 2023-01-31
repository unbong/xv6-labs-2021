#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
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


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 buf_addr  = 0;
  if( argaddr(0, &buf_addr) <0 )
      return -1;
  char * buf = (char * ) buf_addr;
  int size =0;
  if( argint(1, &size) <0)
      return  -1;
  if (size > 64 )
  {
      printf("size must less than 64.\n");
      return -1;
  }
  uint64 bitmask =0;
  if( argaddr(2, &bitmask) < 0)
      return -1;

  uint64 mask = 0;

  // walk to find is accessed
  //pagetable_t  pt = myproc()->pagetable;
  char * item ;
    //vmprint(myproc()->pagetable);
  for(int i = 0; i < size; i++)
  {
      pagetable_t  pt = myproc()->pagetable;

      item = buf + i*PGSIZE;

      pte_t * pte;
      for(int level = 2; level >= 0; level--)
      {
          // 找到页表条目
          pte = &pt[PX(level,item )];
          if( (*pte) & PTE_V)
          {
              pt = (pagetable_t )PTE2PA(*pte);
          }
          else
          {
              pte = 0;
              break;
          }
      }

      //printf("pte: %p\n", pte);
      if( (*pte) & PTE_A)
      {
          *pte = (*pte) & ~(PTE_A);
          mask = (mask | 1l << i);
      }

  }

  printf("mask: %p\n", mask);
  copyout(myproc()->pagetable, bitmask, (char *)&mask, sizeof(uint64));
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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
