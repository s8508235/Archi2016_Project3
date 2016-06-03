#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED
#include "stdlib.h"
typedef struct TLB
{
    unsigned vpn;
    unsigned ppn;
    unsigned valid;
    unsigned last_used;
}TLB;
TLB *ITLB,*DTLB;
typedef struct Cache
{
    unsigned tag;
    unsigned valid;
    unsigned MRU;
}Cache;
Cache **ICache,**DCache;
typedef struct PTE
{
    unsigned valid;
    unsigned ppn;
}PTE;
PTE *IPTE,*DPTE;
typedef struct Memory
{
    unsigned last_used;
    unsigned valid;
}Memory;
Memory *Imemory,*Dmemory;
typedef struct info
{
    int vpn;
    int ppn;
    int page_size;
    int PTE_entries;
    int TLB_entries;
    int CA_size;
    int CA_associate;
    int CA_entries;
    int Block_size;
    int MEM_size;
    int MEM_entries;
    int PageOffset;
    int cache_find;
}info;
info Iinfo,Dinfo;

typedef struct result
{
    int TLBmiss,TLBhit;
    int PTEmiss,PTEhit;
    int CAmiss,CAhit;
}result;
extern result Iresult,Dresult;
result Iresult,Dresult;

void initI(int IMemsize,int IMemoryPageSize,int TotalSizeOfICache,int BlockSizeOfICache ,int SetAssOfICache);
void initD(int DMemorySize,int DMemoryPageSize,int TotalSizeOfDCache,int BlockSizeOfDCache,int SetAssOfDCache);
void checkI(int VA,int cnt);
void checkD(int VA,int cnt);
int cycle;
#endif // DEFINE_H_INCLUDED
