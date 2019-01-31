/**
 * VW CD Changer Emulator
 *
 * This project allows bidirectional communication with a VW Head Unit by emulating a CD changer. Once the head unit
 * is convinced that there is a CD changer attached, the head unit will send key press events to this device, and this
 * device can send commands to set the CD/track number and the current play-time, etc.
 *
 * The aim of this microcontroller is to maintain as little state as humanly possible and serve as a communication
 * proxy to and from the VW head unit. The device driving commands _to_ the head unit speaks to this device over a
 * serial port connection, and receives key press events via the same serial port connection.
 */

#include <Arduino.h>
#include "vw_headunit.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup() {
  Serial.begin(9660);

  pinMode(LED_BUILTIN, OUTPUT);

  vw_headunit_spi_init();
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

/**
 * Setup external interrupt zero.
 *
 * See: https://www.microchip.com/webdoc/AVRLibcReferenceManual/group__avr__interrupts.html
 */
ISR(INT0_vect) {

}

/**
 * Setup external interrupt one.
 *
 * See: https://www.microchip.com/webdoc/AVRLibcReferenceManual/group__avr__interrupts.html
 */
ISR(INT1_vect) {

}
