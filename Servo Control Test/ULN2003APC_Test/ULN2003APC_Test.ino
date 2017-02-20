#include <Stepper.h>
#define STEPS_PER_REV 32
#define STEPS_PER_OUT 2048
#define MOTOR_SPEED 500

// stepper objs, param: IN1, IN2, IN3, IN4 (reverse order works - except reversed, duh)
Stepper stepper(STEPS_PER_REV, 8, 10, 9, 11);

void setup() {
}

int  Steps2Take;
void loop() {
    stepper.setSpeed(MOTOR_SPEED);
    stepper.step(STEPS_PER_OUT);
    stepper.setSpeed(MOTOR_SPEED);
    stepper.step(-STEPS_PER_OUT);
}
