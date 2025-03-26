/*
 *  Copyright (C) 2018-2021 Texas Instruments Incorporated
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

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/SemaphoreP.h>

#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"

#include <drivers/ipc_notify.h>
#include <drivers/ipc_rpmsg.h>
#include "FreeRTOS.h"
#include "task.h"

// MCSPI
#define APP_MCSPI_MSGSIZE                   (256U)

uint8_t gMcspiTxBuffer[APP_MCSPI_MSGSIZE];
uint8_t gMcspiRxBuffer[APP_MCSPI_MSGSIZE];

// IPC
#define IPC_RPMESSAGE_SERVICE_PING        "ti.ipc4.ping-pong"
#define IPC_RPMESSAGE_ENDPT_PING          (13U)

/* This is used to run the echo test with user space kernel */
#define IPC_RPMESSAGE_SERVICE_CHRDEV      "rpmsg_chrdev"
#define IPC_RPMESSAGE_ENDPT_CHRDEV_PING   (14U)

/* Maximum size that message can have in this example
 * RPMsg maximum size is 512 bytes in linux including the header of 16 bytes.
 * Message payload size without the header is 512 - 16 = 496
 */
#define IPC_RPMESSAGE_MAX_MSG_SIZE        (496u)

/*
 * Number of RP Message ping "servers" we will start,
 * - one for ping messages for linux kernel "sample ping" client
 * - and another for ping messages from linux "user space" client using "rpmsg char"
 */
#define IPC_RPMESSAGE_NUM_RECV_TASKS         (2u)

/* RPMessage object used to recvice messages */
RPMessage_Object gIpcRecvMsgObject[IPC_RPMESSAGE_NUM_RECV_TASKS];

/* RPMessage object used to send messages to other non-Linux remote cores */
RPMessage_Object gIpcAckReplyMsgObject;

/* Task priority, stack, stack size and task objects, these MUST be global's */
#define IPC_RPMESSAGE_TASK_PRI         (8U)
#define IPC_RPMESSAGE_TASK_STACK_SIZE  (8*1024U)

uint8_t gIpcTaskStack[IPC_RPMESSAGE_NUM_RECV_TASKS][IPC_RPMESSAGE_TASK_STACK_SIZE] __attribute__((aligned(32)));
TaskP_Object gIpcTask[IPC_RPMESSAGE_NUM_RECV_TASKS];

volatile uint8_t gbShutdown = 0u;
volatile uint8_t gbShutdownRemotecoreID = 0u;

volatile uint8_t gHasUncheckedMail = 0u;

#define LCD_WIDTH 240
#define LCD_HEIGHT 240
#define LCD_COLOR_BYTES 2 // 16 bits per pixel

enum MessageType {
    NOTIFY_FULLFRAME = 737,
    NOTIFY_INTERLACE = 787
};

typedef struct {
    uint32_t type;
    uint32_t bytes;
    uint32_t offset;
} NotificationHeader;

volatile uint8_t gUserSharedMem[LCD_WIDTH*LCD_HEIGHT*2] __attribute__((aligned(4096), section(".bss.user_shared_mem")));

void fastSPI() 
{
    // uint32_t chNum = gConfigMcspi0ChCfg[0U].chNum;
    // a
}

void ipc_recv_task_main(void *args)
{
    int32_t status;
    __attribute__((aligned(4))) char recvMsg[IPC_RPMESSAGE_MAX_MSG_SIZE+1]; /* +1 for NULL char in worst case */
    uint16_t recvMsgSize, remoteCoreId;
    uint32_t remoteCoreEndPt;
    RPMessage_Object *pRpmsgObj = (RPMessage_Object *)args;

    DebugP_log("[IPC RPMSG ECHO] Remote Core waiting for messages at end point %d ... !!!\r\n",
        RPMessage_getLocalEndPt(pRpmsgObj)
    );

    /* wait for messages forever in a loop */
    while(1) {
        /* set 'recvMsgSize' to size of recv buf fer,
        * after return `recvMsgSize` contains actual size of valid data in recv buffer
        */
        // Debug messages are not printed beyond this point
        recvMsgSize = IPC_RPMESSAGE_MAX_MSG_SIZE; // could be shutdown message or data message
        status = RPMessage_recv(pRpmsgObj,
            recvMsg, &recvMsgSize,
            &remoteCoreId, &remoteCoreEndPt,
            SystemP_WAIT_FOREVER);

        if (gbShutdown == 1u) {
            break;
        }

        MCSPI_Transaction spiTransaction;
        MCSPI_Transaction_init(&spiTransaction);
        spiTransaction.channel   = gConfigMcspi0ChCfg[0].chNum;
        spiTransaction.dataSize  = 8;
        spiTransaction.csDisable = TRUE;
        spiTransaction.rxBuf     = NULL;
        spiTransaction.args      = NULL;

        NotificationHeader* notiHeader = (NotificationHeader*) recvMsg;
        int32_t transferOK;
        // Transfer full frame
        if (notiHeader->type == NOTIFY_FULLFRAME) {
            spiTransaction.count = LCD_WIDTH*LCD_HEIGHT / (spiTransaction.dataSize/8);
            spiTransaction.txBuf = (void *)gUserSharedMem;
            transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
            DebugP_assert(transferOK==SystemP_SUCCESS);

            // Send second half, can't send over 2^16 in one go
            spiTransaction.txBuf = (void *)gUserSharedMem + LCD_WIDTH*LCD_HEIGHT;
            transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
            DebugP_assert(transferOK==SystemP_SUCCESS);

        // Transfer odd/ even line alternately
        } else if (notiHeader->type == NOTIFY_INTERLACE) { 
            spiTransaction.count = notiHeader->bytes / (spiTransaction.dataSize/8);
            spiTransaction.txBuf = (void *)gUserSharedMem + notiHeader->offset;
            transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
            DebugP_assert(transferOK==SystemP_SUCCESS);

        // Transfer non-frame data
        } else {
            spiTransaction.count     = recvMsgSize / (spiTransaction.dataSize/8);
            spiTransaction.txBuf     = (void *)recvMsg;
            transferOK = MCSPI_transfer(gMcspiHandle[CONFIG_MCSPI0], &spiTransaction);
            DebugP_assert(transferOK==SystemP_SUCCESS);
        }

        // Reply to data message to indicate SPI data sent
        status = RPMessage_send(
            "okay", 8, // enough to hold the string
            remoteCoreId, remoteCoreEndPt,
            RPMessage_getLocalEndPt(pRpmsgObj),
            SystemP_WAIT_FOREVER);
        DebugP_assert(status==SystemP_SUCCESS);
    }

    /* Fllow the sequence for graceful shutdown for the last recv task */
    DebugP_log("[IPC RPMSG ECHO] Closing all drivers and going to WFI ... !!!\r\n");

    if (gbShutdownRemotecoreID) {
        /* ACK the shutdown message */
        IpcNotify_sendMsg(gbShutdownRemotecoreID,
            IPC_NOTIFY_CLIENT_ID_RP_MBOX, IPC_NOTIFY_RP_MBOX_SHUTDOWN_ACK, 1u);
    }

    Drivers_close();
    System_deinit();

    // RF5 enter wait for Wait For Interrupt mode
    __asm__ __volatile__ ("wfi"   "\n\t": : : "memory");
    
    vTaskDelete(NULL);
}

void ipc_rpmsg_create_recv_tasks()
{
    int32_t status;
    RPMessage_CreateParams createParams;
    TaskP_Params taskParams;

    RPMessage_CreateParams_init(&createParams);
    createParams.localEndPt = IPC_RPMESSAGE_ENDPT_PING;
    status = RPMessage_construct(&gIpcRecvMsgObject[0], &createParams);
    DebugP_assert(status==SystemP_SUCCESS);

    RPMessage_CreateParams_init(&createParams);
    createParams.localEndPt = IPC_RPMESSAGE_ENDPT_CHRDEV_PING;
    status = RPMessage_construct(&gIpcRecvMsgObject[1], &createParams);
    DebugP_assert(status==SystemP_SUCCESS);

    /* We need to "announce" to Linux client else Linux does not know a service exists on this CPU
     * This is not mandatory to do for RTOS clients
     */
    status = RPMessage_announce(CSL_CORE_ID_A53SS0_0,
        IPC_RPMESSAGE_ENDPT_PING, IPC_RPMESSAGE_SERVICE_PING);
    DebugP_assert(status==SystemP_SUCCESS);

    status = RPMessage_announce(CSL_CORE_ID_A53SS0_0,
        IPC_RPMESSAGE_ENDPT_CHRDEV_PING, IPC_RPMESSAGE_SERVICE_CHRDEV);
    DebugP_assert(status==SystemP_SUCCESS);

    /* Create the tasks which will handle the ping service */
    TaskP_Params_init(&taskParams);
    taskParams.name = "RPMESSAGE_PING";
    taskParams.stackSize = IPC_RPMESSAGE_TASK_STACK_SIZE;
    taskParams.stack = gIpcTaskStack[0];
    taskParams.priority = IPC_RPMESSAGE_TASK_PRI;
    /* we use the same task function for echo but pass the appropiate rpmsg handle to it, to echo messages */
    taskParams.args = &gIpcRecvMsgObject[0];
    taskParams.taskMain = ipc_recv_task_main;

    status = TaskP_construct(&gIpcTask[0], &taskParams);
    DebugP_assert(status == SystemP_SUCCESS);

    TaskP_Params_init(&taskParams);
    taskParams.name = "RPMESSAGE_CHAR_PING";
    taskParams.stackSize = IPC_RPMESSAGE_TASK_STACK_SIZE;
    taskParams.stack = gIpcTaskStack[1];
    taskParams.priority = IPC_RPMESSAGE_TASK_PRI;
    /* we use the same task function for echo but pass the appropiate rpmsg handle to it, to echo messages */
    taskParams.args = &gIpcRecvMsgObject[1];
    taskParams.taskMain = ipc_recv_task_main;

    status = TaskP_construct(&gIpcTask[1], &taskParams);
    DebugP_assert(status == SystemP_SUCCESS);
}

void ipc_rp_mbox_callback(uint16_t remoteCoreId, uint16_t clientId, uint32_t msgValue, void *args)
{
    DebugP_log("Mailbox callback, why not shown??\n");
    if (clientId == IPC_NOTIFY_CLIENT_ID_RP_MBOX) {
        /* Shutdown request from the remoteproc */
        if (msgValue == IPC_NOTIFY_RP_MBOX_SHUTDOWN) {
            gbShutdownRemotecoreID = remoteCoreId;
            gbShutdown = 1u;
            RPMessage_unblock(&gIpcRecvMsgObject[0]);
        
        /* Notifying a new RPMessage in the mailbox*/
        } else {
            RPMessage_unblock(&gIpcRecvMsgObject[1]);     
        }
    }
}

void hello_spi_main(void *args)
{
    DebugP_log("Enter hello_spi_main\n");

    /* Memfill SPI buffers */
    for(int i = 0; i < APP_MCSPI_MSGSIZE; i++) {
        gMcspiTxBuffer[i] = i;
    }

    int32_t status = RPMessage_waitForLinuxReady(SystemP_WAIT_FOREVER);
    DebugP_assert(status==SystemP_SUCCESS);

    /* Register a callback for the RP_MBOX messages from the Linux remoteproc driver*/
    IpcNotify_registerClient(IPC_NOTIFY_CLIENT_ID_RP_MBOX, &ipc_rp_mbox_callback, NULL);

    /* create message receive tasks, these tasks always run and never exit */
    ipc_rpmsg_create_recv_tasks();

    /* exit from this task, vTaskDelete() is called outside this function, so simply return */
}

