#include <Stepper.h>
#define STEPS_PER_REV 32
#define STEPS_PER_OUT 2048
#define MOTOR_SPEED 500

// pin assignments
const int analogIn = A0;

// stepper objs, param: IN1, IN2, IN3, IN4 (reverse order works - except reversed, duh)
Stepper stepper(STEPS_PER_REV, 8, 10, 9, 11);

void setup() {
    stepper.setSpeed(MOTOR_SPEED);
    Serial.begin(115200);
}

double value;
void loop() {
    // stepper.step(STEPS_PER_OUT);
    // stepper.step(-STEPS_PER_OUT);
    value = 1023 * (analogRead(analogIn) / 1023.0 - 0.5);
    Serial.println(value);

    stepper.setSpeed(abs(value));
    if (value > 0) stepper.step(1);
    else stepper.step(-1);
}
