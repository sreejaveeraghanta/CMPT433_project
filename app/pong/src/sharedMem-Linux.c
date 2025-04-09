#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include <string.h>

#include "sharedDataLayout.h"

// General R5 Memomry Sharing Routine
// ----------------------------------------------------------------
#define ATCM_ADDR     0x79000000  // MCU ATCM (p59 TRM)
#define BTCM_ADDR     0x79020000  // MCU BTCM (p59 TRM)
#define MEM_LENGTH    0x8000

#define ONE_SECOND 1
#define ZERO 0
#define TENTH_SECOND_IN_NS 100000000
#define HALF_SECOND_IN_NS 500000000

void shared_memory_init(void);
void shared_memory_cleanup(void);
volatile void* get_pR5Base(void);

void hit_animation(void);
void miss_animation(void);
void clear_animation(void);
void set_cut_animation(void);
void clear_cut_animation(void);

volatile void *pR5Base;

static bool cut_animation = false;

// 1s between refreshing screen
//static struct timespec reqDelay_1s = {ONE_SECOND, ZERO};
static struct timespec reqDelay_100ms = {ZERO, TENTH_SECOND_IN_NS};

void set_cut_animation(void)
{
    cut_animation = true;
}

void clear_cut_animation(void)
{
    cut_animation = false;
}

// Return the address of the base address of the ATCM memory region for the R5-MCU
volatile void* getR5MmapAddr(void)
{
    // Access /dev/mem to gain access to physical memory (for memory-mapped devices/specialmemory)
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem; Did you run with sudo?");
        exit(EXIT_FAILURE);
    }

    // Inside main memory (fd), access the part at offset BTCM_ADDR:
    // (Get points to start of R5 memory after it's memory mapped)
    volatile void* pR5Base = mmap(0, MEM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BTCM_ADDR);
    if (pR5Base == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pR5Base;
}

void freeR5MmapAddr(volatile void* pR5Base)
{
    if (munmap((void*) pR5Base, MEM_LENGTH)) {
        perror("R5 munmap failed");
        exit(EXIT_FAILURE);
    }
}

void clear_animation(void)
{
    setSharedMem_uint32(pR5Base, LED0, OFF_LED);
	setSharedMem_uint32(pR5Base, LED1, OFF_LED);
	setSharedMem_uint32(pR5Base, LED2, OFF_LED);
	setSharedMem_uint32(pR5Base, LED3, OFF_LED);
	setSharedMem_uint32(pR5Base, LED4, OFF_LED);
	setSharedMem_uint32(pR5Base, LED5, OFF_LED);
	setSharedMem_uint32(pR5Base, LED6, OFF_LED);
	setSharedMem_uint32(pR5Base, LED7, OFF_LED);
}

void hit_animation(void)
{
    setSharedMem_uint32(pR5Base, LED0, RED);
	setSharedMem_uint32(pR5Base, LED1, OFF_LED);
	setSharedMem_uint32(pR5Base, LED2, OFF_LED);
	setSharedMem_uint32(pR5Base, LED3, OFF_LED);
	setSharedMem_uint32(pR5Base, LED4, OFF_LED);
	setSharedMem_uint32(pR5Base, LED5, OFF_LED);
	setSharedMem_uint32(pR5Base, LED6, OFF_LED);
	setSharedMem_uint32(pR5Base, LED7, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

	setSharedMem_uint32(pR5Base, LED1, RED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

	setSharedMem_uint32(pR5Base, LED2, GREEN);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED0, OFF_LED);
    setSharedMem_uint32(pR5Base, LED3, GREEN);
    
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED1, OFF_LED);
    setSharedMem_uint32(pR5Base, LED4, BLUE);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED2, OFF_LED);
    setSharedMem_uint32(pR5Base, LED5, BLUE);
    
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED3, OFF_LED);
    setSharedMem_uint32(pR5Base, LED6, PURPLE);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }
    
    setSharedMem_uint32(pR5Base, LED4, OFF_LED);
    setSharedMem_uint32(pR5Base, LED7, PURPLE);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED0, RED);
	setSharedMem_uint32(pR5Base, LED1, RED);
	setSharedMem_uint32(pR5Base, LED2, RED);
	setSharedMem_uint32(pR5Base, LED3, RED);
	setSharedMem_uint32(pR5Base, LED4, RED);
	setSharedMem_uint32(pR5Base, LED5, RED);
	setSharedMem_uint32(pR5Base, LED6, RED);
	setSharedMem_uint32(pR5Base, LED7, RED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

    clear_animation();

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);

    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED0, RED);
	setSharedMem_uint32(pR5Base, LED1, RED);
	setSharedMem_uint32(pR5Base, LED2, RED);
	setSharedMem_uint32(pR5Base, LED3, RED);
	setSharedMem_uint32(pR5Base, LED4, RED);
	setSharedMem_uint32(pR5Base, LED5, RED);
	setSharedMem_uint32(pR5Base, LED6, RED);
	setSharedMem_uint32(pR5Base, LED7, RED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);


    if(cut_animation){
        return;
    }

    clear_animation();

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
}

void miss_animation(void)
{
    setSharedMem_uint32(pR5Base, LED0, BLUE);
	setSharedMem_uint32(pR5Base, LED1, BLUE);
	setSharedMem_uint32(pR5Base, LED2, BLUE);
	setSharedMem_uint32(pR5Base, LED3, BLUE);
	setSharedMem_uint32(pR5Base, LED4, BLUE);
	setSharedMem_uint32(pR5Base, LED5, BLUE);
	setSharedMem_uint32(pR5Base, LED6, BLUE);
	setSharedMem_uint32(pR5Base, LED7, BLUE);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED7, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED6, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED5, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED4, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED3, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED2, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED1, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED0, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED0, BLUE);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED0, OFF_LED);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }

    setSharedMem_uint32(pR5Base, LED0, BLUE);

    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    if(cut_animation){
        return;
    }
    nanosleep(&reqDelay_100ms, (struct timespec *) NULL);
    setSharedMem_uint32(pR5Base, LED0, OFF_LED);
}

void shared_memory_init(void)
{
    printf("Sharing memory with R5\n");

    // Get access to shared memory for my uses
    pR5Base = getR5MmapAddr();
}

void shared_memory_cleanup(void)
{
    clear_animation();
    // Cleanup
    freeR5MmapAddr(pR5Base);
}