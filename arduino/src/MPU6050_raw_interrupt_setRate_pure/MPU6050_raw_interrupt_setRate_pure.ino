// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

unsigned long microsPerReading, microsPrevious;


// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;
float aax, aay, aaz;
float ggx, ggy, ggz;
static uint8_t int_state = 0;
unsigned long startTime;
unsigned long endTime;


// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO

const int interruptNumber = 0;
static int count_int = 0;

float convertRawAcceleration(int aRaw)
{
    // since we are using 2G range
    // -2g maps to a raw value of -32768
    // +2g maps to a raw value of 32767

    float a = (aRaw * 2.0) / 32768.0;
    return a;
}

float convertRawGyro(int gRaw)
{
    // since we are using 250 degrees/seconds range
    // -250 maps to a raw value of -32768
    // +250 maps to a raw value of 32767

    float g = (gRaw * 250.0) / 32768.0;
    return g;
}

typedef struct offsetData_ {
    int acc_x;
    int acc_y;
    int acc_z;
    int gyro_x;
    int gyro_y;
    int gyro_z;
} OffsetData;

void setAccGyroOffset(MPU6050& mpu, struct offsetData_& data)
{
    mpu.setXAccelOffset(data.acc_x);
    mpu.setYAccelOffset(data.acc_y);
    mpu.setZAccelOffset(data.acc_z);
    mpu.setXGyroOffset(data.gyro_x);
    mpu.setYGyroOffset(data.gyro_y);
    mpu.setZGyroOffset(data.gyro_z);
}

void setup()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
#endif

    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    accelgyro.setIntEnabled(0x01);
    accelgyro.setRate(4); // 1khz / (1 + 4) = 200 Hz
    accelgyro.setDLPFMode(0x03);

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" :
                   "MPU6050 connection failed");

    OffsetData offset_data;

    switch (4) {
    case 1:
        offset_data.acc_x = -2339 ;
        offset_data.acc_y = 683;
        offset_data.acc_z = 1666;
        offset_data.gyro_x = 32;
        offset_data.gyro_y = -72;
        offset_data.gyro_z = 5;
        setAccGyroOffset(accelgyro, offset_data);
        break;

    case 2:
        offset_data.acc_x = -3220;
        offset_data.acc_y = -489;
        offset_data.acc_z = 1443;
        offset_data.gyro_x = 69;
        offset_data.gyro_y = -3;
        offset_data.gyro_z = 22;
        setAccGyroOffset(accelgyro, offset_data);
        break;

    case 3:
        offset_data.acc_x = -1025;
        offset_data.acc_y = -3752;
        offset_data.acc_z = 1277;
        offset_data.gyro_x = 39;
        offset_data.gyro_y = 25;
        offset_data.gyro_z = 26;
        setAccGyroOffset(accelgyro, offset_data);
        break;

    case 4:
        offset_data.acc_x = -4379;
        offset_data.acc_y = -572;
        offset_data.acc_z = 1851;
        offset_data.gyro_x = 89;
        offset_data.gyro_y = 23;
        offset_data.gyro_z = 0;
        setAccGyroOffset(accelgyro, offset_data);
        break;

    case 5:
        offset_data.acc_x = -3356;
        offset_data.acc_y = -43;
        offset_data.acc_z = 1597;
        offset_data.gyro_x = 122;
        offset_data.gyro_y = -21;
        offset_data.gyro_z = 5;
        setAccGyroOffset(accelgyro, offset_data);
        break;

    default:
        Serial.print("unknown OffsetData\n");
    }

    attachInterrupt(interruptNumber, buttonStateChanged, RISING);
    startTime = millis();         // 記錄開始測量時間
}

void buttonStateChanged()
{
    count_int++;
    int_state = 1;
}

void loop()
{
    if (int_state) {
        // Is sample rate ?
#if 0
        unsigned long timestamp = (unsigned long)micros();
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        // convert from raw data to gravity and degrees/second units
        // 16384 * 0.061 * 9.8 / 1000
        aax = ax * 0.0005978;
        aay = ay * 0.0005978;
        aaz = az * 0.0005978;

        // 32768 * 250 / 32768.0
        ggx = gx * 0.00762939453125;
        ggy = gy * 0.00762939453125;
        ggz = gz * 0.00762939453125;

        Serial.print(aax);
        Serial.print(",");
        Serial.print(aay);
        Serial.print(",");
        Serial.print(aaz);
        Serial.print(",");
        // Serial.print(timestamp);
        Serial.print(",");
        Serial.print(ggx);
        Serial.print(",");
        Serial.print(ggy);
        Serial.print(",");
        Serial.print(ggz);
        Serial.print(",");
        Serial.println(timestamp);

#else
        // show sample rate
        endTime =  millis();

        if (endTime - startTime > 10000) {
            Serial.print("sample rate : ");
            Serial.println(count_int / 10.0);
            count_int = 0;
            startTime = endTime;
        }

#endif

        int_state = 0;
    }
}
