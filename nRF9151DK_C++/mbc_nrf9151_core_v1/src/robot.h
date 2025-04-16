#ifndef I2C_ROBOT_H
#define I2C_ROBOT_H

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <stdint.h>

// Robot Addres
#define ROBOT_I2C_ADDR         0x29
#define ROBOT_PARTID           0x01
#define ROBOT_EXPECTED_PARTID  0x42

typedef struct {
    const struct device *i2c_dev;
    uint8_t i2c_addr;
} robot_t;

int robot_begin(robot_t *dev, const struct device *i2c_dev, uint8_t addr);
uint8_t robot_get_part_id(robot_t *dev);
int robot_read_reg(robot_t *dev, uint8_t reg, void *buf, uint8_t len);
int robot_write_reg(robot_t *dev, uint8_t reg,  void *buf, uint8_t len);

#endif  // I2C_ROBOT_H