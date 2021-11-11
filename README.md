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
* **7.6V:** This typically powers the rumble motors, but we can use it to power the arduino (see `Powering the arduino` in additional notes section).
* **GND:** The common ground for the console, also known as 0V.
* **3.3V:** This is power coming from the console. We can use it to power the arduino (see `Powering the arduino` in additional notes section).
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

* ACK (see `ps2.c`)

## Basic Use

### setup()

- Prepare your input pins by setting them as INPUT_PULLUP

- Call the `PS2_MapInput(uint16_t *input, uint16_t mask, PS2_INPUT buttons)` function multiple times to setup your button mapping. This will create the link between one bit of your input bitfield (via the mask) and one or several PS2_INPUT buttons

- (optional) You may also call `PS2_AlwaysInput(PS2_INPUT buttons)` to add constantly pressed inputs to the mapping if needed (some dedicated game controllers work this way)

- Call `PS2_Init()`

Valid values for PS2_INPUT buttons are listed in ps2.h :

```
PS2_SELECT  
PS2_L3      
PS2_R3      
PS2_START   
PS2_UP      
PS2_RIGHT   
PS2_DOWN    
PS2_LEFT    
PS2_L2      
PS2_R2      
PS2_L1      
PS2_R1      
PS2_TRIANGLE
PS2_CIRCLE  
PS2_CROSS   
PS2_SQUARE  
```

### loop()

- To set a button, `BITWISE OR` the correct define with `button_state`. A button is released if it is not pressed during the loop

- Call `PS2_Task()` to poll the console and update inputs

### Example

The sketch example includes D-Pad controls and start button, connected to `A0`, `A1`, `A2`, `A3` and `A4`.

### Additional Notes

#### Changing the ACK Pin

At the top of the `ps2.c` file will be a section to change the ACK pin location.

```
/* USER CUSTOMIZABLE SETTINGS */
#define ACK_PORT  PORTB
#define ACK_DDR   DDRB
#define ACK_PIN   4     // PB4 (Pin 8 on Micro)
```

The pin number refers to the `PORT` number, not the standard Arduino number. Refer to the purple tags in the image above.

#### Powering the Arduino

There are several **mutually exclusive** ways to power the arduino :

- Use an USB cable with a wall charger (this unfortunately won't work via the PS2 usb port)
- Wire the 7.6 rumble motor line to the arduino `Vin` pin
- Wire the 3.3V line to the arduino `5v` pin (yes, 5v pin, not a typo)

All of these ways are mutually exclusive. **DO NOT** wire multiple power sources at the same time, this can fry your console.

#### Bypassing the Transistors

For safety reasons, I suggest using the transistors as shown above to prevent backfeeding voltage into your console. This has the potential to cause harm. 

If you do not use the transistors and directly wire your Arduino to the console, the following line must be changed at the top of `ps2.c`

```
#define INVERT_CIPO 1  // Set to 1 if CIPO is open-drain via transistor (recommended)
#define INVERT_ACK  1  // Set to 1 if ACK is open-drain via transistor (recommended)
```

to

```
#define INVERT_CIPO 0  // Set to 1 if CIPO is open-drain via transistor (recommended)
#define INVERT_ACK  0  // Set to 1 if ACK is open-drain via transistor (recommended)
```

***WHILE THIS HAS BEEN TESTED USING THE 3.3V LINE TO 5V PIN, I DO NOT RECOMMEND IT, I AM NOT LIABLE FOR ANY DAMAGE CAUSED TO YOUR PLAYSTATION.***

## Credits

Wiring and protocol information from [Curious Inventor's page on the Playstation Controller](https://store.curiousinventor.com/guides/PS2)

Code base adapted from [progmem's USBemani v3](https://github.com/progmem/re-usbemani/).

Discord User GoroKaneda for getting me to work on and document this, as well as additional testing.