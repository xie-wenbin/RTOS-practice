#include "memory.h"
#include <string.h>
#include "vos.h"


UINT8   memblock[MEMORY_BLOCK_SIZE]; // 2 BYTES   4Kbyte
UINT8   memIndex[MEMORY_BLOCK_NUMER]; // 8 BYTES  size 40
UINT16  memOffset[MEMORY_BLOCK_NUMER]; // 16 BYTES

UINT16 memStat;

void InitMem(void)
{
    char ucLoop;
    UINT8 ucIdx;
    for (ucLoop= 0 ; ucLoop < MEMORY_BLOCK_NUMER; ucLoop++)
    {
       memIndex[ucLoop]= 0;

    }

    ucIdx = 1;
    memOffset[0] = 4;
    
    for (ucLoop= 0 ; ucLoop < MEMSIZE032_NUM; ucLoop++)
    {
        memOffset[ucIdx] = memOffset[ucIdx-1] + (32+4);
        ucIdx++;
    }

    for (ucLoop = 0; ucLoop < MEMSIZE128_NUM; ucLoop++)
    {
        memOffset[ucIdx] = memOffset[ucIdx-1] + (128+4);
        ucIdx++;
    }

    
    for (ucLoop = 0; ucLoop < MEMSIZE256_NUM; ucLoop++)
    {
        memOffset[ucIdx] = memOffset[ucIdx-1] + (256+4);
        ucIdx++;
    }

    for (ucLoop = 0; ucLoop < MEMSIZE512_NUM-1; ucLoop++)
    {
        memOffset[ucIdx] = memOffset[ucIdx-1] + (512+4);
        ucIdx++;
    }

    memStat = MEMORY_BLOCK_NUMER;

}

void* AllocMem(UINT16 size)
{
    uint32_t     cpu_sr = 0;

    UINT8 ucStartIndex;
    UINT8 ucLoop;
    UINT8 *buf = NULL;
    OS_ENTER_CRITICAL();
    if (size > MAX_MEM_SIZE) 
    {
       OS_EXIT_CRITICAL();
       return NULL;
    }

    if (size <= 32 )
    {   
        ucStartIndex = 0;
    }
    else if (size <= 128)
    {   
        ucStartIndex = MEMSIZE032_NUM;
    }
    else if (size <= 256)
    {
        ucStartIndex = (MEMSIZE032_NUM+MEMSIZE128_NUM);
    }
    else if (size <= 512)
    {
        ucStartIndex = (MEMSIZE032_NUM+MEMSIZE128_NUM+MEMSIZE256_NUM);
    }

    for (ucLoop = ucStartIndex ; ucLoop < MEMORY_BLOCK_NUMER ; ucLoop++)
    {
        if (!memIndex[ucLoop])
        {
            memIndex[ucLoop] = 1;
            memStat--;
            OS_EXIT_CRITICAL();
            buf = &memblock[memOffset[ucLoop]];
            *((int *)(buf-4)) = ucLoop; // mem index 
            return buf;
        }
    }
    OS_EXIT_CRITICAL();
    return NULL;
    
}

void FreeMem(void *mem)
{
    uint32_t     cpu_sr = 0;
    UINT8 MemIndex = MEMORY_BLOCK_NUMER;

    int offset;

    offset = (UINT8 *)mem - memblock;


    if ( offset >= MEMORY_BLOCK_SIZE
        || offset < 0) // memory not in range
    {
        return ;
    }
    MemIndex = *((int *)((char *)mem - 4));

    if (MemIndex >= MEMORY_BLOCK_NUMER) 
        return;
    OS_ENTER_CRITICAL();
    memStat++;
    memIndex[MemIndex] = 0;
    OS_EXIT_CRITICAL();
}

void MemCheck(void)
{
}

