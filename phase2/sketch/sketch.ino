#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define PIN_START 2
#define PIN_MODE  3
#define PIN_SERVO 10
#define PIN_THERM A0

#define AREF 5.0
#define ADC_RES 10

#define DISPLAY_ADDR   0x3C
#define DISPLAY_WIDTH  128
#define DISPLAY_HEIGHT 64
#define DISPLAY_RESET  -1

#define L_TEMP 200
#define M_TEMP 220
#define D_TEMP 240

#define DEBOUNCE 200

typedef enum {
  LIGHT,
  MEDIUM,
  DARK
} toast_t;

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET);
Servo servo;

volatile toast_t mode = LIGHT;

float getVoltage(int rawValue) {
  return rawValue * (AREF / (pow(2, ADC_RES) - 1));
}

float getTemperature(float voltage) {
  return (voltage - 1.25) / 0.005;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDR)) {
    Serial.println("Display failed to initialize.");
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Toastune v0.1");
  display.setCursor(0, 20);
  display.println("CSE 321 Term Project");
  display.setCursor(0, 40);
  display.println("Harper Scott");
  display.display();
  delay(2000);

  servo.attach(PIN_SERVO);

  servo.write(0);
  delay(500);
  servo.write(90);
  delay(500);
  servo.write(180);
  delay(500);
  servo.write(90);
  delay(500);
}

void loop() {
  if (digitalRead(PIN_MODE) == LOW) {
    delay(DEBOUNCE);

    if (mode == LIGHT) {
      mode = MEDIUM;
    } else if (mode == MEDIUM) {
      mode = DARK;
    } else if (mode == DARK) {
      mode = LIGHT;
    }
  }

  String modeOut;
  if (mode == LIGHT) {
    modeOut = "Mode: Light";
  } else if (mode == MEDIUM) {
    modeOut = "Mode: Medium";
  } else if (mode == DARK) {
    modeOut = "Mode: Dark";
  }

  int rawValue = analogRead(PIN_THERM);
  float voltage = getVoltage(rawValue);
  float temp = getTemperature(voltage);

  String tempOut = "Temp: " + String(temp) + " C";

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Yes, toast...");
  display.setCursor(0, 20);
  display.println(modeOut);
  display.setCursor(0, 30);
  display.println(tempOut);
  display.display();
  delay(500);
}
