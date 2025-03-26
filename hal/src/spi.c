#include "hal/spi.h"
#include "hal/r5f.h"
#include "hal/lcd.h"
#include "ti_rpmsg_char.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

// This should match IPC_RPMESSAGE_ENDPT_CHRDEV_PING in the firmware
#define REMOTE_ENDPT        14

#define RPMSG_ADDR_ANY      0xFFFFFFFF
#define SEND_BUFF_MAXLEN    480
#define EXPECTED_RES_MAXLEN 8

#define STATUS_SUCCESS 0
#define STATUS_FAIL (-1)

static rpmsg_char_dev_t *s_rcdev;
static bool s_isInited = false;

int SPI_init()
{
    // R5F_init();
    if (s_isInited) {
        printf("SPI: module already initialized\n");
        goto cleanup;
    }


    /* RPMSG */
	int status = rpmsg_char_init(NULL);
	if (status == STATUS_FAIL) {
		printf("SPI: rpmsg_char_init failed, status = %d\n", status);
		goto cleanup;
	}

    int rproc_id = R5F_MCU0_0;
    char* dev_name = NULL;
    unsigned int local_endpt = RPMSG_ADDR_ANY;
    unsigned int remote_endpt = REMOTE_ENDPT;
    char eptdev_name[64] = { 0 };
    int flags = 0;

    sprintf(eptdev_name, "rpmsg-char-%d-%d", rproc_id, getpid());
	s_rcdev = rpmsg_char_open(rproc_id, dev_name, local_endpt, remote_endpt,
				                eptdev_name, flags);
    if (!s_rcdev) {
        printf("SPI: Can't create an endpoint device\n");
        goto cleanup;
    }

    s_isInited = true;
	printf("SPI: Created endpt device %s, fd = %d port = %d\n", eptdev_name,
	    s_rcdev->fd, s_rcdev->endpt);
    printf("SPI: Exchanging messages with rpmsg device %s on rproc id %d...\n", eptdev_name, rproc_id);
    printf("SPI inited");
    return STATUS_SUCCESS;

cleanup:
    // R5F_deinit();
    return STATUS_FAIL;
}

enum MessageType {
    NOTIFY_FULLFRAME = 737,
    NOTIFY_INTERLACE = 787
};

typedef struct {
    uint32_t type;
    uint32_t bytes;
    uint32_t offset;
} NotificationHeader;

void SPI_notifyFrameReady()
{
    if (!s_isInited) {
        printf("SPI: notifyFrameReady failed, module not inited\n");
    }
    NotificationHeader header = {
        .type = NOTIFY_FULLFRAME,
        .bytes = LCD_FRAME_BUFFER_SIZE,
        .offset = 0,
    };
    int writtenBytes = write(s_rcdev->fd, &header, sizeof(NotificationHeader));
    if (writtenBytes < 0) {
        printf("SPI: notifyFrameReady can't write to rpmsg endpt device\n");
    }
    char replyMsgBuff[8] = {0};
    int readBytes = read(s_rcdev->fd, replyMsgBuff, EXPECTED_RES_MAXLEN);
    if (readBytes < 0) {
        printf("SPI: notifyFrameReady can't read from rpmsg endpt device\n");
    }
}

void SPI_notifyLineReady(uint32_t line)
{
    if (!s_isInited) {
        printf("SPI: SPI_notifyLineReady failed, module not inited\n");
    }
    NotificationHeader header = {
        .type = NOTIFY_INTERLACE,
        .bytes = LCD_WIDTH*LCD_COLOR_BYTES,
        .offset = line*LCD_WIDTH*LCD_COLOR_BYTES,
    };
    int writtenBytes = write(s_rcdev->fd, &header, sizeof(NotificationHeader));
    if (writtenBytes < 0) {
        printf("SPI: SPI_notifyLineReady can't write to rpmsg endpt device\n");
    }
    char replyMsgBuff[8] = {0};
    int readBytes = read(s_rcdev->fd, replyMsgBuff, EXPECTED_RES_MAXLEN);
    if (readBytes < 0) {
        printf("SPI: SPI_notifyLineReady can't read from rpmsg endpt device\n");
    }
}

int SPI_sendData(uint8_t *buffer, int bytes)
{
    if (!s_isInited) {
        printf("SPI: sendData Streamfailed, module not inited\n");
    }
    int unsentBytes = bytes;
    while(unsentBytes > 0) {
        int bytesToSend = (unsentBytes >= SEND_BUFF_MAXLEN)
            ? SEND_BUFF_MAXLEN
            : unsentBytes; // Last transaction
        
        int writtenBytes = write(s_rcdev->fd, buffer, bytesToSend);
        if (writtenBytes < 0) {
            printf("SPI: sendData can't write to rpmsg endpt device\n");
            return STATUS_FAIL;
        }

        char replyMsgBuff[8] = {0};
        int readBytes = read(s_rcdev->fd, replyMsgBuff, EXPECTED_RES_MAXLEN);
        if (readBytes < 0) {
            printf("SPI: Can't read from rpmsg endpt device\n");
            return STATUS_FAIL;
        }
        unsentBytes -= bytesToSend;
        buffer += bytesToSend;
    }
	return bytes;
}

void SPI_deinit()
{    
    if (!s_isInited) {
        printf("SPI: Deinit fail, module not inited\n");
        return;
    }
	int ret = rpmsg_char_close(s_rcdev);
	if (ret < 0) {
		printf("SPI: Deinit fail, can't close rcdev\n");
    }
    rpmsg_char_exit();
    // R5F_deinit();
    // munmap(s_memMap, PAGE_SIZE);
    // close(s_memFd);
    s_isInited = false;
    printf("SPI deinited\n");
}