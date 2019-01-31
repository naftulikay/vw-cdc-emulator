/**
 * Headers describing VW head unit operations.
 */

// include guard to prevent inclusion multiple time
#ifndef VW_HEADUNIT_H
#define VW_HEADUNIT_H

#include <stdint.h>

/**
 * Read a command over SPI from the head unit.
 */
uint8_t vw_headunit_read_command(uint8_t command);

/**
 * Send an arbitrary byte over SPI to the head unit.
 */
uint8_t vw_headunit_send_byte(uint8_t payload);

/**
 * Send an arbitrary set of eight bytes over SPI to the head unit.
 */
uint8_t vw_headunit_send_command(uint8_t package[8]);

/**
 * Setup the SPI interface to the head unit.
 */
void vw_headunit_spi_init();

#endif
