/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Auto generated file 
 */
#include <stdio.h>
#include <drivers/soc.h>
#include <kernel/dpl/AddrTranslateP.h>
#include "ti_dpl_config.h"
#include "ti_drivers_config.h"
#if defined(OS_SAFERTOS)
#include "SafeRTOS_API.h"

extern const xPORT_INIT_PARAMETERS xPortInit;

portBaseType Dpl_kernelInit(void)
{
    portBaseType xInitSchedResult;

    /* Initialise the kernel by passing in a pointer to the xPortInit structure
     * and return the resulting error code. */
    xInitSchedResult = xTaskInitializeScheduler( &xPortInit );

    if( pdPASS == xInitSchedResult )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( acTimerCommandQueueBuffer, "Timer Command Q" );
#endif
    }

    return xInitSchedResult;
}
#endif

/* ----------- HwiP ----------- */
HwiP_Config gHwiConfig = {
    .intcBaseAddr = 0x7FF0000u,
};

/* ----------- ClockP ----------- */
#define MCU_TIMER0_CLOCK_SRC_MUX_ADDR (0x4508060u)
#define MCU_TIMER0_CLOCK_SRC_HFOSC0_CLKOUT (0x0u)
#define MCU_TIMER0_BASE_ADDR     (0x4800000u)

ClockP_Config gClockConfig = {
    .timerBaseAddr = MCU_TIMER0_BASE_ADDR,
    .timerHwiIntNum = 28,
    .timerInputClkHz = 50000000,
    .timerInputPreScaler = 1,
    .usecPerTick = 100,
};

/* ----------- DebugP ----------- */
void putchar_(char character)
{
    /* Output to memory trace buffer */
    DebugP_memLogWriterPutChar(character);
}

/* DebugP log buffer memory and size
 * - This log can be viewed via ROV in CCS
 * - When linux is enabled, this log can also be viewed via linux debugfs
 */
char gDebugMemLog[DebugP_MEM_LOG_SIZE] __attribute__ ((section (".bss.debug_mem_trace_buf"), aligned (128)));
uint32_t gDebugMemLogSize = DebugP_MEM_LOG_SIZE;


/* ----------- CacheP ----------- */
CacheP_Config gCacheConfig = {
    .enable = 1,
    .enableForceWrThru = 0,
};

/* ----------- MpuP_armv7 ----------- */
#define CONFIG_MPU_NUM_REGIONS  (10u)

MpuP_Config gMpuConfig = {
    .numRegions = CONFIG_MPU_NUM_REGIONS,
    .enableBackgroundRegion = 0,
    .enableMpu = 1,
};

MpuP_RegionConfig gMpuRegionConfig[CONFIG_MPU_NUM_REGIONS] =
{
    {
        .baseAddr = 0x0u,
        .size = MpuP_RegionSize_2G,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 0,
            .isBufferable = 0,
            .isSharable = 1,
            .isExecuteNever = 1,
            .tex = 0,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0x0u,
        .size = MpuP_RegionSize_32K,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 0,
            .isBufferable = 0,
            .isSharable = 1,
            .isExecuteNever = 0,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0x41010000u,
        .size = MpuP_RegionSize_32K,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 0,
            .isBufferable = 0,
            .isSharable = 1,
            .isExecuteNever = 0,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0x79100000u,
        .size = MpuP_RegionSize_512K,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 1,
            .isBufferable = 1,
            .isSharable = 0,
            .isExecuteNever = 0,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0x80000000u,
        .size = MpuP_RegionSize_2G,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 1,
            .isBufferable = 1,
            .isSharable = 0,
            .isExecuteNever = 0,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0x60000000u,
        .size = MpuP_RegionSize_64M,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 1,
            .isBufferable = 1,
            .isSharable = 0,
            .isExecuteNever = 0,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0xA5000000u,
        .size = MpuP_RegionSize_16M,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 0,
            .isBufferable = 0,
            .isSharable = 1,
            .isExecuteNever = 1,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0xA1000000u,
        .size = MpuP_RegionSize_1M,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 0,
            .isBufferable = 0,
            .isSharable = 1,
            .isExecuteNever = 1,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0xA1100000u,
        .size = MpuP_RegionSize_1M,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 0,
            .isBufferable = 0,
            .isSharable = 1,
            .isExecuteNever = 1,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
    {
        .baseAddr = 0xA1100000u,
        .size = MpuP_RegionSize_1M,
        .attrs = {
            .isEnable = 1,
            .isCacheable = 0,
            .isBufferable = 0,
            .isSharable = 1,
            .isExecuteNever = 0,
            .tex = 1,
            .accessPerm = MpuP_AP_ALL_RW,
            .subregionDisableMask = 0x0u
        },
    },
};


#define BOOT_SECTION __attribute__((section(".text.boot")))

/* This function is called by _c_int00 */
void BOOT_SECTION __mpu_init()
{
    CacheP_init();
    MpuP_init();
}

void Dpl_init(void)
{
#if defined (OS_SAFERTOS)
    portBaseType xInitSchedResult;
#endif

    /* initialize Hwi but keep interrupts disabled */
    HwiP_init();

    /* init debug log zones early */
    /* Debug log init */
    /* Initialize linux trace log writer */
    DebugP_memLogWriterInit(CSL_CORE_ID_MCU_R5FSS0_0);


    /* set timer clock source */
    SOC_controlModuleUnlockMMR(SOC_DOMAIN_ID_MCU, 2);
    *(volatile uint32_t*)(MCU_TIMER0_CLOCK_SRC_MUX_ADDR) = MCU_TIMER0_CLOCK_SRC_HFOSC0_CLKOUT;
    SOC_controlModuleLockMMR(SOC_DOMAIN_ID_MCU, 2);
    /* initialize Clock */
    ClockP_init();


    /* Enable interrupt handling */
    HwiP_enable();

#if defined(OS_SAFERTOS)
    xInitSchedResult = Dpl_kernelInit();
    DebugP_assertNoLog(xInitSchedResult == pdPASS);
#endif
}

void Dpl_deinit(void)
{
    /* de-initialize Clock */
    ClockP_deinit();
    /* Disable interrupt handling */
    HwiP_disable();
}
