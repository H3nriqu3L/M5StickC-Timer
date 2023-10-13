#include <M5StickC.h>
#include "esp32-hal-timer.h"

volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int timer_secondsX10 = 0;
float accX = 0;
float accY = 0;
float accZ = 0;
bool isRunning = false;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);

  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(5, 0, 2);
  M5.Lcd.printf("CRONOMETRO");
  M5.Lcd.setTextSize(2);
  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_BUTTON_RST, INPUT);
  delay(100);
}

void loop() {
  M5.Lcd.setCursor(60, 30, 2);
  
  if (isRunning) {
    if (interruptCounter > 0) {
      portENTER_CRITICAL(&timerMux);
      interruptCounter--;
      portEXIT_CRITICAL(&timerMux);
      totalInterruptCounter++;
    }
    timer_secondsX10++;
  }

  int rem_seconds = (timer_secondsX10 / 10) % 60;
  int minutes = (timer_secondsX10 / 10) / 60;
  M5.Lcd.printf("%02d:%02d", minutes, rem_seconds);

  if (digitalRead(M5_BUTTON_HOME) == LOW) {
    isRunning = !isRunning; // Inicia ou pausa o cronômetro
    delay(200);  // Atraso para evitar pressionamento duplo
  } else if (digitalRead(M5_BUTTON_RST) == LOW) {
    // Reinicia o cronômetro
    timer_secondsX10 = 0;
    isRunning = false;
  }

  delay(100);
}
