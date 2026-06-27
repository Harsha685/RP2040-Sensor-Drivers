#include "MPU6050.h"
#include "pico/stdlib.h"

#define REG_PWR_MGMT_1   0x6B
#define REG_WHO_AM_I     0x75
#define REG_ACCEL_XOUT_H 0x3B
#define REG_GYRO_XOUT_H  0x43
#define REG_TEMP_OUT_H   0x41

#define ACCEL_SENS  16384.0f   // LSB/g at ±2g
#define GYRO_SENS   131.0f     // LSB/(deg/s) at ±250dps

static void read_raw(mpu6050_t *dev, uint8_t reg, uint8_t *buf, size_t len)
{
    i2c_write_blocking(dev->i2c_port, dev->addr, &reg, 1, true);
    i2c_read_blocking(dev->i2c_port, dev->addr, buf, len, false);
}

bool mpu6050_init(mpu6050_t *dev, i2c_inst_t *i2c_port, uint8_t addr)
{
    dev->i2c_port = i2c_port;
    dev->addr = addr;
    dev->accel_offset[0] = dev->accel_offset[1] = dev->accel_offset[2] = 0;
    dev->gyro_offset[0]  = dev->gyro_offset[1]  = dev->gyro_offset[2]  = 0;

    uint8_t wakeup[2] = {REG_PWR_MGMT_1, 0x00};
    i2c_write_blocking(dev->i2c_port, dev->addr, wakeup, 2, false);

    sleep_ms(100);

    return mpu6050_check_id(dev);
}

bool mpu6050_check_id(mpu6050_t *dev)
{
    uint8_t chip_id = 0;
    read_raw(dev, REG_WHO_AM_I, &chip_id, 1);
    return (chip_id == 0x68) || (chip_id == 0x70) || (chip_id == 0x71) || (chip_id == 0x73);
}

static void read_accel_raw(mpu6050_t *dev, int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t data[6];
    read_raw(dev, REG_ACCEL_XOUT_H, data, 6);
    *ax = (data[0] << 8) | data[1];
    *ay = (data[2] << 8) | data[3];
    *az = (data[4] << 8) | data[5];
}

static void read_gyro_raw(mpu6050_t *dev, int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t data[6];
    read_raw(dev, REG_GYRO_XOUT_H, data, 6);
    *gx = (data[0] << 8) | data[1];
    *gy = (data[2] << 8) | data[3];
    *gz = (data[4] << 8) | data[5];
}

void mpu6050_calibrate(mpu6050_t *dev, int samples)
{
    int32_t sum_a[3] = {0,0,0};
    int32_t sum_g[3] = {0,0,0};
    int16_t ax, ay, az, gx, gy, gz;

    for (int i = 0; i < samples; i++) {
        read_accel_raw(dev, &ax, &ay, &az);
        read_gyro_raw(dev, &gx, &gy, &gz);

        sum_a[0] += ax; sum_a[1] += ay; sum_a[2] += az;
        sum_g[0] += gx; sum_g[1] += gy; sum_g[2] += gz;

        sleep_ms(5);
    }

    dev->accel_offset[0] = sum_a[0] / samples;
    dev->accel_offset[1] = sum_a[1] / samples;
    dev->accel_offset[2] = (sum_a[2] / samples) - 16384; // subtract 1g on Z (assumes flat & level)

    dev->gyro_offset[0] = sum_g[0] / samples;
    dev->gyro_offset[1] = sum_g[1] / samples;
    dev->gyro_offset[2] = sum_g[2] / samples;
}

void mpu6050_read_accel(mpu6050_t *dev, mpu6050_accel_t *out)
{
    int16_t ax, ay, az;
    read_accel_raw(dev, &ax, &ay, &az);

    out->x = (ax - dev->accel_offset[0]) / ACCEL_SENS;
    out->y = (ay - dev->accel_offset[1]) / ACCEL_SENS;
    out->z = (az - dev->accel_offset[2]) / ACCEL_SENS;
}

void mpu6050_read_gyro(mpu6050_t *dev, mpu6050_gyro_t *out)
{
    int16_t gx, gy, gz;
    read_gyro_raw(dev, &gx, &gy, &gz);

    out->x = (gx - dev->gyro_offset[0]) / GYRO_SENS;
    out->y = (gy - dev->gyro_offset[1]) / GYRO_SENS;
    out->z = (gz - dev->gyro_offset[2]) / GYRO_SENS;
}

float mpu6050_read_temp(mpu6050_t *dev)
{
    uint8_t data[2];
    read_raw(dev, REG_TEMP_OUT_H, data, 2);
    int16_t raw = (data[0] << 8) | data[1];
    return (raw / 340.0f) + 36.53f;  // per datasheet formula
}