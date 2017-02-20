#include <Stepper.h>
const int stepsPerRevolution = 200;
const int motorSpeed = 100;
const int stepLimit = 1000;
// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
int stepCount = 0;
bool forward = true;
int increment;

void setup() {}

void loop() {
    if (stepCount == 0) {
        forward = true;
    }else if (stepCount == stepLimit) {
        forward = false;
    }
    if (forward) {
        increment = stepsPerRevolution / 100;
    }else{
        increment = -stepsPerRevolution / 100;
    }

    myStepper.setSpeed(motorSpeed);
    myStepper.step(increment);
    stepCount = stepCount + increment;
}
