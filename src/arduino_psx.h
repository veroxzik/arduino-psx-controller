#ifndef PSX_h
#define PSX_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#error "Arduino versions older than v1.0 are not supported."
#endif

#include "digitalWriteFast.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

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
        volatile uint8_t *cipoDDR;
        volatile uint8_t *cipoPORT;
        volatile uint8_t cipoPinMask;
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