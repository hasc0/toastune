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

#define DEBOUNCE 100

typedef enum {
  LIGHT,
  MEDIUM,
  DARK
} toast_t;

typedef enum {
  IDLE,
  RUN,
  END
} state_t;

typedef enum {
  EV_MODE,
  EV_START,
  EV_END,
  EV_RESET
} event_t;

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET);
Servo servo;

volatile toast_t mode = LIGHT;
volatile state_t prevState = IDLE_LIGHT;
volatile state_t state = IDLE_LIGHT;
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

          event_t ev = EV_MODE;
          xQueueSend(eventQueue, &ev, 0):
        }
      }
    }

    lastMode = readMode;

    bool readStart = digitalRead(PIN_START);

    if (readStart != lastStart) {
      if (now - startChange > DEBOUNCE) {
        lastStart = readStart;
        startChange = now;

        if (startChange == LOW) {
          switch (state) {
            case IDLE:
              event_t ev = EV_START;
              break;
            case RUN:
              event_t ev = EV_RESET;
              break;
          }

          xQueueSend(eventQueue, &ev, 0):
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

    float temp = temperatureMonitor;
    String tempOut = "Temp: " + String(temp) + " C";

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Yes, toast...");
    display.setCursor(0, 20);
    display.println(modeOut);
    display.setCursor(0, 30);
    display.println(tempOut);

    uint32_t now = millis();
    uint32_t start;

    if (prevState == IDLE && state == RUN) {
      start = now;
      prevState = RUN;
    }

    string timeOut = "Time: ";

    switch (state) {
      case IDLE:
        timeOut += "Idle";
        break;
      case RUN:
        timeOut += String((now - start) / 1000);
        timeOut += " s";
        break;
      case END:
        timeOut += "Done";
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
    if (xQueueReceieve(eventQueue, &ev, portMAX_DELAY) == pdPASS) {
      switch (ev) {
        case EV_MODE:
          break;
        case EV_START:
          break;
        case EV_END:
          break;
        case EV_RESET:
          break;
      }
    }
  }
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

  eventQueue = xQueueCreate(10, sizeof(event_t));

  xTaskCreate(toastService, "Toast Service", 4096, NULL, 3, NULL, 1);
  xTaskCreate(inputHandler, "Input Handler", 4096, NULL, 2, NULL, 1);
  xTaskCreate(displayManager, "Display Manager", 4096, NULL, 1, NULL, 1);
}

void loop() {}

