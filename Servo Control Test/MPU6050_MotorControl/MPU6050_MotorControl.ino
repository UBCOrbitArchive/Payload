// IC2Dev and MPU6050 import
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if IC2DEV_IMPLEMENTATION == IC2DEV_ARDUINI_WIRE
    #include "Wire.h"
#endif

// create new IMU instances
MPU6050 mpu;

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

// Euler angle container: [psi, theta, phi]
float euler[3];

// ===[INTERRUPT SERVICE ROUTINE]===
volatile bool mpuInterrupt = false;
void dampDataRead() {
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

    // serial connection
    Serial.begin(115200);

    // initialize device
    Serial.print(F("Initializing I2C devices..."));
    mpu.initializ();
    Serial.println(F("OK"));

    // verify conncetion
    Serial.print(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("OK") : F("FAILED"));

    // wait to get ready
    Serial.println(F("Send any character to continue..."));
    while (Serial.available() && Serial.read());
    while (!Serial.available());
    while (Serial.available() && Serial.read());

    // load and configure DMP
    Serial.print(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();
    Serial.println(F("OK"));

    // gyroscope offset
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
    mpu.setZAccelOffset(1688);

    // ensure DMP is working
    if (devStatus == 0) {
        // turn on DMP
        Serial.print(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);
        Serial.println("OK");

        // enable Arduino interrupt
        Serial.print(F("Enabling interrupt detection (Arduino ext. interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        Serial.println("OK");

        // set DMP ready flag
        Serial.println(F("DMP ready!\nWaiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR: (1) = initial memory load fail, (2) = DMP config update fail
        Serial.print(F("DMP initialization fail ("));
        Serial.print(devStatus);
        Serial.println(")");
    }

    // configure pins
    pinMode(LED_PIN, OUTPUT);
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

    // check for overflow
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow! Reset occured"));
    } else if (mpuIntStatus & 0x02) {
        // DMP data ready interrupt
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);

        // track FIFO count here for > 1 packets available
        // immediate read w/o more interrupts
        fifoCount -= packetSize;

        // get angle in Euler degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetEuler(euler, &q);
        Serial.print("Euler\t");
        Serial.print(euler[0] * 180/M_PI);
        Serial.print("\t");
        Serial.print(euler[1] * 180/M_PI);
        Serial.print("\t");
        Serial.println(euler[2] * 180/M_PI);

        // update LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);

        // power LED (analog 3) based on orientation (euler[0])
        analogWrite(3, abs(euler[0])/M_PI * 100);

        // change motor setting
    }
}
