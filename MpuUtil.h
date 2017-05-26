/**
 * @file MpuRead.h
 * @author Aaron, Breanna
 * Code by Aaron, refactored by Breanna
 *
 * @brief Methods to utilize the MPU
 */

#include <stdint.h>
#include "I2C.h"
#include "MPU6050.h"

#define SCALE   0x10

/**
 * @enum measurement_t
 * @brief All the types of measurements from the MPU6050
 *
 */

enum measurement_t {
    ACCEL_X,        /**< Get the acceleration in the X direction*/
    ACCEL_Y,        /**< Get the acceleration in the Y direction*/
    ACCEL_Z,        /**< Get the acceleration in the Z direction*/
    GYRO_X,         /**< Get the gyroscope in the X direction*/
    GYRO_Y,         /**< Get the gyroscope in the Y direction*/
    GYRO_Z,         /**< Get the gyroscope in the Z direction*/
    TEMP            /**< Get the current temperature*/
};

/**
 * @fn i2cReadReg(uint8_t regAddress, uint8_t slave)
 * @param regAddress 8-bit address of the register to read from
 * @param slave 7-bit slave address of the chip
 * @brief Reads a specified register assuming a repeated start condition
 */
uint8_t i2cReadReg(uint8_t regAddress, uint8_t slave);

/**
 * @fn readMeasurement(enum measurement_t mm)
 * @param measurement_t Type of measurement to read
 * @brief Returns the current value of the specified sensor
 */
float readMeasurement(enum measurement_t mm);

/**
 * @fn cvtTemp(int16_t temp)
 * @param temp Raw temperature data
 * @brief Returns the converted temperature
 */
inline float cvtTemp(int16_t temp);

/**
 * @fn cvtAccel(int16_t accel)
 * @param accel Raw acceleration data
 * @brief Returns the converted acceleration
 */
inline float cvtAccel(int16_t accel);

/**
 * @fn cvtGyro(int16_t gyro)
 * @param gyro Raw gyroscope data
 * *brief Returns the converted gyroscope
 */
inline float cvtGyro(int16_t gyro);

/**
 * @fn mpuInit();
 * @brief TODO
 */
void mpuInit(void);
