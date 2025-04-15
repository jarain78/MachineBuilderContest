#ifndef I2C_HELPER_H
#define I2C_HELPER_H

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <stdint.h>

// Default I2C address for MPU6050
#define DEFAULT_I2C_ADDR 0x68

// Node label for the I2C device
#define I2C_HELPER_NODE DT_NODELABEL(i2c2)

// Initialize the I2C device
const struct device *i2c_helper_init(void);

// Read a single register from the I2C slave
int i2c_helper_read_reg(const struct device *i2c_dev, uint8_t dev_addr, uint8_t reg_addr, uint8_t *value);


void initMax30102(const struct device *i2c_dev);
void getHearthRate(void);
void configI2C(const struct device *i2c_dev);


#endif  // I2C_HELPER_H
