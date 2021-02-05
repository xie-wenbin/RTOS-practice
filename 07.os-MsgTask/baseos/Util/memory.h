#ifndef _MEMORY_H_
#define _MEMORY_H_

 /**
*****************************************************************
* @file: memory.h
* @author: wen  
* @date:  12/25/2020
* @brief:  4k byte ram for memory to malloc or free
* @attention:
****************************************************************
 */
#include "freeRTOS.h"
    
#include "task.h"
#include "DtypeStm32.h"


/* 
    sram 64kb, use 4kb
    memory usage:
    total uart rx/tx 16/16 = 32bytes
           index for rx/tx queue = 2*2 = 4
           total = 32+4 = 36

    total timer * sizeof(TIMER)*2 = 10*2 = 20bytes
    
    total message = 30, 

    left = 512 - 36 -20 - 30 = 426

    dynamic memory alloc 400

    left = 412

    256 256   128 128     32 32 32 32  32 32 32 32      
         512       256         256            
*/


#define MAX_MEM_SIZE 512

#define MAX_MEM_SIZE 512

#define MEMSIZE512_NUM 4 
#define MEMSIZE256_NUM 2 
#define MEMSIZE128_NUM 2 
#define MEMSIZE032_NUM 32

// block size 40
#define MEMORY_BLOCK_NUMER (MEMSIZE512_NUM+MEMSIZE256_NUM+MEMSIZE128_NUM+MEMSIZE032_NUM)

// 4*512 + 2 * 256 + 2 * 128 + 32 * 32 + 40 * 4 = 2048 + 512 + 256 + 1024 + 160 = 4000 = 4k byte
#define MEMORY_BLOCK_SIZE ((MEMSIZE512_NUM*512+MEMSIZE256_NUM*256+MEMSIZE128_NUM*128+MEMSIZE032_NUM*32)+(MEMORY_BLOCK_NUMER*4))


#define MEMORY_INVALID_INDEX 0xFF


void FreeMem(void *mem);
void* AllocMem(UINT16 size);
void InitMem(void);
void MemCheck(void);





#endif

