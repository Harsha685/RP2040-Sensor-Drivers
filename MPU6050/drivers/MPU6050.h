#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

typedef struct {
    i2c_inst_t *i2c_port;
    uint8_t     addr;

    // calibration offsets (raw units)
    int16_t accel_offset[3];
    int16_t gyro_offset[3];
} mpu6050_t;

typedef struct {
    float x, y, z;   // in g
} mpu6050_accel_t;

typedef struct {
    float x, y, z;   // in deg/s
} mpu6050_gyro_t;

// Setup
bool mpu6050_init(mpu6050_t *dev, i2c_inst_t *i2c_port, uint8_t addr);
bool mpu6050_check_id(mpu6050_t *dev);
void mpu6050_calibrate(mpu6050_t *dev, int samples);

// Reading
void mpu6050_read_accel(mpu6050_t *dev, mpu6050_accel_t *out);
void mpu6050_read_gyro(mpu6050_t *dev, mpu6050_gyro_t *out);
float mpu6050_read_temp(mpu6050_t *dev);

#endif