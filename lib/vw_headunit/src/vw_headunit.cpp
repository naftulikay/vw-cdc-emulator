/**
 *
 * Relevant Documentation:
 *   - Special Function Registers: https://bit.ly/2DNI9Yj
 *   - Register Info: apt install -y avr-libc && grep -RiP 'PORTA' /usr/lib/avr/include/
 */

#include <Arduino.h>

void vw_headunit_spi_init() {
  Serial.println("called vw_headunit_spi_init.");
}
