
#include "i2c_helper.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h>
#include "robot.h"


int robot_begin(robot_t *dev, const struct device *i2c_dev, uint8_t addr)
{
    dev->i2c_dev = i2c_dev;
    dev->i2c_addr = addr;
    
    uint8_t part_id;

    if (robot_get_part_id(dev) != ROBOT_EXPECTED_PARTID) {
        printk("Robot: Invalid part ID\n");
        return -1;
    }

    //max30102_soft_reset(dev);
    return 0;
}

uint8_t robot_get_part_id(robot_t *dev)
{
    uint8_t part_id = 0;
    //robot_read_reg(dev, 0x00, &part_id, 1);
    uint8_t mode_cfg;
    mode_cfg = 0x01;
    
    int wr_reg = robot_write_reg(dev, 0x01, &mode_cfg, 1);
    printk("Robot: Write register 0x01 = %d\n", wr_reg);
    uint8_t *buf;

    //i2c_read(dev, &buf, 1, 0x01);
    //printk(buf);
    
    //printk("Robot: Part ID = 0x%02X\n", part_id);
    return 1;
}

int robot_read_reg(robot_t *dev, uint8_t reg, void *buf, uint8_t len)
{
    int ret = i2c_write_read(dev->i2c_dev, dev->i2c_addr, &reg, 0x01, buf, len);
    return ret;
}

int robot_write_reg(robot_t *dev, uint8_t reg,  void *buf, uint8_t len)
{
    uint8_t tx_buf[len + 1];
    tx_buf[0] = reg;
    memcpy(&tx_buf[1], buf, len);
    return i2c_write(dev->i2c_dev, tx_buf, len + 1, dev->i2c_addr);
}

