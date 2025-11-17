#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <U8x8lib.h>
#include <Wire.h>
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

typedef enum {
  EV_MODE,
  EV_START,
  EV_RESET
} event_t;

U8X8_SSD1306_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE);
Servo servo;

volatile toast_t mode = LIGHT;
volatile state_t prevState = IDLE;
volatile state_t state = IDLE;
QueueHandle_t eventQueue;

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

void inputHandler(void *pv) {
  bool lastMode = HIGH;
  bool lastStart = HIGH;

  uint32_t modeChange = 0;
  uint32_t startChange = 0;

  for (;;) {
    uint32_t now = millis();

    bool readMode = digitalRead(PIN_MODE);

    if (readMode != lastMode) {
      if (now - modeChange > DEBOUNCE) {
        lastMode = readMode;
        modeChange = now;

        if (modeChange == LOW) {
          event_t ev = EV_MODE;
          xQueueSend(eventQueue, &ev, 0);
        }
      }
    }

    lastMode = readMode;

    bool readStart = digitalRead(PIN_START);

    if (readStart != lastStart) {
      if (now - startChange > DEBOUNCE) {
        lastStart = readStart;
        startChange = now;

        event_t ev;
        if (startChange == LOW) {
          switch (state) {
            case IDLE:
              ev = EV_START;
              break;
            case RUN:
              ev = EV_RESET;
              break;
          }

          xQueueSend(eventQueue, &ev, 0);
        }
      }
    }

    lastStart = readStart;
  }
}

void displayManager(void *pv) {
  for (;;) {
    String modeOut;

    if (mode == LIGHT) {
      modeOut = "Mode: Light";
    } else if (mode == MEDIUM) {
      modeOut = "Mode: Medium";
    } else if (mode == DARK) {
      modeOut = "Mode: Dark";
    }

    float temp = temperatureMonitor();
    String tempOut = "Temp: " + String(temp) + " C";

    display.setFont(u8x8_font_victoriabold8_r);
    display.drawString(0, 0, "Yes, toast...");
    display.drawString(0, 20, modeOut.c_str());
    display.drawString(0, 30, tempOut.c_str());

    uint32_t now = millis();
    uint32_t start;

    if (prevState == IDLE && state == RUN) {
      start = now;
      prevState = RUN;
    }

    String timeOut = "Time: ";

    switch (state) {
      case IDLE:
        timeOut += "Idle";
        break;
      case RUN:
        timeOut += String((now - start) / 1000);
        timeOut += " s";
        break;
    }

    display.setCursor(0, 40);
    display.println(timeOut);

    display.display();
    delay(500);
  }
}

void toastService(void *pv) {
  event_t ev;

  int target;
  if (mode == LIGHT) {
    target = L_TEMP;
  } else if (mode == MEDIUM) {
    target = M_TEMP;
  } else {
    target = D_TEMP;
  }

  for (;;) {
    if (xQueueReceive(eventQueue, &ev, portMAX_DELAY) == pdPASS) {
      switch (ev) {
        case EV_MODE:
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

          break;
        case EV_START:
          prevState = state;
          state = RUN;
          break;
        case EV_RESET:
          prevState = state;
          state = IDLE;
          break;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);

  display.begin();

  display.setFont(u8x8_font_victoriabold8_r);
  display.drawString(0, 0, "Toastune v0.1");
  display.drawString(0, 20, "CSE 321 Project");
  display.drawString(0, 30, "Harper Scott");

  servo.attach(PIN_SERVO);

  delay(500);
  servo.write(180);
  delay(500);
  servo.write(90);
  delay(500);

  eventQueue = xQueueCreate(5, sizeof(event_t));

  xTaskCreate(toastService, "Toast Service", 1024, NULL, 3, NULL);
  xTaskCreate(inputHandler, "Input Handler", 1024, NULL, 2, NULL);
  xTaskCreate(displayManager, "Display Manager", 1024, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {}

