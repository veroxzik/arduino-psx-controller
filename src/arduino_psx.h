#ifndef PSX_h
#define PSX_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#error "Arduino versions older than v1.0 are not supported."
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* 
    Following section from Arduino-Libs (digitalWriteFast)

    Copyright (c) 2011-2020 Watterott electronic (www.watterott.com)
    All rights reserved.
*/

// --- Arduino Leonardo and ATmega16U4/32U4 based boards ---
#if (defined(ARDUINO_AVR_LEONARDO) || \
       defined(__AVR_ATmega16U4__) || \
       defined(__AVR_ATmega32U4__))

#define UART_RX_PIN     (0) //PD2
#define UART_TX_PIN     (1) //PD3

#define I2C_SDA_PIN     (2) //PD1
#define I2C_SCL_PIN     (3) //PD0

#define SPI_HW_SS_PIN   (17) //PB0
#define SPI_HW_MOSI_PIN (16) //PB2
#define SPI_HW_MISO_PIN (14) //PB3
#define SPI_HW_SCK_PIN  (15) //PB1

#define __digitalPinToPortReg(P) \
((((P) >= 0 && (P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &PORTD : (((P) == 5 || (P) == 13) ? &PORTC : (((P) >= 18 && (P) <= 23)) ? &PORTF : (((P) == 7) ? &PORTE : &PORTB)))
#define __digitalPinToDDRReg(P) \
((((P) >= 0 && (P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &DDRD : (((P) == 5 || (P) == 13) ? &DDRC : (((P) >= 18 && (P) <= 23)) ? &DDRF : (((P) == 7) ? &DDRE : &DDRB)))
#define __digitalPinToPINReg(P) \
((((P) >= 0 && (P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &PIND : (((P) == 5 || (P) == 13) ? &PINC : (((P) >= 18 && (P) <= 23)) ? &PINF : (((P) == 7) ? &PINE : &PINB)))
#define __digitalPinToBit(P) \
(((P) >= 8 && (P) <= 11) ? (P) - 4 : (((P) >= 18 && (P) <= 21) ? 25 - (P) : (((P) == 0) ? 2 : (((P) == 1) ? 3 : (((P) == 2) ? 1 : (((P) == 3) ? 0 : (((P) == 4) ? 4 : (((P) == 6) ? 7 : (((P) == 13) ? 7 : (((P) == 14) ? 3 : (((P) == 15) ? 1 : (((P) == 16) ? 2 : (((P) == 17) ? 0 : (((P) == 22) ? 1 : (((P) == 23) ? 0 : (((P) == 24) ? 4 : (((P) == 25) ? 7 : (((P) == 26) ? 4 : (((P) == 27) ? 5 : 6 )))))))))))))))))))


// --- Arduino Uno and ATmega168/328 based boards ---
#elif (defined(ARDUINO_AVR_UNO) || \
       defined(ARDUINO_AVR_DUEMILANOVE) || \
       defined(__AVR_ATmega8__) || \
       defined(__AVR_ATmega48__) || \
       defined(__AVR_ATmega48P__) || \
       defined(__AVR_ATmega48PB__) || \
       defined(__AVR_ATmega88P__) || \
       defined(__AVR_ATmega88PB__) || \
       defined(__AVR_ATmega168__) || \
       defined(__AVR_ATmega168PA__) || \
       defined(__AVR_ATmega168PB__) || \
       defined(__AVR_ATmega328__) || \
       defined(__AVR_ATmega328P__) || \
       defined(__AVR_ATmega328PB__))

#define UART_RX_PIN     (0) //PD0
#define UART_TX_PIN     (1) //PD1

#define I2C_SDA_PIN     (18) //A4
#define I2C_SCL_PIN     (19) //A5

#define SPI_HW_SS_PIN   (10) //PB0
#define SPI_HW_MOSI_PIN (11) //PB2
#define SPI_HW_MISO_PIN (12) //PB3
#define SPI_HW_SCK_PIN  (13) //PB1

#if defined(__AVR_ATmega48PB__) || defined(__AVR_ATmega88PB__) || defined(__AVR_ATmega168PB__) || defined(__AVR_ATmega328PB__)
#define __digitalPinToPortReg(P) \
(((P) >= 0 && (P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : (((P) >= 14 && (P) <= 19) ? &PORTC : &PORTE)))
#define __digitalPinToDDRReg(P) \
(((P) >= 0 && (P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : (((P) >= 14 && (P) <= 19) ? &DDRC : &DDRE)))
#define __digitalPinToPINReg(P) \
(((P) >= 0 && (P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : (((P) >= 14 && (P) <= 19) ? &PINC : &PINE)))
#define __digitalPinToBit(P) \
(((P) >= 0 && (P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P) - 8 : (((P) >= 14 && (P) <= 19) ? (P) - 14 : (((P) >= 20 && (P) <= 21) ? (P) - 18 : (P) - 22))))
#else
#define __digitalPinToPortReg(P) \
(((P) >= 0 && (P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
#define __digitalPinToDDRReg(P) \
(((P) >= 0 && (P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
#define __digitalPinToPINReg(P) \
(((P) >= 0 && (P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
#define __digitalPinToBit(P) \
(((P) >= 0 && (P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P) - 8 : (P) - 14))
#endif

#else
#error "This library does not support any processor besides the ATmega32U4 or ATmega16U4"
#endif

/* End Arduino-Libs Section */

typedef enum {
    PS_NONE     = 0,
    PS_SELECT   = (1 << 0),
    PS_L3       = (1 << 1),
    PS_R3       = (1 << 2),
    PS_START    = (1 << 3),
    PS_UP       = (1 << 4),
    PS_RIGHT    = (1 << 5),
    PS_DOWN     = (1 << 6),
    PS_LEFT     = (1 << 7),
    PS_L2       = (1 << 8),
    PS_R2       = (1 << 9),
    PS_L1       = (1 << 10),
    PS_R1       = (1 << 11),
    PS_TRIANGLE = (1 << 12),
    PS_CIRCLE   = (1 << 13),
    PS_CROSS    = (1 << 14),
    PS_SQUARE   = (1 << 15),
} PS_INPUT;

class PSX_ {

    private:
        uint16_t buttonState = 0;
        uint16_t defaultState = 0;
        bool invertCIPO = true;
        bool invertACK = true;
        volatile uint8_t *ackDDR;
        volatile uint8_t *ackPORT;
        volatile uint8_t ackPinMask;

        uint8_t dataBuf[5] = {0x41, 0x5A, 0xFF, 0xFF, 0xFF};
        uint8_t commandBuf[5] = {0x01, 0x42, 0x00, 0x00, 0x00};
        uint8_t commandMemcard = 0x81;
        volatile bool memcardActive = false;
        volatile uint8_t currentByte = 0;

    public:
        void init(int ackPin, bool invACK = false, bool invCIPO = true);
        void setAlwaysHeld(uint16_t buttons) {
            defaultState = buttons;
            buttonState = defaultState;
        };

        void setButton(uint16_t button, bool state);
        void send();

        void isr();

};

extern PSX_ PSX;

#endif  // PSX_h