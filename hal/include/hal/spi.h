#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* This module starts a MCU-R5F core for real time IO
 * It sends data to MCU_SPI0 only, which is used by the LCD HAT.
 * MCU MCSPI is used to circumvent the software emulation
 * and achieve higher throughput.
 */

// Initialize the SPI module, R5F module must be initialized before
int SPI_init();

// Notify the R5F next frame is ready, shared memory is used retrieve the frame
void SPI_notifyFrameReady();

// Notify R5F to send next line in interlaced mode
void SPI_notifyLineReady(uint32_t line);

// Sending data to R5F using RPMessage
int SPI_sendData(uint8_t *buffer, int bytes);

// Cleanup the SPI module
void SPI_deinit();

#ifdef __cplusplus
}
#endif
