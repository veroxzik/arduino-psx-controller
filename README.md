# Arduino PSX Controller
This project is a short of example on how to use an Arduino board as a controller for the Playstation 1 or 2.

### Caveats

1. This library will only function in digital mode. That means no joysticks and no pressure sensitive triggers or buttons.

## Materials Required

It is **highly** recommended that you use an [Arduino Micro](https://store.arduino.cc/products/arduino-micro) or [comparable clone](https://www.aliexpress.com/item/1893729784.html). This is due to the fact that all necessary pins are easily broken out. Using a Leonardo or Pro Micro will require you to solder onto pre-existing LEDs.

You will also need qty 2 (two) NPN signal transistors. I used 2N2222A, but many others will work.

Lastly,  you will need a cable to plug into your Playstation. Options include:
1. Cutting the cord from an existing controller
2. Buying an extension cable and using the male end
3. A pre-terminated cable [such as this one](https://www.aliexpress.com/item/32826516802.html)

## Playstation Wiring

Let's get acquainted with the Playstation cable and color codes. It is always a good idea to verify that these colors match the cable you own. Some third party or cheap cables may vary.

![Playstation Cable](/images/psx-cable-colors.png)

* **CIPO:** (Controller-In / Peripheral-Out) This is data from the controller to the console. It is held HIGH via a pull-up resistor inside the console. We will use one of the transistors to pull this line to GND. Also known as MISO.
* **COPI:** (Controller-Out / Peripheral-In) This is the data from the console to the controller. Also known as MOSI.
* **7.6V:** This typically powers the rumble motors, but we can use it to power the arduino.
* **GND:** The common ground for the console, also known as 0V.
* **3.3V:** This is power coming from the console. We will not be using this.
* **CS:** (Chip Select) This line goes LOW when the console is requesting data from a controller. This is how the console selects which player's controller to read from. Also known as SS.
* **N/C:** This wire is not used (not connected).
* **ACK:** This is how the controller tells the console it has finished sending data. It is held HIGH via a pull-up resistor inside the console. The second transistor will pull this line to GND.

## Arduino Wiring

The example program follows the wiring below. Please refer to your specific transistor datasheet for its pinout.

![Arduino Micro Wiring](/images/micro-psx-wiring.png)

### Pins that cannot be changed

* CIPO
* COPI
* 7.6V
* GND
* CS

### Pins that can be changed

* ACK

## Basic Use

The example includes D-Pad controls. Connect `A0`, `A1`, `A2`, or `A3`.

```
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
```

To set a button, `BITWISE OR` the correct define with `button_state`. A button is released if it is not pressed during the loop.

Valid defines are:

```
PS_SELECT  
PS_L3      
PS_R3      
PS_START   
PS_UP      
PS_RIGHT   
PS_DOWN    
PS_LEFT    
PS_L2      
PS_R2      
PS_L1       
PS_R1       
PS_TRIANGLE 
PS_CIRCLE   
PS_CROSS    
PS_SQUARE   
```

### Additional Notes

#### Changing the ACK Pin

At the top of the `*.ino` file will be a section to change the ACK pin location.

```
/* USER CUSTOMIZABLE SETTINGS */
#define ACK_PORT  PORTB
#define ACK_DDR   DDRB
#define ACK_PIN   4     // PB4 (Pin 8 on Micro)
```

The pin number refers to the `PORT` number, not the standard Arduino number. Refer to the purple tags in the image above.

#### Bypassing the Transistors

For safety reasons, I suggest using the transistors as shown above to prevent backfeeding voltage into your console. This has the potential to cause harm. 

If you do not use the transistors and directly wire your Arduino to the console, the following line must be changed

```
#define INVERT_OUTPUT 1   // Set to 1 if CIPO and ACK are open-drain via transistors
```

to

```
#define INVERT_OUTPUT 0   // Set to 1 if CIPO and ACK are open-drain via transistors
```

***THIS IS NOT TESTED AND I DO NOT RECOMMEND IT, I AM NOT LIABLE FOR ANY DAMAGE CAUSED TO YOUR PLAYSTATION.***

## Credits

Wiring and protocol information from [Curious Inventor's page on the Playstation Controller](https://store.curiousinventor.com/guides/PS2)

Code base adapted from [CrazyRedMachine's Ultiamte Pop'n Controller](https://github.com/CrazyRedMachine/UltimatePopnController/tree/PSX), which itself is based on [busslave's PSX_RECEIVER.cpp](https://nfggames.com/forum2/index.php?topic=5001.0).

Discord User GoroKaneda for getting me to work on and document this, as well as additional testing.