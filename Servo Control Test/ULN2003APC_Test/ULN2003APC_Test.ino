#include <Stepper.h>
const int stepsPerRevolution = 200;
const int motorSpeed = 100;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
bool forward = true;
int increment;

void setup() {
    myStepper.setSpeed(motorSpeed);
}

void loop() {
    // if (forward) increment = stepsPerRevolution / 100;
    // else increment = -stepsPerRevolution / 100;
    myStepper.step(stepsPerRevolution);
    delay(500);
    myStepper.step(-200);
    delay(500);

    // if (millis() % 5000 <= 10) {
    //     forward = forward ? false : true;
    // }
}
