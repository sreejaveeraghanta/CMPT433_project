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

#include "ti_drivers_config.h"
#include <drivers/sciclient.h>

    /*
 * IPC Notify
 */
#include <drivers/ipc_notify.h>
#include <drivers/ipc_notify/v0/ipc_notify_v0.h>

/* this function is called within IpcNotify_init, this function returns core specific IPC config */
void IpcNotify_getConfig(IpcNotify_InterruptConfig **interruptConfig, uint32_t *interruptConfigNum)
{
    /* extern globals that are specific to this core */
    extern IpcNotify_InterruptConfig gIpcNotifyInterruptConfig_mcu_r5fss0_0[];
    extern uint32_t gIpcNotifyInterruptConfigNum_mcu_r5fss0_0;

    *interruptConfig = &gIpcNotifyInterruptConfig_mcu_r5fss0_0[0];
    *interruptConfigNum = gIpcNotifyInterruptConfigNum_mcu_r5fss0_0;
}

/*
 * IPC RP Message
 */
#include <drivers/ipc_rpmsg.h>

/* Number of CPUs that are enabled for IPC RPMessage */
#define IPC_RPMESSAGE_NUM_CORES           (6U)
/* Number of VRINGs for the numner of CPUs that are enabled for IPC */
#define IPC_RPMESSAGE_NUM_VRINGS          (IPC_RPMESSAGE_NUM_CORES*(IPC_RPMESSAGE_NUM_CORES-1))
/* Number of a buffers in a VRING, i.e depth of VRING queue */
#define IPC_RPMESSAGE_NUM_VRING_BUF       (256U)
/* Max size of a buffer in a VRING */
#define IPC_RPMESSAGE_MAX_VRING_BUF_SIZE  (512U)

/* Size of each VRING is
 *     2 x number of buffers x size of each buffer
 */
#define IPC_RPMESSAGE_VRING_SIZE          RPMESSAGE_VRING_SIZE_PDK(IPC_RPMESSAGE_NUM_VRING_BUF, IPC_RPMESSAGE_MAX_VRING_BUF_SIZE)


/* VRING base address, all VRINGs are put one after other in the below region.
 *
 * IMPORTANT: Make sure of below,
 * - The section defined below should be placed at the exact same location in memory for all the CPUs
 * - The memory should be marked as non-cached for all the CPUs
 * - The section should be marked as NOLOAD in all the CPUs linker command file
 */
/* In this case gRPMessageVringMem size is 7864320 bytes */
uint8_t gRPMessageVringMem[IPC_RPMESSAGE_NUM_VRINGS][IPC_RPMESSAGE_VRING_SIZE] __attribute__((aligned(128), section(".bss.ipc_vring_mem")));


/* Buffer used for trace, address and size of this buffer is put in the resource table so that Linux can read it */
extern char gDebugMemLog[];

const RPMessage_ResourceTable gRPMessage_linuxResourceTable __attribute__ ((section (".resource_table"), aligned (1024))) =
{
    {
        1U,         /* we're the first version that implements this */
        2U,         /* number of entries, MUST be 2 */
        { 0U, 0U, } /* reserved, must be zero */
    },
    /* offsets to the entries */
    {
        offsetof(RPMessage_ResourceTable, vdev),
        offsetof(RPMessage_ResourceTable, trace),
    },
    /* vdev entry */
    {
        RPMESSAGE_RSC_TYPE_VDEV, RPMESSAGE_RSC_VIRTIO_ID_RPMSG,
        0U, 1U, 0U, 0U, 0U, 2U, { 0U, 0U },
    },
    /* the two vrings */
    { RPMESSAGE_RSC_VRING_ADDR_ANY, 4096U, 256U, 1U, 0U },
    { RPMESSAGE_RSC_VRING_ADDR_ANY, 4096U, 256U, 2U, 0U },
    {
        (RPMESSAGE_RSC_TRACE_INTS_VER0 | RPMESSAGE_RSC_TYPE_TRACE),
        (uint32_t)gDebugMemLog, DebugP_MEM_LOG_SIZE,
        0, "trace:mcu_r5fss0_0",
    },
};



    /*
 * MCSPI
 */

/* MCSPI atrributes */
static MCSPI_Attrs gMcspiAttrs[CONFIG_MCSPI_NUM_INSTANCES] =
{
    {
        .baseAddr           = CSL_MCU_MCSPI0_CFG_BASE,
        .inputClkFreq       = 50000000U,
        .intrNum            = 207,
        .operMode           = MCSPI_OPER_MODE_POLLED,
        .intrPriority       = 4U,
        .chMode             = MCSPI_CH_MODE_SINGLE,
        .pinMode            = MCSPI_PINMODE_4PIN,
        .initDelay          = MCSPI_INITDLY_0,
    },
};
/* MCSPI objects - initialized by the driver */
static MCSPI_Object gMcspiObjects[CONFIG_MCSPI_NUM_INSTANCES];
/* MCSPI driver configuration */
MCSPI_Config gMcspiConfig[CONFIG_MCSPI_NUM_INSTANCES] =
{
    {
        &gMcspiAttrs[CONFIG_MCSPI0],
        &gMcspiObjects[CONFIG_MCSPI0],
    },
};

uint32_t gMcspiConfigNum = CONFIG_MCSPI_NUM_INSTANCES;

#include <drivers/mcspi/v0/dma/mcspi_dma.h>
MCSPI_DmaConfig gMcspiDmaConfig =
{
    .fxns        = NULL,
    .mcspiDmaArgs = (void *)NULL,
};


uint32_t gMcspiDmaConfigNum = CONFIG_MCSPI_NUM_DMA_INSTANCES;


void Pinmux_init();
void PowerClock_init(void);
void PowerClock_deinit(void);

/*
 * Common Functions
 */

#if defined(OS_SAFERTOS)
void System_lateInit(void)
{
    /* We should do sciclient init before we enable power and clock to the peripherals */
        /* SCICLIENT init */
    {

        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_init(CSL_CORE_ID_MCU_R5FSS0_0);
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);

    }

    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
            /* IPC Notify */
    {
        IpcNotify_Params notifyParams;
        int32_t status;

        /* initialize parameters to default */
        IpcNotify_Params_init(&notifyParams);

        /* specify the core on which this API is called */
        notifyParams.selfCoreId = CSL_CORE_ID_MCU_R5FSS0_0;

        /* list the cores that will do IPC Notify with this core
        * Make sure to NOT list 'self' core in the list below
        */
        notifyParams.numCores = 6;
        notifyParams.coreIdList[0] = CSL_CORE_ID_WKUP_R5FSS0_0;
        notifyParams.coreIdList[1] = CSL_CORE_ID_MAIN_R5FSS0_0;
        notifyParams.coreIdList[2] = CSL_CORE_ID_C75SS0_0;
        notifyParams.coreIdList[3] = CSL_CORE_ID_C75SS1_0;
        notifyParams.coreIdList[4] = CSL_CORE_ID_A53SS0_0;
        notifyParams.coreIdList[5] = CSL_CORE_ID_A53SS0_0;
        notifyParams.linuxCoreId = CSL_CORE_ID_A53SS0_0;

        /* initialize the IPC Notify module */
        status = IpcNotify_init(&notifyParams);
        DebugP_assert(status==SystemP_SUCCESS);

    }
    /* IPC RPMessage */
    {
        RPMessage_Params rpmsgParams;
        int32_t status;

        /* initialize parameters to default */
        RPMessage_Params_init(&rpmsgParams);

        /* VRING mapping from source core to destination core, '-1' means NO VRING,
            mcu_r5fss0_0 => {"mcu_r5fss0_0":-1,"wkup_r5fss0_0":0,"main_r5fss0_0":1,"c75ss0_0":2,"c75ss1_0":3,"a53ss0_0":4}
            wkup_r5fss0_0 => {"mcu_r5fss0_0":5,"wkup_r5fss0_0":-1,"main_r5fss0_0":6,"c75ss0_0":7,"c75ss1_0":8,"a53ss0_0":9}
            main_r5fss0_0 => {"mcu_r5fss0_0":10,"wkup_r5fss0_0":11,"main_r5fss0_0":-1,"c75ss0_0":12,"c75ss1_0":13,"a53ss0_0":14}
            c75ss0_0 => {"mcu_r5fss0_0":15,"wkup_r5fss0_0":16,"main_r5fss0_0":17,"c75ss0_0":-1,"c75ss1_0":18,"a53ss0_0":19}
            c75ss1_0 => {"mcu_r5fss0_0":20,"wkup_r5fss0_0":21,"main_r5fss0_0":22,"c75ss0_0":23,"c75ss1_0":-1,"a53ss0_0":24}
            a53ss0_0 => {"mcu_r5fss0_0":25,"wkup_r5fss0_0":26,"main_r5fss0_0":27,"c75ss0_0":28,"c75ss1_0":29,"a53ss0_0":-1}
         */
        /* TX VRINGs */
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_WKUP_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[0];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_MAIN_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[1];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_C75SS0_0] = (uintptr_t)gRPMessageVringMem[2];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_C75SS1_0] = (uintptr_t)gRPMessageVringMem[3];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_A53SS0_0] = (uintptr_t)gRPMessageVringMem[4];
        /* RX VRINGs */
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_WKUP_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[5];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_MAIN_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[10];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_C75SS0_0] = (uintptr_t)gRPMessageVringMem[15];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_C75SS1_0] = (uintptr_t)gRPMessageVringMem[20];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_A53SS0_0] = (uintptr_t)gRPMessageVringMem[25];
        /* Other VRING properties */
        rpmsgParams.vringSize = IPC_RPMESSAGE_VRING_SIZE;
        rpmsgParams.vringNumBuf = IPC_RPMESSAGE_NUM_VRING_BUF;
        rpmsgParams.vringMsgSize = IPC_RPMESSAGE_MAX_VRING_BUF_SIZE;
        rpmsgParams.linuxResourceTable = &gRPMessage_linuxResourceTable;
        rpmsgParams.linuxCoreId = CSL_CORE_ID_A53SS0_0;


        /* initialize the IPC RP Message module */
        status = RPMessage_init(&rpmsgParams);
        DebugP_assert(status==SystemP_SUCCESS);
    }

            MCSPI_init();
}
#endif

void System_init(void)
{
    /* DPL init sets up address transalation unit, on some CPUs this is needed
     * to access SCICLIENT services, hence this needs to happen first
     */
    Dpl_init();
#if !defined(OS_SAFERTOS)
    /* We should do sciclient init before we enable power and clock to the peripherals */
        /* SCICLIENT init */
    {

        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_init(CSL_CORE_ID_MCU_R5FSS0_0);
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);

    }

    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
        /* IPC Notify */
    {
        IpcNotify_Params notifyParams;
        int32_t status;

        /* initialize parameters to default */
        IpcNotify_Params_init(&notifyParams);

        /* specify the core on which this API is called */
        notifyParams.selfCoreId = CSL_CORE_ID_MCU_R5FSS0_0;

        /* list the cores that will do IPC Notify with this core
        * Make sure to NOT list 'self' core in the list below
        */
        notifyParams.numCores = 6;
        notifyParams.coreIdList[0] = CSL_CORE_ID_WKUP_R5FSS0_0;
        notifyParams.coreIdList[1] = CSL_CORE_ID_MAIN_R5FSS0_0;
        notifyParams.coreIdList[2] = CSL_CORE_ID_C75SS0_0;
        notifyParams.coreIdList[3] = CSL_CORE_ID_C75SS1_0;
        notifyParams.coreIdList[4] = CSL_CORE_ID_A53SS0_0;
        notifyParams.coreIdList[5] = CSL_CORE_ID_A53SS0_0;
        notifyParams.linuxCoreId = CSL_CORE_ID_A53SS0_0;

        /* initialize the IPC Notify module */
        status = IpcNotify_init(&notifyParams);
        DebugP_assert(status==SystemP_SUCCESS);

    }
    /* IPC RPMessage */
    {
        RPMessage_Params rpmsgParams;
        int32_t status;

        /* initialize parameters to default */
        RPMessage_Params_init(&rpmsgParams);

        /* VRING mapping from source core to destination core, '-1' means NO VRING,
            mcu_r5fss0_0 => {"mcu_r5fss0_0":-1,"wkup_r5fss0_0":0,"main_r5fss0_0":1,"c75ss0_0":2,"c75ss1_0":3,"a53ss0_0":4}
            wkup_r5fss0_0 => {"mcu_r5fss0_0":5,"wkup_r5fss0_0":-1,"main_r5fss0_0":6,"c75ss0_0":7,"c75ss1_0":8,"a53ss0_0":9}
            main_r5fss0_0 => {"mcu_r5fss0_0":10,"wkup_r5fss0_0":11,"main_r5fss0_0":-1,"c75ss0_0":12,"c75ss1_0":13,"a53ss0_0":14}
            c75ss0_0 => {"mcu_r5fss0_0":15,"wkup_r5fss0_0":16,"main_r5fss0_0":17,"c75ss0_0":-1,"c75ss1_0":18,"a53ss0_0":19}
            c75ss1_0 => {"mcu_r5fss0_0":20,"wkup_r5fss0_0":21,"main_r5fss0_0":22,"c75ss0_0":23,"c75ss1_0":-1,"a53ss0_0":24}
            a53ss0_0 => {"mcu_r5fss0_0":25,"wkup_r5fss0_0":26,"main_r5fss0_0":27,"c75ss0_0":28,"c75ss1_0":29,"a53ss0_0":-1}
         */
        /* TX VRINGs */
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_WKUP_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[0];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_MAIN_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[1];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_C75SS0_0] = (uintptr_t)gRPMessageVringMem[2];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_C75SS1_0] = (uintptr_t)gRPMessageVringMem[3];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_A53SS0_0] = (uintptr_t)gRPMessageVringMem[4];
        /* RX VRINGs */
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_WKUP_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[5];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_MAIN_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[10];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_C75SS0_0] = (uintptr_t)gRPMessageVringMem[15];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_C75SS1_0] = (uintptr_t)gRPMessageVringMem[20];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_A53SS0_0] = (uintptr_t)gRPMessageVringMem[25];
        /* Other VRING properties */
        rpmsgParams.vringSize = IPC_RPMESSAGE_VRING_SIZE;
        rpmsgParams.vringNumBuf = IPC_RPMESSAGE_NUM_VRING_BUF;
        rpmsgParams.vringMsgSize = IPC_RPMESSAGE_MAX_VRING_BUF_SIZE;
        rpmsgParams.linuxResourceTable = &gRPMessage_linuxResourceTable;
        rpmsgParams.linuxCoreId = CSL_CORE_ID_A53SS0_0;


        /* initialize the IPC RP Message module */
        status = RPMessage_init(&rpmsgParams);
        DebugP_assert(status==SystemP_SUCCESS);
    }

        MCSPI_init();
#endif
}

void System_deinit(void)
{
    RPMessage_deInit();
    IpcNotify_deInit();

    MCSPI_deinit();
    PowerClock_deinit();
    /* SCICLIENT deinit */
    {
        int32_t         retVal = SystemP_SUCCESS;

        retVal = Sciclient_deinit();
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }
    Dpl_deinit();
}
