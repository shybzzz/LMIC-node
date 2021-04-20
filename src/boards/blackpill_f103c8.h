/*******************************************************************************
 * 
 *  File:         blackpill_f103c8.h
 * 
 *  Function:     Board Support File for STM32 F103C8T6 'blackpill' 64k and 128k
 *                with external SPI LoRa module.
 * 
 *  Copyright:    Copyright (c) 2021 Leonel Lopes Parente
 * 
 *  License:      MIT License. See accompanying LICENSE file.
 * 
 *  Author:       Leonel Lopes Parente
 * 
 *  Description:  This Board has onboard USB (provided by the MCU).
 *                However, the default bootloader does not support automatic 
 *                firmware upload and serial over USB.
 *                No onboard display. Optionally an external display can be connected.
 * 
 *                For firmware upload use a USB to serial adapter, STLink programmer,
 *                or special bootloader (not standard supported).
 *                The upload_protocol setting for this board (in platformio.ini) 
 *                must be set to the preferred upload method (default is stlink).
 *                For serial monitor use a USB to serial adapter.
 *                (It may also be possible to configure the onboard USB
 *                for this but I haven't yet figured out how.)
 * 
 *                This board requires a workaround to prevent losing output
 *                printed to the serial port (see boardInit() below).
 * 
 *                Connect the LoRa module and optional display
 *                according to below connection details.
 * 
 *                CONNECTIONS AND PIN DEFINITIONS:
 *                
 *                Indentifiers between parentheses are defined in the board's 
 *                Board Support Package (BSP) which is part of the Arduino core. 
 * 
 *                Leds                GPIO         
 *                ----                ----
 *                LED   <――――――――――>  PB12 / 16  (LED_BUILTIN)  Active-low
 * 
 *                I2C [display]       GPIO 
 *                ---                 ---- 
 *                SDA   <――――――――――>  PB7 / 2    (SCA)
 *                SCL   <――――――――――>  PB6 / 3    (SCL)
 *
 *                SPI/LoRa module     GPIO
 *                ---                 ----
 *                MOSI  <――――――――――>  PA7 / 27   (MOSI)
 *                MISO  <――――――――――>  PA6 / 26   (MISO)
 *                SCK   <――――――――――>  PA5 / 25   (SCK)
 *                NSS   <――――――――――>  PA4 / 24   (SS)
 *                RST   <――――――――――>  PB0 / 28
 *                DIO0  <――――――――――>  PA3 / 23
 *                DIO1  <――――――――――>  PA2 / 22
 *                DIO2                -          Not needed for LoRa.
 * 
 *                Serial              GPIO
 *                ------              ----
 *                RX    <――――――――――>  PA10 / 10  (PIN_SERIAL_RX)
 *                TX    <――――――――――>  PA9  / 11  (PIN_SERIAL_TX)
 * 
 *                SWD                 GPIO
 *                ---                 ----
 *                SWCLK <――――――――――>  PA14 / 34
 *                SWDIO <――――――――――>  PA13 / 33
 * 
 *                USB                 GPIO
 *                ---                 ----
 *                D-    <――――――――――>  PA11 / 9
 *                D+    <――――――――――>  PA12 / 8       
 * 
 *  Identifiers:  LMIC-node
 *                    board-id:       blackpill_f103c8          (64k version)
 *                                    blackpill_f103c8_128k    (128k version)
 *                PlatformIO
 *                    board:          blackpill_f103c8          (64k version)
 *                                    blackpill_f103c8_128k    (128k version)
 *                    platform:       ststm32
 *                Arduino
 *                    board:          ARDUINO_BLACKPILL_F103C8
 *                    architecture:   ARDUINO_ARCH_STM32 
 * 
 ******************************************************************************/

#pragma once

#ifndef BLACKPILL_F103C8_H_
#define BLACKPILL_F103C8_H_

#include "lmic-node.h"

#define DEVICEID_DEFAULT "blackpill"

// Wait for Serial
// Can be useful for boards with MCU with integrated USB support.
// This board by default does not support serial over USB but
// wait for serial is enabled anyway for if boards have a different bootloader.
#define WAITFOR_SERIAL_SECONDS_DEFAULT 10   // -1 waits indefinitely

// LMIC Clock Error
// This is only needed for slower 8-bit MCUs (e.g. 8MHz ATmega328 and ATmega32u4).
// Value is defined in parts per million (of MAX_CLOCK_ERROR).
// #define LMIC_CLOCK_ERROR_PPM 0

// Pin mappings for LoRa tranceiver
const lmic_pinmap lmic_pins = {
    .nss = PA4,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PB0,
    .dio = { /*dio0*/ PA3, /*dio1*/ PA2, /*dio2*/ LMIC_UNUSED_PIN }
#ifdef MCCI_LMIC
    ,
    .rxtx_rx_active = 0,
    .rssi_cal = 10,
    .spi_freq = 8000000     /* 8 MHz */
#endif    
};

#ifdef USE_SERIAL
    HardwareSerial& serial = Serial;
#endif   

#ifdef USE_LED
    EasyLed led(LED_BUILTIN, EasyLed::ActiveLevel::Low);
#endif

#ifdef USE_DISPLAY
    // Create U8x8 instance for SSD1306 OLED display (no reset) using hardware I2C.
    U8X8_SSD1306_128X64_NONAME_HW_I2C display(/*rst*/ U8X8_PIN_NONE, /*scl*/ SCL, /*sda*/ SDA);
#endif


bool boardInit(InitType initType)
{
    // This function is used to perform board specific initializations.
    // Required as part of standard template.

    // InitType::Hardware        Must be called at start of setup() before anything else.
    // InitType::PostInitSerial  Must be called after initSerial() before other initializations.    

    bool success = true;
    switch (initType)
    {
        case InitType::Hardware:
            // Note: Serial port and display are not yet initialized and cannot be used use here.
            // No actions required for this board.
            break;

        case InitType::PostInitSerial:
            // Note: If enabled Serial port and display are already initialized here.
            
            // Required workaround:
            // Data printed to the serial port during the first second gets lost.
            // Inserting a 1500 millisecond delay will usually fix this.
            delay(1500);
            break;
    }
    return success;
}


#endif  // BLACKPILL_F103C8_H_