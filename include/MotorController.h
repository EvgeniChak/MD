#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include <BasicStepperDriver.h>
#include <EncButton.h>

class MotorController {
public:
  MotorController(uint8_t dirPin, uint8_t stepPin, uint8_t sleepPin,
                  uint8_t ccwPin, uint8_t cwPin);

  void begin();
  void update();
  void processCommand(String command);

  void calibrate();
  void calibrateMeasure();
  void moveToPosition(uint8_t percent);
  void moveSteps(int32_t steps);
  void stop();

private:
  static const uint16_t MOTOR_STEPS = 200;
  static const uint8_t MICROSTEPS = 32;
  static const uint32_t TOTAL_STEPS = 96000;
  static const uint8_t RPM = 10;

  uint8_t dirPin;
  uint8_t stepPin;
  uint8_t sleepPin;

  BasicStepperDriver motor;
  Button cwBtn;
  Button ccwBtn;

  bool isMoving;
  int32_t currentPosition;
  int32_t targetPosition;
  int32_t stepDifference;
  int8_t direction;
  uint32_t totalSteps;

  void startMotor();
  void stopMotor();
  bool checkButtons();
  void calibrateReverse();
};

#endif // MOTOR_CONTROLLER_H