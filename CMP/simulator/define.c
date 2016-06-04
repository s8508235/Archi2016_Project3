#include "define.h"
void initI(int IMemsize,int IMemoryPageSize,int TotalSizeOfICache,int BlockSizeOfICache ,int SetAssOfICache)
{
    int i,j;
    Iinfo.MEM_size = IMemsize;
    Iinfo.page_size = IMemoryPageSize;
    Iinfo.CA_size = TotalSizeOfICache;
    Iinfo.Block_size = BlockSizeOfICache;
    Iinfo.CA_associate = SetAssOfICache;

    Iinfo.PTE_entries = 1024/Iinfo.page_size;
    Iinfo.TLB_entries = Iinfo.PTE_entries/4;
    Iinfo.MEM_entries = Iinfo.MEM_size / Iinfo.page_size;
    Iinfo.CA_entries = Iinfo.CA_size / Iinfo.CA_associate / Iinfo.Block_size;

    ITLB = malloc(Iinfo.TLB_entries * sizeof(TLB));
    IPTE = malloc(Iinfo.PTE_entries * sizeof(PTE));
    Imemory = malloc(Iinfo.MEM_entries * sizeof(Memory));
    ICache = malloc(Iinfo.CA_entries * sizeof(Cache*));
    for(i=0; i<Iinfo.PTE_entries; i++)
    {
        IPTE[i].ppn=0;
        IPTE[i].valid=0;
    }
    for(i=0; i<Iinfo.TLB_entries; i++)
    {
        ITLB[i].last_used=-1;
        ITLB[i].ppn=0;
        ITLB[i].vpn=0;
    }
    for(i=0; i<Iinfo.MEM_entries; i++)
    {
        Imemory[i].last_used=-1;
        Imemory[i].valid=0;
    }
    for(i=0; i<Iinfo.CA_entries; i++)
    {
        ICache[i] = malloc(Iinfo.CA_associate * sizeof(Cache));
    }
    for(i=0; i<Iinfo.CA_entries; i++)
    {
        for(j=0; j<Iinfo.CA_associate; j++)
        {
            ICache[i][j].MRU=0;
            ICache[i][j].tag=0;
            ICache[i][j].valid=0;
        }
    }

    Iresult.TLBhit=0;
    Iresult.TLBmiss=0;
    Iresult.PTEhit=0;
    Iresult.PTEmiss=0;
}
void initD(int DMemorySize,int DMemoryPageSize,int TotalSizeOfDCache,int BlockSizeOfDCache,int SetAssOfDCache)
{

    int i,j;
    Dinfo.MEM_size = DMemorySize;
    Dinfo.page_size = DMemoryPageSize;
    Dinfo.CA_size = TotalSizeOfDCache;
    Dinfo.Block_size = BlockSizeOfDCache;
    Dinfo.CA_associate = SetAssOfDCache;

    Dinfo.PTE_entries = 1024/Dinfo.page_size;
    Dinfo.TLB_entries = Dinfo.PTE_entries/4;
    Dinfo.MEM_entries = Dinfo.MEM_size / Dinfo.page_size;
    Dinfo.CA_entries = Dinfo.CA_size / Dinfo.CA_associate / Dinfo.Block_size;

    DTLB = malloc(Dinfo.TLB_entries * sizeof(TLB));
    DPTE = malloc(Dinfo.PTE_entries * sizeof(PTE));
    Dmemory = malloc(Dinfo.MEM_entries * sizeof(Memory));
    DCache = malloc(Dinfo.CA_entries * sizeof(Cache*));
    for(i=0; i<Dinfo.PTE_entries; i++)
    {
        DPTE[i].ppn=0;
        DPTE[i].valid=0;
    }
    for(i=0; i<Dinfo.TLB_entries; i++)
    {
        DTLB[i].last_used=-1;
        DTLB[i].ppn=0;
        DTLB[i].vpn=0;
    }
    for(i=0; i<Dinfo.MEM_entries; i++)
    {
        Dmemory[i].last_used=-1;
        Dmemory[i].valid=0;
    }
    for(i=0; i<Dinfo.CA_entries; i++)
    {
        DCache[i] = malloc(Dinfo.CA_associate * sizeof(Cache));
    }
    for(i=0; i<Dinfo.CA_entries; i++)
    {
        for(j=0; j<Dinfo.CA_associate; j++)
        {
            DCache[i][j].MRU=0;
            DCache[i][j].tag=0;
            DCache[i][j].valid=0;
        }
    }

    Dresult.TLBhit=0;
    Dresult.TLBmiss=0;
    Dresult.PTEhit=0;
    Dresult.PTEmiss=0;
}
void MRU_detect_change_I(int idx)
{
    int i,flag = 0;
    for(i=0; i<Iinfo.CA_associate; i++)
    {
        if(ICache[idx][i].MRU == 0)
        {
            flag = 1;
            break;
        }
    }
    if(flag == 0)
    {
        for(i=0; i<Iinfo.CA_associate; i++)
        {
            ICache[idx][i].MRU = 0;
        }
    }
}
void MRU_detect_change_D(int idx)
{
    int i,flag = 0;
    for(i=0; i<Dinfo.CA_associate; i++)
    {
        if(DCache[idx][i].MRU == 0)
        {
            flag = 1;
            break;
        }
    }
    if(flag == 0)
    {
        for(i=0; i<Dinfo.CA_associate; i++)
        {
            DCache[idx][i].MRU = 0;
        }
    }
}

void update_ICache(int ppn)
{
    int i;
    int physical_address = ppn * Iinfo.page_size + Iinfo.PageOffset;
    int idx = physical_address / Iinfo.Block_size % Iinfo.CA_entries;
    int tag = physical_address / Iinfo.Block_size / Iinfo.CA_entries;
    int change_idx = 0,flag = 0;
    for(i=0; i<Iinfo.CA_associate; i++)
    {
        if(ICache[idx][i].valid == 0)
        {
            change_idx = i;
            flag = 1;
            break;
        }
    }
    if(flag == 1)
    {
        ICache[idx][change_idx].MRU = 1;
        MRU_detect_change_I(idx);
        ICache[idx][change_idx].MRU = 1;
        ICache[idx][change_idx].valid = 1;
        ICache[idx][change_idx].tag = tag;
    }
    else
    {
        for(i=0; i<Iinfo.CA_associate; i++)
        {
            if(ICache[idx][i].MRU == 0)
            {
                change_idx = i;
                break;
            }
        }
        ICache[idx][change_idx].MRU = 1;
        MRU_detect_change_I(idx);
        ICache[idx][change_idx].MRU = 1;
        ICache[idx][change_idx].valid = 1;
        ICache[idx][change_idx].tag = tag;
    }
}
void update_ITLB(int vpn)
{
    int ppn = IPTE[vpn].ppn;
    int i,min = -1;

    for(i=0; i<Iinfo.TLB_entries; i++)
    {
        if(ITLB[i].last_used !=-1)
        {
            if(ITLB[i].last_used < ITLB[min].last_used ||min == -1)
            {
                min = i;
            }
        }
        else
        {
            min = i;
            break;
        }
    }
    ITLB[min].last_used=cycle;
    ITLB[min].ppn=ppn;
    ITLB[min].vpn=vpn;
}
void update_IPTE(int vpn)
{
    //need to swap first
    int i,j,min = -1;
    int ppn = 0;
    for(i=0; i<Iinfo.MEM_entries; i++)
    {
        if(Imemory[i].valid == 0)
        {
            IPTE[vpn].valid = 1;
            IPTE[vpn].ppn = i;
            Imemory[i].last_used = cycle;
            Imemory[i].valid = 1;
            update_ITLB(vpn);
            Iinfo.ppn = i;
            return ;
        }
        else if(min == -1 || Imemory[i].last_used <min)
        {
            min = Imemory[i].last_used;
            ppn = i;
        }
    }
    Imemory[ppn].last_used = cycle;
   // Imemory[ppn].valid = 1;
    for(i=0; i<Iinfo.TLB_entries; i++)
    {
        if(ITLB[i].ppn == ppn)
        {
            ITLB[i].last_used = -1;
        }
    }

    for(i=0; i<Iinfo.PTE_entries; i++)
    {
        if(IPTE[i].ppn == ppn)
        {
            IPTE[i].valid = 0;
        }
    }

    for(i=0; i<Iinfo.page_size; i++)
    {
        int physical_address = ppn * Iinfo.page_size + i;
        int idx = physical_address / Iinfo.Block_size % Iinfo.CA_entries;
        int tag = physical_address / Iinfo.Block_size / Iinfo.CA_entries;
        if(Iinfo.CA_associate == 1)
        {
            if(ICache[idx][0].tag == tag)
                ICache[idx][0].valid = 0;
        }
        else
        {
            for(j=0; j<Iinfo.CA_associate; j++)
                if(ICache[idx][j].tag ==tag && ICache[idx][j].valid ==1)
                    ICache[idx][j].valid = 0,ICache[idx][j].MRU = 0;
        }

    }

    IPTE[vpn].ppn = ppn;
    IPTE[vpn].valid =  1;
    update_ITLB(vpn);

    Iinfo.ppn =  IPTE[vpn].ppn;
}
void finding_in_ICA(int ppn)
{
    int i;
    int physical_address = ppn * Iinfo.page_size + Iinfo.PageOffset;
    int idx = physical_address / Iinfo.Block_size % Iinfo.CA_entries;
    int tag = physical_address / Iinfo.Block_size / Iinfo.CA_entries;
//2a
    for(i=0; i<Iinfo.CA_associate; i++)
    {
        if(tag == ICache[idx][i].tag && 1 == ICache[idx][i].valid )
        {
            Iresult.CAhit++;
            ICache[idx][i].MRU = 1;
            MRU_detect_change_I(idx);
            ICache[idx][i].MRU = 1;
            return ;
        }
    }
    Iresult.CAmiss ++;
    if(Iinfo.CA_associate == 1)
    {
        ICache[idx][0].tag = tag;
        ICache[idx][0].MRU = 1;
        ICache[idx][0].valid = 1;
        return ;
    }
    else
    {
        update_ICache(ppn);
        //2b
    }

}
void finding_in_IPTE(int vpn)
{
    if(IPTE[vpn].valid ==1 )
    {
        //tlb miss and pte hit 2c hit
        Iresult.PTEhit++;
        update_ITLB(vpn);
        Imemory[IPTE[vpn].ppn].last_used = cycle;
        Iinfo.ppn = IPTE[vpn].ppn;
    }
    else
        //tlb miss and pte miss 2d miss
    {
        Iresult.PTEmiss++;
        //Imemory[IPTE[vpn].ppn].last_used = cycle;
        update_IPTE(vpn);
    }
}
void finding_in_ITLB(int vpn)
{
    //1a
    int i ;
    for(i=0; i<Iinfo.TLB_entries; i++)
    {
        if(ITLB[i].vpn == vpn && ITLB[i].last_used != -1)
        {
            Iresult.TLBhit++;
            ITLB[i].last_used = cycle;
            Iinfo.ppn = ITLB[i].ppn;//1a hit
            return ;
        }
    }
    //1b
    Iresult.TLBmiss ++;
    finding_in_IPTE(vpn);
}
void checkI(int VA,int cnt)
{
    cycle = cnt;
    int vpn = VA/Iinfo.page_size;
    //  printf("current:%8d\n",vpn);
    Iinfo.PageOffset = VA % Iinfo.page_size;
    finding_in_ITLB(vpn);
    //  printITLB();
    //  printIPTE();
   //printf("ass%d\n",Iinfo.CA_associate);
    finding_in_ICA(Iinfo.ppn);
   //printICache(Iinfo.ppn);
//   printIMem();
}


void update_DCache(int ppn)
{
    int i;
    int physical_address = ppn * Dinfo.page_size + Dinfo.PageOffset;
    int idx = physical_address / Dinfo.Block_size % Dinfo.CA_entries;
    int tag = physical_address / Dinfo.Block_size / Dinfo.CA_entries;
    int change_idx = 0,flag = 0;
    for(i=0; i<Dinfo.CA_associate; i++)
    {
        if(DCache[idx][i].valid == 0)
        {
            change_idx = i;
            flag = 1;
            break;
        }
    }
    if(flag == 1)
    {
        DCache[idx][change_idx].MRU = 1;
        MRU_detect_change_D(idx);
        DCache[idx][change_idx].MRU = 1;
        DCache[idx][change_idx].valid = 1;
        DCache[idx][change_idx].tag = tag;
    }
    else
    {
        for(i=0; i<Dinfo.CA_associate; i++)
        {
            if(DCache[idx][i].MRU == 0)
            {
                change_idx = i;
                break;
            }
        }
        DCache[idx][change_idx].MRU = 1;
        MRU_detect_change_D(idx);
        DCache[idx][change_idx].MRU = 1;
        DCache[idx][change_idx].valid = 1;
        DCache[idx][change_idx].tag = tag;
    }
}
void update_DTLB(int vpn)
{
    int ppn = DPTE[vpn].ppn;
    int i,tmp = 0,min = 2147483647;

    for(i=0; i<Dinfo.TLB_entries; i++)
    {
        if(DTLB[i].last_used != -1)
        {
            if(DTLB[i].last_used < min )
            {
                min = DTLB[i].last_used;
                tmp = i;
            }
        }
        else
        {
            tmp = i;
            break;
        }
    }
    DTLB[tmp].last_used=cycle;
    DTLB[tmp].ppn=ppn;
    DTLB[tmp].vpn=vpn;
}
void update_DPTE(int vpn)
{
    //need to swap first
    int i,j,min = -1;
    int ppn = 0;
    for(i=0; i<Dinfo.MEM_entries; i++)
    {
        if(Dmemory[i].valid == 0)
        {
            ppn = i;
            DPTE[vpn].valid = 1;
            DPTE[vpn].ppn = i;
            Dmemory[i].last_used = cycle;
            Dmemory[i].valid = 1;
            update_DTLB(vpn);
            Dinfo.ppn = i;
            return ;
        }
        else if(min == -1 || Dmemory[i].last_used <min)
        {
            min = Dmemory[i].last_used;
            ppn = i;
        }
    }
    Dmemory[ppn].last_used = cycle;
    Dmemory[ppn].valid = 1;
    for(i=0; i<Dinfo.TLB_entries; i++)
    {
        if(DTLB[i].ppn == ppn)
        {
            DTLB[i].last_used = -1;
        }
    }

    for(i=0; i<Dinfo.PTE_entries; i++)
    {
        if(DPTE[i].ppn == ppn)
        {
            DPTE[i].valid = 0;
        }
    }

    for(i=0; i<Dinfo.page_size; i++)
    {
        int physical_address = ppn * Dinfo.page_size + i;
        int idx = physical_address / Dinfo.Block_size % Dinfo.CA_entries;
        int tag = physical_address / Dinfo.Block_size / Dinfo.CA_entries;
        if(Dinfo.CA_associate == 1)
        {
            if(DCache[idx][0].tag == tag)
                DCache[idx][0].valid = 0;
        }
        else
        {
            for(j=0; j<Dinfo.CA_associate; j++)
                if(DCache[idx][j].tag ==tag && DCache[idx][j].valid ==1)
                    DCache[idx][j].valid = 0,DCache[idx][j].MRU = 0;
        }

    }

    DPTE[vpn].ppn = ppn;
    DPTE[vpn].valid =  1;
    update_DTLB(vpn);

    Dinfo.ppn =  DPTE[vpn].ppn;
}
void finding_in_DCA(int ppn)
{
    int i;
    int physical_address = ppn * Dinfo.page_size + Dinfo.PageOffset;
    int idx = physical_address / Dinfo.Block_size % Dinfo.CA_entries;
    int tag = physical_address / Dinfo.Block_size / Dinfo.CA_entries;
//2a
    for(i=0; i<Dinfo.CA_associate; i++)
    {
        if(tag == DCache[idx][i].tag && 1 == DCache[idx][i].valid )
        {
            Dresult.CAhit++;
            DCache[idx][i].MRU = 1;
            MRU_detect_change_D(idx);
            DCache[idx][i].MRU = 1;
            return ;
        }
    }
    Dresult.CAmiss ++;
    if(Dinfo.CA_associate == 1)
    {
        DCache[idx][0].tag = tag;
        DCache[idx][0].MRU = 1;
        DCache[idx][0].valid = 1;
        return ;
    }
    else
    {
        update_DCache(ppn);
        //2b
    }

}
void finding_in_DPTE(int vpn)
{
    if(DPTE[vpn].valid ==1 )
    {
        //tlb miss and pte hit 2c hit
        Dresult.PTEhit++;
        update_DTLB(vpn);
        Dmemory[DPTE[vpn].ppn].last_used = cycle;
        Dinfo.ppn = DPTE[vpn].ppn;
    }
    else
        //tlb miss and pte miss 2d miss
    {
        Dresult.PTEmiss++;
        //Dmemory[DPTE[vpn].ppn].last_used = cycle;
        update_DPTE(vpn);
    }
}
void finding_in_DTLB(int vpn)
{
    //1a
    int i ;
    for(i=0; i<Dinfo.TLB_entries; i++)
    {
        if(DTLB[i].vpn == vpn && DTLB[i].last_used != -1)
        {
            Dresult.TLBhit++;
            DTLB[i].last_used = cycle;
            Dinfo.ppn = DTLB[i].ppn;//1a hit
            return ;
        }
    }
    //1b
    Dresult.TLBmiss ++;
    finding_in_DPTE(vpn);
}
void checkD(int VA,int cnt)
{
    cycle = cnt;
    int vpn = VA/Dinfo.page_size;
    //  printf("current:%8d\n",vpn);
    Dinfo.PageOffset = VA % Dinfo.page_size;
    finding_in_DTLB(vpn);
    //  printTLB();
    //  printPTE();
   //printf("ass%d\n",Dinfo.CA_associate);
    finding_in_DCA(Dinfo.ppn);
   //printCache(Dinfo.ppn);
//   printMem();
}

void printDTLB()
{
    printf("TLB\n");
    int i;
    for(i=0; i<Dinfo.TLB_entries; i++)
    {
        printf("ppn:%4d vpn:%4d last:%4d\n",DTLB[i].ppn,DTLB[i].vpn,DTLB[i].last_used);
    }
}
void printDPTE()
{
    printf("PTE\n");
    int i;
    for(i=0; i<Dinfo.PTE_entries; i++)
    {
        printf("ppn:%4d valid:%4d\n",DPTE[i].ppn,DPTE[i].valid);
    }
}
void printDCache(int ppn)
{
    printf("Cache:%d\n",ppn);
    int i;
    int physical_address = ppn * Dinfo.page_size + Dinfo.PageOffset;
    int idx = physical_address / Dinfo.Block_size % Dinfo.CA_entries;
    for(i=0; i<Dinfo.CA_associate; i++)
        printf("tag:%4d MRU:%4d valid:%4d   \n",DCache[idx][i].tag,DCache[idx][i].MRU,DCache[idx][i].valid);
}
void printDMem()
{
    printf("MEM\n");
    int i;
    for(i=0; i<Dinfo.MEM_entries; i++)
    {
        printf("last:%4d valid:%4d\n",Dmemory[i].last_used,Dmemory[i].valid);
    }
}

void printITLB()
{
    printf("TLB\n");
    int i;
    for(i=0; i<Iinfo.TLB_entries; i++)
    {
        printf("ppn:%4d vpn:%4d last:%4d \n",ITLB[i].ppn,ITLB[i].vpn,ITLB[i].last_used);
    }
}
void printIPTE()
{
    printf("PTE\n");
    int i;
    for(i=0; i<Iinfo.PTE_entries; i++)
    {
        printf("ppn:%4d valid:%4d\n",IPTE[i].ppn,IPTE[i].valid);
    }
}
void printICache(int ppn)
{
    printf("Cache:%d\n",ppn);
    int i;
    int physical_address = ppn * Iinfo.page_size + Iinfo.PageOffset;
    int idx = physical_address / Iinfo.Block_size % Iinfo.CA_entries;
    for(i=0; i<Iinfo.CA_associate; i++)
        printf("tag:%4d MRU:%4d valid:%4d   \n",ICache[idx][i].tag,ICache[idx][i].MRU,ICache[idx][i].valid);
}
void printIMem()
{
    printf("MEM\n");
    int i;
    for(i=0; i<Iinfo.MEM_entries; i++)
    {
        printf("last:%4d valid:%4d\n",Imemory[i].last_used,Imemory[i].valid);
    }
}
