#include "i2c_helper.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h>
#include "max30102.h"
//#include "spo2.c"

const struct device *i2c_helper_init(void) {
    const struct device *i2c_dev = DEVICE_DT_GET(I2C_HELPER_NODE);
    
    if (!device_is_ready(i2c_dev)) {
        printk("I2C: Device not ready.\n");
        return NULL;
    }

    return i2c_dev;
}

int i2c_helper_read_reg(const struct device *i2c_dev, uint8_t dev_addr, uint8_t reg_addr, uint8_t *value) {
    //return i2c_write_read(i2c_dev, dev_addr, &reg_addr, sizeof(reg_addr), value, sizeof(*value));
    uint8_t NORMAL_OPERATION_MODE[] = {reg_addr, 0, 0};
    return i2c_write(i2c_dev, NORMAL_OPERATION_MODE, 3, dev_addr);

}

void initMax30102(const struct device *i2c_dev) {
    // Initialize MAX30102 Settings
    MAX30102_check(i2c_dev);     
    MAX30102_reset(i2c_dev);    
    MAX30102_clear(i2c_dev);   
    MAX30102_config(i2c_dev);
}
void getHearthRate(){

}
