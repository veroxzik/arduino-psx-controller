#include "ps2.h"

/* internal button state define */
#define BUTTON_1     (1<<0)
#define BUTTON_2     (1<<1)
#define BUTTON_3     (1<<2)
#define BUTTON_4     (1<<3)
#define BUTTON_5     (1<<4)

uint16_t button_state = 0;

void setup() {
  /* Pins setup */
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);

  /* PS2 Input Mapping */
  // map bits from internal button state bitfield to the corresponding PS buttons
  PS2_MapInput(&button_state, BUTTON_1, PS2_UP);
  PS2_MapInput(&button_state, BUTTON_2, PS2_DOWN);
  PS2_MapInput(&button_state, BUTTON_3, PS2_LEFT);
  PS2_MapInput(&button_state, BUTTON_4, PS2_RIGHT);
  PS2_MapInput(&button_state, BUTTON_5, PS2_START);
  // if you need your controller to keep some buttons always pressed (e.g. pop'n controller)
  //PS2_AlwaysInput(PS2_LEFT|PS2_DOWN|PS2_RIGHT);

  /* PS2 Init */
  PS2_Init();
}

void loop() {

  button_state = 0;
  if (digitalRead(A0) == LOW) {
    button_state |= BUTTON_1;
  }
  if (digitalRead(A1) == LOW) {
    button_state |= BUTTON_2;
  }
  if (digitalRead(A2) == LOW) {
    button_state |= BUTTON_3;
  }
  if (digitalRead(A3) == LOW) {
    button_state |= BUTTON_4;
  }
  if (digitalRead(A4) == LOW) {
    button_state |= BUTTON_5;
  }

  PS2_Task();
  
}
