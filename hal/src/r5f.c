#include "hal/r5f.h"

#include <stdbool.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#define R5F_FIRMWARE "hello_spi.release.out"
#define COMMAND_LEN 100

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1

static int s_remoteProcNo = STATUS_FAIL;
static bool s_isInited = false;

/* Find the remote processor number of MCU-R5F.
 * Usually it is remoteproc2, but there is a small chance it isn't
 * due to non-deterministic boot order */
static int findRemoteProcNumber()
{
    int remoteProcNum;
    DIR* dir = opendir("/sys/bus/platform/devices/79000000.r5f/remoteproc");
	if (!dir) {
        printf("SPI: can't open dir\n");
		return STATUS_FAIL;
	}

    // Extract the number X in directory remoteprocX
    struct dirent* entry;
    bool found = false;
	while ((entry = readdir(dir))) {
		if (entry->d_type == DT_DIR) {
            int ret = sscanf(entry->d_name, "remoteproc%d", &remoteProcNum);
			if(ret < 1) {
                continue;
            }
            found = true;
		}
	}
	if (!found) {
        printf("R5F: remoteProc number not found\n");
        return STATUS_FAIL;
    }
    closedir(dir);
    printf("R5F: Remote Prco found: %d\n", remoteProcNum);
    return remoteProcNum;
}

/* Executed command's priviledge should be inherited from parent.
 * It is also possible to use linux remoteproc API but need extra installation.
 * remoteproc API are called behind thse echo command */

// Set the firmware to the predefined ELF file located in /lib/firmware
// It was resetted to "j7-mcu-r5f0_0-fw.tisdk" everytime the board reboot
// Assume the firmware in already copied to /lib/firmware
static int setFirmware()
{
    char setFirmwareCommand[COMMAND_LEN];
    sprintf(setFirmwareCommand,
        "echo %s > /sys/class/remoteproc/remoteproc%d/firmware",
        R5F_FIRMWARE,
        s_remoteProcNo
    );
    int status = system(setFirmwareCommand);
    if (status != 0) {
        printf("R5F: Fail to set R5F firmware: %s\n", R5F_FIRMWARE);
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}

// Boot the R5F MCU processor
static int boot()
{
    char bootCommand[COMMAND_LEN];
    sprintf(bootCommand,
        "echo start > /sys/class/remoteproc/remoteproc%d/state",
        s_remoteProcNo
    );
    int status = system(bootCommand);
    if (status != 0) {
        printf("R5F: Fail to boot R5F\n");
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}

// Shutdown the R5F MCU Processor
static int shutdown()
{
    char shutdownCommand[COMMAND_LEN];
    sprintf(shutdownCommand,
        "echo stop > /sys/class/remoteproc/remoteproc%d/state",
        s_remoteProcNo
    );
    int status = system(shutdownCommand);
    if (status != 0) {
        printf("R5F: Fail to shutdown R5F\n");
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}

int R5F_init()
{
    if (s_isInited) {
        printf("R5F: module already initialized\n");
        return STATUS_FAIL;
    }
    s_remoteProcNo = findRemoteProcNumber();
    if (s_remoteProcNo == STATUS_FAIL) {
        return STATUS_FAIL;
    }
    int status = setFirmware();
    if (status == STATUS_FAIL) {
        return STATUS_FAIL;
    }
    status = boot();
    if (status == STATUS_FAIL) {
        return STATUS_FAIL;
    }
    s_isInited = true;
    printf("R5F: init success\n");
    return STATUS_SUCCESS;
}

void R5F_deinit()
{
    if (!s_isInited) {
        printf("R5F: module is not initialized\n");
    }
    int status = shutdown();
    if (status == STATUS_FAIL) {
        printf("R5F: Can't shutdown, deinit failed\n");
    }
    s_isInited = false;
}