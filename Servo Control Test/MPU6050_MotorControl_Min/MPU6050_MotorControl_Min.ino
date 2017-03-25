// IC2Dev and MPU6050 import
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if IC2DEV_IMPLEMENTATION == IC2DEV_ARDUINI_WIRE
#include "Wire.h"
#endif

// motor control import
#include <Stepper.h>

// motor constants
#define STEPS_PER_REV 32
#define STEPS_PER_OUT 2048

// different operating modes
#define HARD_INIT

// driver inputs in the order of: IN1, IN2, IN3, IN4
#define STEPPER_INPUT 8, 10, 9, 11

// create new IMU instances
MPU6050 mpu;

// create new stepper obj, param: steps per revolution, IN1, IN2, IN3, IN4 (reverse works)
Stepper stepper(STEPS_PER_REV, STEPPER_INPUT);

// motor target value
double motor_tgt;

// built in LED output
#define LED_PIN 13
bool LED_blinkState = false;

// ===[MPU control or status]===
// true if success DMP initialization
bool dmpReady = false;

// interrupt status byte from MPU
uint8_t mpuIntStatus;

// return status after each device operation, 0 = success
uint8_t devStatus;

// expected DMP packet size (42 bytes by default)
uint16_t packetSize;

// FIFO storange buffer
uint8_t fifoBuffer[64];

// count of all bytes currently in FIFO
uint16_t fifoCount;

// ===[orientation / motion]===
// Quarternion container: [w, x, y, z]
Quaternion q;

// gravity vector
VectorFloat gravity;

// yaw-pitch-roll container
float ypr[3];

// ===[INTERRUPT SERVICE ROUTINE]===
volatile bool mpuInterrupt = false;
void dmpDataReady() {
    mpuInterrupt = true;
}

// ===[INITIAL SETUP]===
void setup() {
    #if IC2DEV_IMPLEMENTATION == IC2DEV_ARDUINI_WIRE
    Wire.begin();

    // 400kHz I2C clock
    TWBR = 24;
    #elif IC2DEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
    #endif

    // initialize device
    mpu.initialize();

    // verify conncetion
    // Serial.print(F("Testing device connections..."));
    // Serial.println(mpu.testConnection() ? F("OK") : F("FAILED"));

    // load and configure DMP
    devStatus = mpu.dmpInitialize();

    // gyroscope offset
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
    mpu.setZAccelOffset(1688);

    // ensure DMP is working
    if (devStatus == 0) {
        // turn on DMP
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set DMP ready flag
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
        } else {
        // ERROR: (1) = initial memory load fail, (2) = DMP config update fail
    }

    // configure pins
    pinMode(LED_PIN, OUTPUT);

    // motor setup (set default motor speed)
    stepper.setSpeed(500);

    // instanciate stepper motor: move stepper motor until 
    #ifdef HARD_INIT
    // move motor 180 deg one direction
    #endif 

}

// ===[MAIN PROGRAM LOOP]===
void loop() {
    // halt if something failed
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packets available
    while (!mpuInterrupt && fifoCount < packetSize) {}

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for inputstream overflow
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset
        mpu.resetFIFO();
        } else if (mpuIntStatus & 0x02) {
        // DMP data ready interrupt
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);

        // track FIFO count here for > 1 packets available
        // immediate read w/o more interrupts
        fifoCount -= packetSize;

        // get yaw-pitch-roll profiles based on gravity
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        // update LED to indicate activity
        LED_blinkState = !LED_blinkState;
        digitalWrite(LED_PIN, LED_blinkState);

        // set motor target based on PITCH (ypr[1])
        // **ASSUMING that IMU is placed in reference to rotor
        // if pitch is greater than 0: move in one direciton, else move the other
        if (ypr[1] > 0) stepper.step(1);
        else stepper.step(-1);
    }
}
