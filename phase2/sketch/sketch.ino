#include <Arduino.h>
#include <U8x8lib.h>
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

#define L_TEMP 155.0
#define M_TEMP 170.0
#define D_TEMP 185.0

#define DEBOUNCE 100

typedef enum {
  LIGHT,
  MEDIUM,
  DARK
} toast_t;

typedef enum {
  IDLE,
  RUN
} state_t;

U8X8_SSD1306_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE);
Servo servo;

volatile toast_t mode = LIGHT;
volatile state_t state = IDLE;

volatile bool lastMode = HIGH;
volatile bool lastStart = HIGH;

float getVoltage(int rawValue) {
  return rawValue * (AREF / (pow(2, ADC_RES) - 1));
}

float getTemperature(float voltage) {
  return (voltage - 1.25) / 0.005;
}

float temperatureMonitor() {
  int rawValue = analogRead(PIN_THERM);
  float voltage = getVoltage(rawValue);
  float temp = getTemperature(voltage);
  
  return temp;
}

void displayOutput() {
  String modeOut = "Mode: ";

  if (mode == LIGHT) {
    modeOut += "Light ";
  } else if (mode == MEDIUM) {
    modeOut += "Medium";
  } else if (mode == DARK) {
    modeOut += "Dark  ";
  }

  float temp = temperatureMonitor();
  String tempOut = "Temp: " + String(temp) + " C   ";

  display.setFont(u8x8_font_victoriabold8_r);
  display.drawString(0, 0, "Yes, toast...");
  display.drawString(0, 10, modeOut.c_str());
  display.drawString(0, 20, tempOut.c_str());

  switch (state) {
    case IDLE:
      display.drawString(0, 30, "Case: Idle");
      break;
    case RUN:
      display.drawString(0, 30, "Case: Run ");
      break;
  }


  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);

  display.begin();

  display.setFont(u8x8_font_victoriabold8_r);
  display.drawString(0, 0, "Toastune v0.2");
  display.drawString(0, 20, "CSE 321 Project");
  display.drawString(0, 30, "Harper Scott");

  servo.attach(PIN_SERVO);

  servo.write(180);
  delay(500);
  servo.write(90);

  delay(500);
  display.clearDisplay();
}

void loop() {
  uint32_t modeChange = 0;
  uint32_t startChange = 0;

  uint32_t now = millis();

  bool readMode = digitalRead(PIN_MODE);

  if (readMode != lastMode) {
    if (now - modeChange > DEBOUNCE) {
      modeChange = now;

      if (readMode == LOW) {
        switch (mode) {
          case LIGHT:
            mode = MEDIUM;
            break;
          case MEDIUM:
            mode = DARK;
            break;
          case DARK:
            mode = LIGHT;
            break;
        }
      }
    }
  }

  lastMode = readMode;

  bool readStart = digitalRead(PIN_START);

  if (readStart != lastStart) {
    if (now - startChange > DEBOUNCE) {
      startChange = now;

      if (readStart == LOW) {
        switch (state) {
          case IDLE:
            state = RUN;
            break;
          case RUN:
            state = IDLE;
            break;
        }
      }
    }
  }

  lastStart = readStart;

  float target;
  if (mode == LIGHT) {
    target = L_TEMP;
  } else if (mode == MEDIUM) {
    target = M_TEMP;
  } else if (mode == DARK) {
    target = D_TEMP;
  }

  if (state == RUN) {
    float temp = temperatureMonitor();
    while (temp < target) {
      displayOutput();
      temp = temperatureMonitor();
    }

    servo.write(180);
    delay(1000);
    servo.write(90);

    state = IDLE;
  }

  displayOutput();
}

