#include "ps2.h"

/* USER CUSTOMIZABLE SETTINGS */
#define ACK_PORT  PORTB
#define ACK_DDR   DDRB
#define ACK_PIN   4     // PB4 (Pin 8 on Micro) 
/* END OF USER CUSTOMIZABLE SETTINGS */

// Port and Pin mask to setup
uint8_t PinMask = (1<<ACK_PIN);

// Stores a constructed packet for the PS2.
uint16_t Data = 0;
// When set, ignore all data until chip select goes high again
// Shoutouts to @nicolasnoble for the insight on how the PS1 handles chip select
uint8_t QuietTime = 0;
// List of available PS2 inputs.
PS2_InputList_t *PS2Input = NULL;

// Current PS2 state.
void (*PS2Handler)(uint8_t) = NULL;

// Invert mask for PS2 data.
uint8_t InvertMask = 0x00;

inline uint8_t ps2_byte(uint8_t data) {
  return data ^ InvertMask;
}

void PS2_Acknowledge(void) {
  // Burn a few cycles before acknowledging
  asm volatile(
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
  );
  
  ACK_DDR |=  PinMask;
  
  // 40 cycles of delay should give us the same delay as a real PS1 controller
  asm volatile(
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
  );
  ACK_DDR  &= ~(PinMask);
}

// Default implementation methods
void PS2_Listen(uint8_t in);
void PS2_Addressed(uint8_t in);
void PS2_HeaderFinished(uint8_t in);
void PS2_LowerSent(uint8_t in);

// Idle state.
void PS2_Listen(uint8_t in) {
  // Report as a digital controller when addressed
  if (in == 0x01) {
    SPDR = ps2_byte(0x41);
    PS2Handler = PS2_Addressed;
    PS2_Acknowledge();
    return;
  }
  // Otherwise, ignore all incoming traffic until our task performs a reset
  DDRB &= ~0x08;
  QuietTime = 255;
}

// When polling is requested, begin responding
void PS2_Addressed(uint8_t in) {
  if (in == 0x42) {
    SPDR = ps2_byte(0x5A);
    PS2Handler = PS2_HeaderFinished;
    PS2_Acknowledge();
  }
}

// After end-of-header sent, send the first byte
void PS2_HeaderFinished(uint8_t in) {
  uint8_t *data = (uint8_t *)&Data;

  SPDR = ps2_byte(*data);
  PS2Handler = PS2_LowerSent;
  PS2_Acknowledge();
}

// After first byte sent, send the second and go back to listening.
void PS2_LowerSent(uint8_t in) {
  uint8_t *data = (uint8_t *)&Data + 1;

  SPDR = ps2_byte(*data);
  PS2Handler = PS2_Listen;
  PS2_Acknowledge();
}

void PS2_Init(PS2_INVERT invert) {
  cli();
  
  InvertMask = invert;

  ACK_PORT &= ~(PinMask);
  
  PS2_Acknowledge();
  // Set MISO as an output pin
  DDRB |= 0x08;
  // Setup data on falling edge, sample on rising edge (SPI mode 3)
  SPCR  = (1 << CPOL) | (1 << CPHA)
  // Transmit LSB first
        | (1 << DORD)
  // Enable interrupts for SPI
        | (1 << SPIE)
  // Enable SPI
        | (1 << SPE);

  // Set the first byte up
  SPDR = ps2_byte(0xFF);
  // Setup our default and current handlers
  PS2Handler = PS2_Listen;
  // Re-enable interrupts
  sei();
}

// Update the stored data packet
void PS2_Task(void) {
  // If chip select is high (not selected), quiet time is over. Reset state.
  if (PINB & 0x01) {
    QuietTime = 0;
    DDRB |= 0x08;
    SPDR  = ps2_byte(0xFF);
  }

  PS2_InputList_t *map = PS2Input;
  uint16_t new_data = 0;

  while(map) {
    if (!map->input || (*map->input & map->mask))
      new_data |= map->buttons;

    map = map->parent;
  }

  Data = ~new_data;
}

void PS2_MapInput(uint16_t *input, uint16_t mask, PS2_INPUT buttons) {
  PS2_InputList_t *child = calloc(1, sizeof(PS2_InputList_t));

  child->input    = input,
  child->mask     = mask,
  child->buttons  = buttons,
  child->parent   = PS2Input;

  PS2Input = child;
}

void PS2_AlwaysInput(PS2_INPUT buttons) {
  PS2_InputList_t *child = calloc(1, sizeof(PS2_InputList_t));

  child->input    = NULL,
  child->mask     = 0,
  child->buttons  = buttons,
  child->parent   = PS2Input;

  PS2Input = child;
}

// When a transfer is complete, determine what to do next
ISR(SPI_STC_vect) {
  // If chip select is still enabled, stay in quiet mode if set.
  if (QuietTime) return;

  uint8_t input = SPDR;
  // If our current input packet is polling the controller, re-enable writes and listen
  if (input == 0x01)
    PS2Handler = PS2_Listen;

  PS2Handler(input);
}
