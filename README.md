# toastune
This project should result in repeatable, ideal toast (regardless of the toaster).

## Requirements
### Hardware
- Arduino UNO Rev3
- Breadboard
- 128x64 SSD1306 OLED IIC Display
- FS90R Continuous Rotation Servo
- Adafruit AD8495
- Type-K Thermocouple
- 18x Jumper Wire
- 2x Push Button

### Software
- Arduino CLI
- Visual Studio Code
- Arduino Community Edition Extension

> [!NOTE]
> This project can be built using the Arduino IDE, but was developed using the software listed here.

### Libraries
- `Arduino.h`
- `U8x8lib.h`
- `Servo.h`

## Setup
### Hardware
- Connect all peripheral ground pins to an Arduino ground pin.
- Connect all peripheral power pins to an Arduino 5V pin.
- Connect the display SCL and SDA pins to the Arduino SCL and SDA pins.
- Connect the servo input pin to digital pin 10 on the Arduino.
- Connect the start button output pin to digital pin 2 on the Arduino.
- Connect the mode button output pin to digital pin 3 on the Arduino.
- Connect the AD8495 output pin to analog pin 0 on the Arduino.

### Software
- Build and upload the sketch to the Arduino (located at `toastune/phase2/sketch/sketch.ino`).

## Usage
1. Position and mount the servo on a toaster such that the servo can actuate the button that cancels the toasting process.
2. Power the Arduino via USB and wait for the display to show the current temperature.
3. Insert the thermocouple into a slice of bread such that the surface temperature is being monitored.
4. Use the button connected to digital pin 3 to cycle through toast darkness modes.
5. Once the toaster is started, press the button connected to digital pin 2 to start the system.
6. Wait for the servo to actuate and enjoy the resulting toast.

> [!NOTE]
> This was created for the CSE321 Fall 2025 term project.

