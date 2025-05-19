#include "MotorController.h"
#include <Arduino.h>
#include <Wire.h>

#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)
#define DEBUG_WITH_TEXT(x, text1, text2)                                       \
  Serial.println(String(text1) + x + String(text2))
#else
#define DEBUG(x)
#define DEBUGLN(x)
#define DEBUG_WITH_TEXT(x, text1, text2)
#endif

#define DIR 12
#define STEP 11
#define SLEEP 10

#define CCW_PIN 3
#define CW_PIN 2

#define BUFFER_SIZE 32
char i2cBuffer[BUFFER_SIZE];
int i2cBufferIndex = 0;
bool newCommandReceived = false;

MotorController motorController(DIR, STEP, SLEEP, CCW_PIN, CW_PIN);

void receiveEvent(int howMany) {
  i2cBufferIndex = 0;
  while (Wire.available() && i2cBufferIndex < BUFFER_SIZE - 1) {
    i2cBuffer[i2cBufferIndex] = Wire.read();
    i2cBufferIndex++;
  }
  i2cBuffer[i2cBufferIndex] = '\0';
  newCommandReceived = true;
}

void processI2CCommand() {
  if (newCommandReceived) {
    String command = String(i2cBuffer);
    command.trim();
    motorController.processCommand(command);
    newCommandReceived = false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // i2c initt
  Wire.begin(0x08);
  Wire.onReceive(receiveEvent);

  motorController.begin();

  DEBUGLN("start");
  // motorController.calibrate();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    DEBUG_WITH_TEXT(command, "Current command: ", "");
    motorController.processCommand(command);
  }

  processI2CCommand();

  motorController.update();
}
