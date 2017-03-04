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
}
