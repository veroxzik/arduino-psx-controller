# Arduino PSX Controller

This is an Arduino library used to emulate a Playstation 1/2 controller, exclusively for the Leonardo / Micro / Pro Micro.

### Caveats

1. This library will only function in digital mode. That means no joysticks and no pressure sensitive triggers or buttons.

## Materials Required

It is **highly** recommended that you use an [Arduino Micro](https://store.arduino.cc/products/arduino-micro) or [comparable clone](https://www.aliexpress.com/item/1893729784.html). This is due to the fact that all necessary pins are easily broken out. Using a Leonardo or Pro Micro will require you to solder onto pre-existing LEDs, which can be difficult.

You will also need qty 1 (one) NPN signal transistor. I used 2N2222A, but many others will work. A similar N-Channel MOSFET, such as the 2N7000 will also work.

Lastly,  you will need a cable to plug into your Playstation. Options include:
1. Cutting the cord from an existing controller
2. Buying an extension cable and using the male end
3. A pre-terminated cable [such as this one](https://www.aliexpress.com/item/32826516802.html)

## Playstation Wiring

Let's get acquainted with the Playstation cable and color codes. It is always a good idea to verify that these colors match the cable you own. Some third party or cheap cables may vary.

![Playstation Cable](/images/psx-cable-colors.png)

* **CIPO:** (Controller-In / Peripheral-Out) This is data from the controller to the console. It is held HIGH via a pull-up resistor inside the console. We will use the transistor to pull this line to GND. Also known as MISO.
* **COPI:** (Controller-Out / Peripheral-In) This is the data from the console to the controller. Also known as MOSI.
* **7.6V:** This provides power for the rumble motors. See [Powering the Arduino](#powering-the-arduino).
* **GND:** The common ground for the console, also known as 0V.
* **3.3V:** This is power coming from the console, which typically powers the controller processor. See [Powering the Arduino](#powering-the-arduino).
* **CS:** (Chip Select) This line goes LOW when the console is requesting data from a controller. This is how the console selects which player's controller to read from. Also known as SS.
* **N/C:** This wire is not used (not connected).
* **ACK:** This is how the controller tells the console it has finished sending data. It is held HIGH via a pull-up resistor inside the console.

## Arduino Wiring

The example program follows the wiring below. Please refer to your specific transistor datasheet for its pinout.

![Arduino Micro Wiring](/images/micro-psx-wiring.png)

### Pins that cannot be changed

* CIPO
* COPI
* CS
* SCK

### Pins that can be changed

* ACK

### Power Pins

* VIN
* 5V
* GND

## Powering the Arduino

There's a few options you have for powering the arduino.

### Option 1: (Recommended!) 7.6V to VIN

Connect 7.6V to VIN on the Arduino. This will power the Arduino off the rumble line and at 5V. It is safe to plug in the USB connector at the same time using this method.

### Option 2: USB Only

Disconnect both 7.6V and 3.3V from the Arduino and only use the USB connection. USB will need to be plugged in any time you wish to use the controller.

### Option 3: Playstation to 3.3V

Disconnect 7.6V and connect 3.3V from the Playstation to the 5V pin on your Arduino. This will force the chip to run at 3.3V. 

**DO NOT PLUG IN USB WHEN CONNECTED LIKE THIS** as it can damage the Playstation or your PC.

## Installation Instructions:

1. Download: https://github.com/veroxzik/arduino-psx-controller/archive/refs/heads/main.zip
2. In the Arduino IDE, select `Sketch` > `Include Library` > `Add .ZIP Library...`. Browse to where the downloaded ZIP file is located and click Open. The example will now appear under `File` > `Examples` > `Arduino-PSX`.

## Basic Use

### Initialization

Include the header at the top of your code. This will automatically create an instance of the `PSX` object.

```
#include <arduino_psx.h>
```

Next, run the initialization function inside `setup`:

```
void setup() {
    PSX.init(8);
}
```

The `init` function takes up to 3 arguments.

1. `ackPin`: This is the pin that the ACK line is attached to. Use the digital pin number, shown in the diagram above as the `D` numbers. 
    * The default is `8`.
2. `invertACK`: This allows you to invert the logic for the ACK pin. Set this to `false` when connected directly to the Arduino and use `true` when connected via a transistor. 
    * The default is `false`.
3. `invertCIPO`: This allows you to invert the logic for the CIPO pin. Set this to `false` when connected directly to the Arduino and use `true` when connected via a transistor. Please see [Bypassing the Transistor](#bypassing-the-transistor)
    * The default is `true`.

#### Always Held Buttons

For certain types of controllers (Pop'n Music and Guitar Hero come to mind), the game expects certain buttons to *always* be pressed.

To do so, run the method `setAlwaysHeld` using the button enums `BITWISE OR`'d together.

```
void setup() {
    PSX.init(8);
    PSX.setAlwaysHeld(PS_INPUT::PS_LEFT | PS_INPUT::PS_DOWN | PS_INPUT::PS_RIGHT);
}
```

### Setting Buttons

Buttons can be set easily using the `setButton` method.

Set a button to `true` to press it and `false` to release it.

```
    // Press left button
    PSX.setButton(PS_INPUT::PS_LEFT, true);

    // Release left button
    PSX.setButton(PS_INPUT::PS_LEFT, false);
```

Valid options are as follows:
```
PS_INPUT::PS_SELECT  
PS_INPUT::PS_L3      
PS_INPUT::PS_R3      
PS_INPUT::PS_START   
PS_INPUT::PS_UP      
PS_INPUT::PS_RIGHT   
PS_INPUT::PS_DOWN    
PS_INPUT::PS_LEFT    
PS_INPUT::PS_L2      
PS_INPUT::PS_R2      
PS_INPUT::PS_L1       
PS_INPUT::PS_R1       
PS_INPUT::PS_TRIANGLE 
PS_INPUT::PS_CIRCLE   
PS_INPUT::PS_CROSS    
PS_INPUT::PS_SQUARE   
```

## Example

A basic example is included. Navigate to the Example sketches in the Arduino IDE to run it.

The example includes D-Pad controls, as well as O and X. Connect the following pins to GND to press the corresponding button:
|Pin|Button|
|:---:|:---:|
|A0|LEFT|
|A1|DOWN|
|A2|UP|
|A3|RIGHT|
|A4|O|
|A5|X|

## Additional Notes

### Bypassing the Transistor

For safety and performance reasons, I suggest using the transistor as shown above to prevent backfeeding voltage into your console. This has the potential to cause harm. 

***It has been noted by myself and others that bypassing the use of a transistor on the CIPO line may result in poor performance or corrupted data. It is not a software issue, but a result of electrical characteristics related to the Arduino, the length and impedance of your wires, and other factors. I will not be able to troubleshoot your situation if you go this route.***

## Credits

Some code used from [watterott's Arduino-Libs](https://github.com/watterott/Arduino-Libs). Specific lines are noted.

```
Copyright (c) 2011-2020 Watterott electronic (www.watterott.com)
All rights reserved.
```

Wiring and protocol information from [Curious Inventor's page on the Playstation Controller](https://store.curiousinventor.com/guides/PS2)

Code base adapted from [CrazyRedMachine's Ultiamte Pop'n Controller](https://github.com/CrazyRedMachine/UltimatePopnController/tree/PSX), which itself is based on [busslave's PSX_RECEIVER.cpp](https://nfggames.com/forum2/index.php?topic=5001.0).

Discord User GoroKaneda for getting me to work on and document this, as well as additional testing.

Additional suggestions and feedback from [progmem](https://github.com/progmem).

## Donate

If you appreciate this library or the other projects I do, you can make a monetary donation below. Thanks!

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/donate/?business=6M7ENNVE2ZP5Q&no_recurring=1&currency_code=USD)