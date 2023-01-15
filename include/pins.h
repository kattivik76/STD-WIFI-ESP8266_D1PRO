#ifndef _PINS_CONFIG_H_
#define _PINS_CONFIG_H_

  // >>> PIN DEFINITIONS ---------------------
  /* pin table conversion [ESP] | [ARDUINO]
   * NodeMCU
   * static const uint8_t D0   = 16;  12k pull-up BUILT-IN LED ONBOARD
   * static const uint8_t D1   = 5;
   * static const uint8_t D2   = 4;   10k pull-up
   * static const uint8_t D3   = 0;   10k pull-up
   * static const uint8_t D4   = 2;   BUILT-IN LED WIFI(reserved preferred)
   * static const uint8_t D5   = 14;
   * static const uint8_t D6   = 12;
   * static const uint8_t D7   = 13;
   * static const uint8_t D8   = 15;
   * static const uint8_t D9   = 3;
   * static const uint8_t D10  = 1;
   *
   * WEMOS D1 MINI PRO
   * #define PIN_WIRE_SDA (4)
   * #define PIN_WIRE_SCL (5)
   *
   * static const uint8_t SDA = PIN_WIRE_SDA;
   * static const uint8_t SCL = PIN_WIRE_SCL;
   *
   * #define LED_BUILTIN 2
   *
   * static const uint8_t D0   = 16;  GPIO
   * static const uint8_t D1   = 5;   SDA
   * static const uint8_t D2   = 4;   SCL
   * static const uint8_t D3   = 0;   10k pull-up (reserved preferred, HIGH >>> RUN, LOW >>> FLASH)
   * static const uint8_t D4   = 2;   10k pull-up, BUILT-IN LED (reserved preferred)
   * static const uint8_t D5   = 14;  SCK
   * static const uint8_t D6   = 12;  MISO
   * static const uint8_t D7   = 13;  MOSI
   * static const uint8_t D8   = 15;  10k pull-down, SS (reserved preferred)
   * static const uint8_t RX   = 3;   RX
   * static const uint8_t TX   = 1;   TX
   *
   * * WEMOS D1 MINI PRO (this)
   * #define PIN_WIRE_SDA (   4)
   * #define PIN_WIRE_SCL (5)
   *
   * static const uint8_t SDA = PIN_WIRE_SDA;
   * static const uint8_t SCL = PIN_WIRE_SCL;
   *
   * #define LED_BUILTIN 2
   *
   * static const uint8_t D0   = 16;  [S1]
   * static const uint8_t D1   = 5;
   * static const uint8_t D2   = 4;
   * static const uint8_t D3   = 0;   [RESERVED]  10k pull-up (reserved preferred, HIGH >>> RUN, LOW >>> FLASH)
   * static const uint8_t D4   = 2;   [RESERVED]  10k pull-up, BUILT-IN LED
   * static const uint8_t D5   = 14;  [S2]
   * static const uint8_t D6   = 12;  [LED1]
   * static const uint8_t D7   = 13;
   * static const uint8_t D8   = 15;  [RESERVED]  10k pull-down, SS
   * static const uint8_t RX   = 3;   [RESERVED]  RX
   * static const uint8_t TX   = 1;   [RESERVED]  TX
   */

   #define S1 D0                                       //Pin input S1
   #define S2 D5                                       //Pin input S2
   #define LED1 D6                                     //Pin output LED1
  // <<< PIN DEFINITIONS ---------------------

#endif // _PINS_CONFIG_H_
