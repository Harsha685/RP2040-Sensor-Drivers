# MPU6050 driver for Raspberry Pi Pico W (C/C++ SDK)

Minimal MPU6050 6-axis accelerometer/gyroscope driver for the Pico W,
built on the Raspberry Pi Pico C/C++ SDK `hardware_i2c` API.

## Files

- `MPU6050.h` / `MPU6050.c` — driver: init, WHO_AM_I check, calibration, accel/gyro/temp reads
- `main.c` — example application: calibrates on startup, then prints accel/gyro/temp twice a second over USB serial
- `CMakeLists.txt` — build configuration
- `pico_sdk_import.cmake` — standard Pico SDK import script

## Wiring (I2C0)

| MPU6050 pin | Pico W pin   |
|-------------|--------------|
| VCC         | 3V3 (pin 36) |
| GND         | any GND pin  |
| SCL         | GP5 (pin 7)  |
| SDA         | GP4 (pin 6)  |

Add 4.7kΩ pull-ups from SDA/SCL to 3.3V if your breakout doesn't already have them.

I2C address is `0x68` with the AD0 pin tied low (default), or `0x69` if AD0 is tied high.

## Behavior notes

- `mpu6050_init()` wakes the sensor out of sleep mode (`PWR_MGMT_1` = 0x00) and verifies
  its identity via the `WHO_AM_I` register before returning success.
- `mpu6050_calibrate()` averages raw accel/gyro samples while the sensor is stationary
  and stores the result as an offset, so place the board flat and still before/while
  this runs. It assumes Z-axis is pointing up (subtracts 1g from the Z accel average) —
  if your board is mounted on a different axis, adjust accordingly.
- Default full-scale ranges are accel ±2g and gyro ±250°/s (the sensor's power-on
  defaults); the sensitivity constants in `MPU6050.c` match those ranges.

## Build

```bash
mkdir -p build
cd build
cmake ..
make -j4
```

This produces `build/<project_name>.uf2`.

## Flash

Hold BOOTSEL on the Pico W, plug in via USB, then drag the `.uf2` onto the
mass-storage device that appears.

## Run

Open a serial terminal on the device's USB CDC port (e.g. `/dev/ttyACM0` on Linux):

```bash
screen /dev/ttyACM0 115200
```

Keep the board still while it calibrates, then you should see accelerometer (g),
gyroscope (°/s), and temperature (°C) readings printed twice a second.