#include <stdint.h>
#include <msp430.h>
#include "MpuUtil.h"

uint8_t i2cReadReg(uint8_t regAddress, uint8_t slave)
{
    // Set the slave address
    slaveAddress = slave;
    // Send the register to read from
    TX_Data[0] = regAddress;
    // Only transmitting one byte
    TX_ByteCtr = 1;
    // Receiving 2-1 bytes
    RX_ByteCtr = 2;
    // Ensure a repeated start condition
    repeatedStart = 0x01;
    // Send the register to read
    i2cWrite(slaveAddress);

    // Read from the IC
    i2cRead(slaveAddress);
    // Remove repeated start condition
    repeatedStart = 0x00;
    // Return the data received
    return RX_Data[1];
}

float readMeasurement(enum measurement_t mm) {
    uint8_t buffer[2];

    switch(mm) {
    case ACCEL_X:
        // Read the high bits
        buffer[0] = i2cReadReg(MPU6050_RA_ACCEL_XOUT_H, MPU6050_ADDRESS_AD0_LOW);
        // Read the low bits
        buffer[1] = i2cReadReg(MPU6050_RA_ACCEL_XOUT_L, MPU6050_ADDRESS_AD0_LOW);
        // Concatenate and convert to true value
        return cvtAccel(((int16_t)buffer[0] << 8) | ((int16_t)buffer[1]));
    case ACCEL_Y:
        // Read the high bits
        buffer[0] = i2cReadReg(MPU6050_RA_ACCEL_YOUT_H, MPU6050_ADDRESS_AD0_LOW);
        // Read the low bits
        buffer[1] = i2cReadReg(MPU6050_RA_ACCEL_YOUT_L, MPU6050_ADDRESS_AD0_LOW);
        // Concatenate and convert to true value, Same for the rest of the measurements
        return cvtAccel(((int16_t)buffer[0] << 8) | ((int16_t)buffer[1]));
    case ACCEL_Z:
        buffer[0] = i2cReadReg(MPU6050_RA_ACCEL_ZOUT_H, MPU6050_ADDRESS_AD0_LOW);
        buffer[1] = i2cReadReg(MPU6050_RA_ACCEL_ZOUT_L, MPU6050_ADDRESS_AD0_LOW);
        return cvtAccel(((int16_t)buffer[0] << 8) | ((int16_t)buffer[1]));
    case GYRO_X:
        buffer[0] = i2cReadReg(MPU6050_RA_GYRO_XOUT_H, MPU6050_ADDRESS_AD0_LOW);
        buffer[1] = i2cReadReg(MPU6050_RA_GYRO_XOUT_L, MPU6050_ADDRESS_AD0_LOW);
        return cvtAccel(((int16_t)buffer[0] << 8) | ((int16_t)buffer[1]));
    case GYRO_Y:
        buffer[0] = i2cReadReg(MPU6050_RA_GYRO_YOUT_H, MPU6050_ADDRESS_AD0_LOW);
        buffer[1] = i2cReadReg(MPU6050_RA_GYRO_YOUT_L, MPU6050_ADDRESS_AD0_LOW);
        return cvtAccel(((int16_t)buffer[0] << 8) | ((int16_t)buffer[1]));
    case GYRO_Z:
        buffer[0] = i2cReadReg(MPU6050_RA_GYRO_ZOUT_H, MPU6050_ADDRESS_AD0_LOW);
        buffer[1] = i2cReadReg(MPU6050_RA_GYRO_ZOUT_L, MPU6050_ADDRESS_AD0_LOW);
        return cvtAccel(((int16_t)buffer[0] << 8) | ((int16_t)buffer[1]));
    case TEMP:
        buffer[0] = i2cReadReg(MPU6050_RA_TEMP_OUT_H, MPU6050_ADDRESS_AD0_LOW);
        buffer[1] = i2cReadReg(MPU6050_RA_TEMP_OUT_L, MPU6050_ADDRESS_AD0_LOW);
        return cvtTemp(((int16_t)buffer[0] << 8) | ((int16_t)buffer[1]));
    default:
        return 0;
    }
}

inline float cvtTemp(int16_t temp) {
    // Formula from datasheet
    return ((temp / 340.0f) + 36.53);
}

inline float cvtAccel(int16_t accel) {
    // Raw data is the two's complement scaled according
    // to the range selected
    switch (SCALE) {
    case 0x00:
        return (~accel + 0x01) / (16384.0f / 2);
    case 0x08:
        return (~accel + 0x01) / (8192.0f / 2);
    case 0x10:
        return (~accel + 0x01) / (4096.0f / 2);
    case 0x18:
        return (~accel + 0x01) / (2048.0f / 2);
    default:
        return 0;
    }
}

inline float cvtGyro(int16_t gyro) {
    // Raw data is the two's complement scaled according
    // to the range selected
    switch (SCALE) {
    case 0x00:
        return (~gyro + 0x01) / (131.0f / 2);
    case 0x08:
        return (~gyro + 0x01) / (65.5f / 2);
    case 0x10:
        return (~gyro + 0x01) / (32.8f / 2);
    case 0x18:
        return (~gyro + 0x01) / (16.4f / 2);
    default:
        return 0;
    }
}

void mpuInit() {

    volatile int i;

    // Enable pins 3.0 and 3.1 for I2C operation
    P3SEL |= 0x03;

    // To read from registers, the MPU6050 requires a repeated start condition
    repeatedStart = 0x01;
    // When pin AD0 on MPU6050 is low, slave address is 0x68
    // When it is high, the slave address is 0x69
    slaveAddress = MPU6050_ADDRESS_AD0_LOW;

    // Initialize the I2C peripheral
    i2cInit();

    // For some reason, the chip doesn't respond if a register isn't read first
    uint8_t buffer[2];
    buffer[0] = i2cReadReg(MPU6050_RA_PWR_MGMT_1, MPU6050_ADDRESS_AD0_LOW);

    // Address of PWR_MGMT_1 register
    TX_Data[1] = MPU6050_RA_PWR_MGMT_1;
    // Set register to 0x80, resets all register values
    TX_Data[0] = 0x80;
    TX_ByteCtr = 2;
    i2cWrite(slaveAddress);

    // Delay to ensure correct data
    for(i = 0; i < 1000; i++);

    // Address of PWR_MGMT_1 register
    TX_Data[1] = MPU6050_RA_PWR_MGMT_1;
    // Clear register reset pin (unsure if this is required)
    TX_Data[0] = 0x00;
    TX_ByteCtr = 2;
    i2cWrite(slaveAddress);

    // Place the accelerometer in mode 1, +/- 4g
    TX_Data[1] = MPU6050_RA_ACCEL_CONFIG;
    TX_Data[0] = SCALE;
    TX_ByteCtr = 2;
    i2cWrite(slaveAddress);

    // Place the gyroscope in mode 1, +/- 500 degree/seconds
    TX_Data[1] = MPU6050_RA_GYRO_CONFIG;
    TX_Data[0] = SCALE;
    TX_ByteCtr = 2;
    i2cWrite(slaveAddress);

    // First data is always garbage
    readMeasurement(ACCEL_X);
    readMeasurement(ACCEL_Y);
    readMeasurement(ACCEL_Z);
    readMeasurement(GYRO_X);
    readMeasurement(GYRO_X);
    readMeasurement(GYRO_X);
    readMeasurement(TEMP);
}

