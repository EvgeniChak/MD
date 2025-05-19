#include "MotorController.h"

MotorController::MotorController(uint8_t dirPin, uint8_t stepPin,
                                 uint8_t sleepPin, uint8_t cwPin,
                                 uint8_t ccwPin)
    : dirPin(dirPin), stepPin(stepPin), sleepPin(sleepPin),
      motor(MOTOR_STEPS, dirPin, stepPin, sleepPin), ccwBtn(ccwPin),
      cwBtn(cwPin), isMoving(false), currentPosition(0),
      totalSteps(TOTAL_STEPS) {}

void MotorController::begin() {

  // motor init
  motor.begin(RPM, MICROSTEPS);
  motor.setSpeedProfile(motor.LINEAR_SPEED, 700, 350);
  stop();
  cwBtn.setBtnLevel(1);
  ccwBtn.setBtnLevel(1);
}

void MotorController::update() {
  if (isMoving) {
    cwBtn.tick();
    ccwBtn.tick();

    if (!motor.nextAction()) {
      stop();
    } else {
      currentPosition += direction;
    }
    checkButtons();
  }
}

void MotorController::processCommand(String command) {
  if (command.startsWith("motor")) {
    String s = command.substring(5);
    s.replace("(", "");
    s.replace(" ", "");
    s.replace(")", "");

    if (s == "calibrate") {
      calibrate();
      // } else if (s == "calibrate_measure") {
      //   calibrateMeasure();
    } else if (s == "down") {
      moveToPosition(100);
    } else if (s == "up") {
      moveToPosition(0);
    } else if (s.startsWith("m")) {
      moveToPosition(s.substring(1).toInt());
    } else if (s.startsWith("S")) {
      int32_t steps = s.substring(1).toInt();
      moveSteps(steps);
    } else if (s == "stop") {
      stop();
    }
  }
}

void MotorController::calibrate() {
  Serial.println("calibration");
  motor.setSpeedProfile(motor.CONSTANT_SPEED);
  motor.enable();
  bool keep_moving = true;

  Serial.println("start moving");
  while (keep_moving) {
    cwBtn.tick();
    ccwBtn.tick();

    motor.move(-5); // go up
    while (motor.getStepsRemaining() != 0) {
    }
    // microsMotor = micros();
    // while (micros() - microsMotor <= 10) {
    // }

    if (cwBtn.press() || cwBtn.click()) {
      motor.stop();
      motor.move(8000); // go to deadZone
      currentPosition = 0;
      Serial.println("ccwBtn pressed - stopping calibration");
      keep_moving = false;
    }
  }

  motor.disable();
  motor.setSpeedProfile(motor.LINEAR_SPEED, 700, 350);
  currentPosition = 0;
}

void MotorController::moveToPosition(uint8_t percent) {
  if (!checkButtons()) {
    Serial.println("checkButtons");
    if (percent >= 0 && percent <= 100) {
      targetPosition = map(percent, 0, 100, 0, totalSteps);
      stepDifference = targetPosition - currentPosition;

      startMotor();
      direction = stepDifference > 0 ? 1 : -1;
      motor.startMove(stepDifference);
      isMoving = true;
    }
  }
}

void MotorController::stop() {
  motor.stop();
  motor.disable();
  isMoving = false;
}

void MotorController::startMotor() {
  motor.enable();
  Serial.println("Motor started");
}

bool MotorController::checkButtons() {
  cwBtn.tick();
  ccwBtn.tick();

  if (cwBtn.press()) {
    Serial.println("cwBtn pressed");
    // startMotor();
    // motor.move(-8000);
    stop();
    return true;
  }

  else if (ccwBtn.press()) {
    Serial.println("ccwBtn pressed");
    // startMotor();
    //  motor.move(8000);
    stop();
    return true;
  }

  else
    return false;
}

void MotorController::moveSteps(int32_t steps) {
  if (!checkButtons()) {
    startMotor();
    direction = steps > 0 ? 1 : -1;
    motor.startMove(steps);
    isMoving = true;
  }
}