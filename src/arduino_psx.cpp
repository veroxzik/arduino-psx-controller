#include "arduino_psx.h"

ISR(SPI_STC_vect) {
    PSX.isr();
}

void PSX_::init(int ackPin = 8, bool invACK = false, bool invCIPO = true) {
    cipoDDR = __digitalPinToDDRReg(SPI_HW_MISO_PIN);
    cipoPORT = __digitalPinToPortReg(SPI_HW_MISO_PIN);
    cipoPinMask = __digitalPinToBit(SPI_HW_MISO_PIN);
    ackDDR = __digitalPinToDDRReg(ackPin);
    ackPORT = __digitalPinToPortReg(ackPin);
    ackPinMask = __digitalPinToBit(ackPin);
    invertACK = invACK;
    invertCIPO = invCIPO;

    if (invertACK) {
        (*ackDDR) |= (1 << ackPinMask);     // Set as output
        (*ackPORT) &= ~(1 << ackPinMask);   // Set LOW (open drain -- pullup on console)
    } else {
        (*ackDDR) &= ~(1 << ackPinMask);    // Set as input
        (*ackPORT) &= ~(1 << ackPinMask);   // Ensure pullup is off
    }

    if (invertCIPO) {
        (*cipoDDR) |= (1 << cipoPinMask);       // Set as output
        (*cipoPORT) &= ~(1 << cipoPinMask);     // Set LOW (open drain -- pullup on console)
    } else {
        (*cipoDDR) &= ~(1 << cipoPinMask);      // Set as input
        (*cipoPORT) &= ~(1 << cipoPinMask);     // Ensure pullup is off
    }

#if (F_CPU == 16000000L)
    SPCR |= (1 << SPR1);  // Fosc/64 @16MHz==250KHz
#elif (F_CPU == 8000000L)
    SPCR |= (1 << SPR2) | (1 << SPR1);  // Fosc/32 @8MHz==250KHz
#endif
    SPCR |= (1 << CPHA);  // Setup @ leading edge, sample @ falling edge
    SPCR |= (1 << CPOL);  // Leading edge is falling edge, trailing edge is rising edge
    SPCR &= ~(1 << MSTR); // MSTR bit is zero, SPI is slave
    SPCR |= (1 << DORD);  // Byte is transmitted LSB first, MSB last
    SPCR |= (1 << SPE);   // Enable SPI
    SPCR |= (1 << SPIE);  // Enable Serial Transfer Complete (STC) interrupt
    SPDR = 0xFF;

    sei();
}

void PSX_::setButton(uint16_t button, bool state) {
    if (state) {
        buttonState |= button;
    } else {
        buttonState &= ~button;
    }
    buttonState |= defaultState;    // Ensure the previously set Always Held buttons are indeed held
}

void PSX_::send() {
    // Fill data array with button info
    dataBuf[2] = ~(buttonState & 0xFF);
    dataBuf[3] = ~((buttonState >> 8) & 0xFF);

    // Check SS and clear memcard flag if necessary
    if ((PINB & (1 << 0)) > 0) {
        if (memcardActive) {
            memcardActive = false;
        }
        currentByte = 0;
        if (!invertCIPO) {
            (*cipoDDR) &= ~(1 << cipoPinMask);  // Disable output
        }
    }
}

void PSX_::isr() {
    uint8_t recvByte = SPDR;

    if (recvByte == commandBuf[currentByte] && memcardActive == false) {
        (*cipoDDR) |= (1 << 3);   // Enable output
        if (invertCIPO) {
            SPDR = ~dataBuf[currentByte];
        } else {
            SPDR = dataBuf[currentByte];
        }
        currentByte++;
        if (currentByte < 5) {
            _delay_us(8); // Necessary delay for PS1 (not needed for PS2)
            // Set ACK low
            if (invertACK) {
                (*ackPORT) |= (1 << ackPinMask);
            } else {
                (*ackDDR) |= (1 << ackPinMask);     // Enable output (default LOW)
            }
            _delay_us(1);
            // Set ACK high
            if (invertACK) {
                (*ackPORT) &= ~(1 << ackPinMask);
            } else {
                (*ackDDR) &= ~(1 << ackPinMask);
            }
        } else {
            (*cipoDDR) &= ~(1 << cipoPinMask);  // Disable output
            SPDR = 0xFF;
            currentByte = 0;
        }
    } else {
        if (recvByte == commandMemcard && currentByte == 0) {
            memcardActive = true;
        }
        (*cipoDDR) &= ~(1 << cipoPinMask);  // Disable output
        SPDR = 0xFF;
        currentByte = 0;
    }
}

PSX_ PSX;