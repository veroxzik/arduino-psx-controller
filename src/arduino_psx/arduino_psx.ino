/* USER CUSTOMIZABLE SETTINGS */
#define ACK_PORT  PORTB
#define ACK_DDR   DDRB
#define ACK_PIN   4     // PB4 (Pin 8 on Micro)


/* RECOMMENDED DO NOT CHANGE */
#define INVERT_CIPO 1   // Set to 1 if CIPO is open-drain via a transistor
#define INVERT_ACK  1   // Set to 1 if ACK is open-drain via a transistor


/* PSX DEFINE */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define SPI_PORT  PORTB
#define SPI_PINS  PINB
#define SPI_DDR   DDRB

#define ATT_PIN   0 // ~CS
#define CMD_PIN   2 // COPI
#define DATA_PIN  3 // CIPO
#define CLK_PIN   1 // SCK
#define DATA_LEN  5

#define PS_SELECT   (1 << 0)
#define PS_L3       (1 << 1)
#define PS_R3       (1 << 2)
#define PS_START    (1 << 3)
#define PS_UP       (1 << 4)
#define PS_RIGHT    (1 << 5)
#define PS_DOWN     (1 << 6)
#define PS_LEFT     (1 << 7)
#define PS_L2       (1 << 8)
#define PS_R2       (1 << 9)
#define PS_L1       (1 << 10)
#define PS_R1       (1 << 11)
#define PS_TRIANGLE (1 << 12)
#define PS_CIRCLE   (1 << 13)
#define PS_CROSS    (1 << 14)
#define PS_SQUARE   (1 << 15)
/* /PSX DEFINE */

/* PSX globals */
#if INVERT_CIPO
uint8_t data_buff[DATA_LEN] = {~0x41, ~0x5A, ~0xFF, ~0xFF, ~0xFF}; //Reply.
#else
uint8_t data_buff[DATA_LEN] = {0x41, 0x5A, 0xFF, 0xFF, 0xFF}; //Reply.
#endif
uint8_t command_buff[DATA_LEN] = {0x01, 0x42, 0x00, 0x00, 0x00};
uint8_t command_memcard = 0x81;
volatile bool memcard_active = false;

volatile uint8_t curr_byte = 0;
volatile uint8_t next_byte = 0;

uint8_t byte4;
uint8_t byte5;

/* NOTE: a bit set to 1 is unpressed, a bit set to 0 is pressed */
void convertInputToPSX(uint16_t state) {

  byte4 = ~(state & 0xFF);
  byte5 = ~((state >> 8) & 0xFF);

#if INVERT_CIPO
  data_buff[2] = ~byte4;
  data_buff[3] = ~byte5;
#else
  data_buff[2] = byte4;
  data_buff[3] = byte5;
#endif
}

ISR(SPI_STC_vect) {
  uint8_t inbyte = SPDR;

  if (inbyte == command_buff[curr_byte] && memcard_active == false) {
    SPI_DDR |= (1 << DATA_PIN); // enable output
    SPDR = data_buff[curr_byte];
    curr_byte++;
    if (curr_byte < DATA_LEN) {
      _delay_us(8); // Necessary delay for PS1 (not needed for PS2)
      // Set ACK low
#if INVERT_ACK
      ACK_PORT |= (1 << ACK_PIN);
#else
      ACK_DDR |= (1 << ACK_PIN);    // enable output
#endif
      _delay_us(1);
      // Set ACK high
#if INVERT_ACK
      ACK_PORT &= ~(1 << ACK_PIN);  // Release ACK
#else
      ACK_DDR &= ~(1 << ACK_PIN); // disable output
#endif

    } else {
      SPI_DDR &= ~(1 << DATA_PIN); // disable output
      SPDR = 0xFF;
      curr_byte = 0;
    }
  } else {
    if (inbyte == command_memcard && curr_byte == 0) {
      memcard_active = true;
    }
    SPI_DDR &= ~(1 << DATA_PIN); // disable output
    SPDR = 0xFF;
    curr_byte = 0;
  }
}
/* /PSX globals */

uint16_t button_state = 0;

void setup() {

  /* PSX setup */
#if INVERT_ACK
  ACK_DDR |= (1 << ACK_PIN);    // output
  ACK_PORT &= ~(1 << ACK_PIN);  // set LOW (open drain -- pull-up on console)
#else
  ACK_DDR &= ~(1 << ACK_PIN); // input
  ACK_PORT &= ~(1 << ACK_PIN); // ensure pullup is off
#endif

#if INVERT_CIPO
  SPI_DDR |= (1 << DATA_PIN); // output
  SPI_PORT &= ~(1 << DATA_PIN); //set LOW (open drain -- pull-up on console)
#else
  SPI_DDR &= ~(1 << DATA_PIN); // input
  SPI_PORT &= ~(1 << DATA_PIN); // ensure pullup is off
#endif

  //SPI setup
  SPCR |= (1 << SPR1);  // Fosc/64. @16MHz==250KHz.
  SPCR |= (1 << CPHA);  // Setup @ leading edge, sample @ falling edge.
  SPCR |= (1 << CPOL);  // Leading edge is falling edge, trailing edge is rising edge.
  SPCR &= ~(1 << MSTR); // MSTR bit is zero, SPI is slave.
  SPCR |= (1 << DORD);  // Byte is transmitted LSB first, MSB last.
  SPCR |= (1 << SPE);   // Enable SPI.
  SPCR |= (1 << SPIE);  // Enable Serial Transfer Complete (STC) interrupt.
  SPDR = 0xFF;

  sei();  // Enable global interrupts
  /* /PSX setup */

  /* --- --- --- --- --- --- */
  /*  USER CODE STARTS HERE  */
  /* --- --- --- --- --- --- */

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
}

void loop() {

  button_state = 0;
  if (digitalRead(A0) == LOW) {
    button_state |= PS_LEFT;
  }
  if (digitalRead(A1) == LOW) {
    button_state |= PS_DOWN;
  }
  if (digitalRead(A2) == LOW) {
    button_state |= PS_UP;
  }
  if (digitalRead(A3) == LOW) {
    button_state |= PS_RIGHT;
  }
  if (digitalRead(A4) == LOW) {
    button_state |= PS_CIRCLE;
  }
  if (digitalRead(A5) == LOW) {
    button_state |= PS_CROSS;
  }

  /* --- --- --- --- --- --- */
  /*   USER CODE ENDS HERE   */
  /* --- --- --- --- --- --- */

  convertInputToPSX(button_state);

  if ((SPI_PINS & (1 << ATT_PIN)) > 0) {
    if (memcard_active) {
      memcard_active = false;
    }
    curr_byte = 0;
#if INVERT_CIPO == 0
    SPI_DDR &= ~(1 << DATA_PIN); // input
    SPI_PORT &= ~(1 << DATA_PIN); // ensure pullup is off
#endif
  }
}
