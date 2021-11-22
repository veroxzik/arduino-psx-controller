#include "ps2.h"

/* internal button state define */
#define BUTTON_1     (1<<0)
#define BUTTON_2     (1<<1)
#define BUTTON_3     (1<<2)
#define BUTTON_4     (1<<3)
#define BUTTON_5     (1<<4)
#define BUTTON_6     (1<<5)
#define BUTTON_7     (1<<6)
#define BUTTON_8     (1<<7)
#define BUTTON_9     (1<<8)
#define BUTTON_10    (1<<9)

uint16_t button_state = 0;

void setup() {
  /* Pins setup */
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);

  /* PS2 Input Mapping */
  // map bits from internal button state bitfield to the corresponding PS buttons
  PS2_MapInput(&button_state, BUTTON_1, PS2_UP);
  PS2_MapInput(&button_state, BUTTON_2, PS2_DOWN);
  PS2_MapInput(&button_state, BUTTON_3, PS2_LEFT);
  PS2_MapInput(&button_state, BUTTON_4, PS2_RIGHT);
  PS2_MapInput(&button_state, BUTTON_5, PS2_TRIANGLE);
  PS2_MapInput(&button_state, BUTTON_6, PS2_CROSS);
  PS2_MapInput(&button_state, BUTTON_7, PS2_SQUARE);
  PS2_MapInput(&button_state, BUTTON_8, PS2_CIRCLE);
  PS2_MapInput(&button_state, BUTTON_9, PS2_START);
  PS2_MapInput(&button_state, BUTTON_10,PS2_SELECT);
  // if you need your controller to keep some buttons always pressed (e.g. pop'n controller)
  //PS2_AlwaysInput(PS2_LEFT|PS2_DOWN|PS2_RIGHT);

  /* PS2 Init */
  // Indicate how MISO will be used for the PS2.
  // * PS2_TRANSISTOR is the most reliable method, requiring an N-Channel MOSFET or transistor.
  //    * Connect the AVR MISO pin to the base/gate.
  //    * Connect the PS2 MISO pin to the collector/drain.
  //    * Connect the emitter/source to ground.
  //    * The BS170 N-Channel MOSFET works well with no gate resistor.
  // * PS2_DIRECT can be used if running at 3.3V.
  //    * This will provide varying levels of success based on a number of factors.
  //    * Cable quality and the presence of a ferrite core are factors that come to play.
  //    * A generic PS1 extension with no ferrites can work better than Konami's cable!
  //    * If PS2_DIRECT doesn't work for you, you must use PS2_TRANSISTOR.
  //
  // Additionally, indicate which pin you want to use for PS2 "Acknowledge" line in the header of ps2.c
  PS2_Init(PS2_DIRECT);
}

void loop() {

  button_state = 0;
  if (digitalRead(0) == LOW) {
    button_state |= BUTTON_1;
  }
  if (digitalRead(1) == LOW) {
    button_state |= BUTTON_2;
  }
  if (digitalRead(2) == LOW) {
    button_state |= BUTTON_3;
  }
  if (digitalRead(3) == LOW) {
    button_state |= BUTTON_4;
  }
  if (digitalRead(4) == LOW) {
    button_state |= BUTTON_5;
  }  
  if (digitalRead(5) == LOW) {
    button_state |= BUTTON_6;
  }
  if (digitalRead(6) == LOW) {
    button_state |= BUTTON_7;
  }
  if (digitalRead(7) == LOW) {
    button_state |= BUTTON_8;
  }
  if (digitalRead(8) == LOW) {
    button_state |= BUTTON_9;
  }
  if (digitalRead(9) == LOW) {
    button_state |= BUTTON_10;
  }

  PS2_Task();
  
}
