#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// R5 Shared Memory:
// We have 32K (=0x8000) of BTCM ram. 
// Code running in Zephyr usess up to about address 0x069f8 (see below)
// To be safe, we use the last 4k of BTCM: 0x7000 - 0x8000.
/*
    $ readelf -l ./build/zephyr/zephyr.elf
    ...
    Program Headers:
    Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
    EXIDX          0x003a88 0x000039b0 0x000039b0 0x00008 0x00008 R   0x4
    LOAD           0x0000d8 0x00000000 0x00000000 0x0510c 0x069f8 RWE 0x8
    LOAD           0x0051e4 0x000069f8 0x000069f8 0x00070 0x00070 RW  0x4
    LOAD           0x005254 0xa1100000 0xa1100000 0x00010 0x00010 RW  0x4
    TLS            0x004b88 0x00004ab0 0x00004ab0 0x00000 0x00004 R   0x4

*/
#define MEM_START_OFFSET 0x7000

#define LED0 MEM_START_OFFSET
#define LED1 (MEM_START_OFFSET + sizeof(uint32_t))
#define LED2 (LED1 + sizeof(uint32_t))
#define LED3 (LED2 + sizeof(uint32_t))
#define LED4 (LED3 + sizeof(uint32_t))
#define LED5 (LED4 + sizeof(uint32_t))
#define LED6 (LED5 + sizeof(uint32_t))
#define LED7 (LED6 + sizeof(uint32_t))
#define END_MEMORY_OFFSET (LED7 + sizeof(uint32_t))

#define GREEN 0x01000000
#define RED 0x00010000
#define BLUE 0x00000100
#define WHITE 0x00000001
#define YELLOW 0x01010000
#define PURPLE 0x00010100
#define TEAL 0x01000100

#define GREEN_BRIGHT 0x08000000
#define RED_BRIGHT 0x00080000
#define BLUE_BRIGHT 0x00000800
#define WHITE_BRIGHT 0x00000004
#define OFF_LED 0x00000000

// Try these; they are birght! 
// (You'll need to comment out some of the above)
// 0xff000000, // Green Bright
// 0x00ff0000, // Red Bright
// 0x0000ff00, // Blue Bright
// 0xffffff00, // White
// 0xff0000ff, // Green Bright w/ Bright White
// 0x00ff00ff, // Red Bright w/ Bright White
// 0x0000ffff, // Blue Bright w/ Bright White
// 0xffffffff, // White w/ Bright White

static inline uint8_t getSharedMem_uint8(volatile void *base, uint32_t byte_offset) {
    volatile uint8_t *addr_tmp = (uint8_t *)base + byte_offset ;
    volatile uint8_t val = *addr_tmp;
    return val;
}

static inline void setSharedMem_uint8(volatile void* base, uint32_t byte_offset, uint8_t val) {
    volatile uint8_t *addr_tmp = (uint8_t *)base + byte_offset;
    volatile uint8_t val_tmp = val;
    // printf("        --> Set8  addr 0x%08x to %x\n", (uint32_t *)addr_tmp, val_tmp);
    *addr_tmp = val_tmp;
}

static inline uint32_t getSharedMem_uint32(volatile void *base, uint32_t byte_offset) {
    volatile uint32_t *addr_tmp = (uint32_t *) ((uint8_t *)base + byte_offset);
    volatile uint32_t val = *addr_tmp;
    return val;
}

static inline void setSharedMem_uint32(volatile void* base, uint32_t byte_offset, uint32_t val) {
    volatile uint32_t *addr_tmp = (uint32_t *) ((uint8_t *)base + byte_offset);
    volatile uint32_t val_tmp = val;
    // printf("        --> Set32 addr 0x%08x to %x\n", (uint32_t *)addr_tmp, val_tmp);
    *addr_tmp = val_tmp;
}

void shared_memory_init(void);
void shared_memory_cleanup(void);

void hit_animation(void);
void miss_animation(void);
void clear_animation(void);
void set_cut_animation(void);
void clear_cut_animation(void);

#ifdef __cplusplus
}
#endif

#endif