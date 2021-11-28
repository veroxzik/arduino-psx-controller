#include <arduino_psx.h>

void setup() {
    // Setup PSX
    PSX.init(8);

    // Setup buttons
    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(A3, INPUT_PULLUP);
    pinMode(A4, INPUT_PULLUP);
    pinMode(A5, INPUT_PULLUP);
}

void loop() {

    // Set buttons
    PSX.setButton(PS_INPUT::PS_LEFT,    digitalRead(A0) == LOW);
    PSX.setButton(PS_INPUT::PS_DOWN,    digitalRead(A1) == LOW);
    PSX.setButton(PS_INPUT::PS_UP,      digitalRead(A2) == LOW);
    PSX.setButton(PS_INPUT::PS_RIGHT,   digitalRead(A3) == LOW);
    PSX.setButton(PS_INPUT::PS_CIRCLE,  digitalRead(A4) == LOW);
    PSX.setButton(PS_INPUT::PS_CROSS,   digitalRead(A5) == LOW);

    // Send info
    PSX.send();

}