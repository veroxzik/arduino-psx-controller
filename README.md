# Arduino PSX Controller
This project is a short of example on how to use an Arduino board as a controller for the Playstation 1 or 2.

### Caveats

1. This library will only function in digital mode. That means no joysticks and no pressure sensitive triggers or buttons.

## Materials Required

It is **highly** recommended that you use an [Arduino Micro](https://store.arduino.cc/products/arduino-micro) or [comparable clone](https://www.aliexpress.com/item/1893729784.html). This is due to the fact that all necessary pins are easily broken out. Using a Leonardo or Pro Micro will require you to solder onto pre-existing LEDs, which can be difficult.

You will also need qty 2 (two) NPN signal transistors. I used 2N2222A, but many others will work. A similar N-Channel MOSFET, such as the 2N7000 will also work.

Lastly,  you will need a cable to plug into your Playstation. Options include:
1. Cutting the cord from an existing controller
2. Buying an extension cable and using the male end
3. A pre-terminated cable [such as this one](https://www.aliexpress.com/item/32826516802.html)

## Playstation Wiring

Let's get acquainted with the Playstation cable and color codes. It is always a good idea to verify that these colors match the cable you own. Some third party or cheap cables may vary.

![Playstation Cable](/images/psx-cable-colors.png)

* **CIPO:** (Controller-In / Peripheral-Out) This is data from the controller to the console. It is held HIGH via a pull-up resistor inside the console. We will use one of the transistors to pull this line to GND. Also known as MISO.
* **COPI:** (Controller-Out / Peripheral-In) This is the data from the console to the controller. Also known as MOSI.
* **7.6V:** This provides power for the rumble motors. See [Powering the Arduino](#powering-the-arduino).
* **GND:** The common ground for the console, also known as 0V.
* **3.3V:** This is power coming from the console, which typically powers the controller processor. See [Powering the Arduino](#powering-the-arduino).
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

## Powering the Arduino

There's a few options you have for powering the arduino.

### Option 1 (Recommended): 7.6V

Connect 7.6V to VIN on the Arduino, as shown in the diagram above. This will power the Arduino off the rumble line and at 5V. It is safe to plug in the USB connector at the same time using this method.

### Option 2: USB Only

Disconnect both 7.6V and 3.3V from the Arduino and only use the USB connection. USB will need to be plugged in any time you wish to use the controller.

### Option 3: Playstation to 3.3V

Disconnect 7.6V and connect 3.3V from the Playstation to the 5V pin on your Arduino. This will force the chip to run at 3.3V. 

**DO NOT PLUG IN USB WHEN CONNECTED LIKE THIS** as it can damage the Playstation or your PC.

## Basic Use

The example includes D-Pad controls, as well as X and O. Connect `A0`, `A1`, `A2`, `A3`, `A4`, or `A5` to GND to try it out.

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
if (digitalRead(A4) == LOW) {
button_state |= PS_CIRCLE;
}
if (digitalRead(A5) == LOW) {
button_state |= PS_CROSS;
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

### Always On Buttons

For certain types of controllers (Pop'n Music and Guitar Hero come to mind), the game expects certain buttons to *always* be pressed.

To do so, change the following line from above:

```
button_state = 0;
```

to include the necessary buttons. In this example, Left, Down, and Right are being held permanently:

```
button_state = PS_LEFT | PS_DOWN | PS_RIGHT;
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

For safety and performance reasons, I suggest using the transistors as shown above to prevent backfeeding voltage into your console. This has the potential to cause harm. 

***It has been noted by myself and others that bypassing the use of a transistor on the CIPO line may result in poor performance or corrupted data. It is not a software issue, but a result of electrical characteristics related to the Arduino, the length and impedance of your wires, and other factors. I will not be able to troubleshoot your situation if you go this route.***

If you do not use the transistors and directly wire your Arduino to the console, the following lines must be changed. CIPO and ACK can be change individually.

```
#define INVERT_CIPO 1   // Set to 1 if CIPO is open-drain via a transistor
#define INVERT_ACK  1   // Set to 1 if ACK is open-drain via a transistor
```

to

```
#define INVERT_CIPO 0   // Set to 1 if CIPO is open-drain via a transistor
#define INVERT_ACK  0   // Set to 1 if ACK is open-drain via a transistor
```

## Credits

Wiring and protocol information from [Curious Inventor's page on the Playstation Controller](https://store.curiousinventor.com/guides/PS2)

Code base adapted from [CrazyRedMachine's Ultiamte Pop'n Controller](https://github.com/CrazyRedMachine/UltimatePopnController/tree/PSX), which itself is based on [busslave's PSX_RECEIVER.cpp](https://nfggames.com/forum2/index.php?topic=5001.0).

Discord User GoroKaneda for getting me to work on and document this, as well as additional testing.