#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "MPU6050.h"

#define I2C_PORT      i2c0
#define I2C_SDA       4
#define I2C_SCL       5
#define MPU6050_ADDR  0x68

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    mpu6050_t mpu;
    if (!mpu6050_init(&mpu, I2C_PORT, MPU6050_ADDR)) {
        printf("MPU6050 not found!\n");
        while (true) {
            sleep_ms(1000);
        }
    }

    printf("MPU6050 found, calibrating (keep still)...\n");
    mpu6050_calibrate(&mpu, 200);
    printf("Calibration done.\n");

    while (true) {
        mpu6050_accel_t accel;
        mpu6050_gyro_t gyro;

        mpu6050_read_accel(&mpu, &accel);
        mpu6050_read_gyro(&mpu, &gyro);

        printf("Accel(g): X=%.3f Y=%.3f Z=%.3f\n", accel.x, accel.y, accel.z);
        printf("Gyro(dps): X=%.3f Y=%.3f Z=%.3f\n", gyro.x, gyro.y, gyro.z);
        printf("Temp: %.2f C\n\n", mpu6050_read_temp(&mpu));

        sleep_ms(500);
    }
}