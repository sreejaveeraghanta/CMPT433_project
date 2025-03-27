#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* This module starts a MCU-R5F core for real time IO
 * It sends data to MCU_SPI0 only, which is used by the LCD HAT.
 * MCU MCSPI is used to circumvent the software emulation
 * and achieve higher throughput.
 */

// Initialize the SPI module, R5F module must be initialized before
int SPI_init();

int SPI_sendBytes(uint8_t *buf, size_t cnt);
int SPI_sendWords(uint32_t *buf, size_t cnt);

// Cleanup the SPI module
void SPI_deinit();

#ifdef __cplusplus
}
#endif
