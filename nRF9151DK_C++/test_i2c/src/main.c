#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#define I2C_NODE DT_NODELABEL(i2c2)
#define I2C_ADDR 0x68  // Dirección del esclavo I2C (ej: MPU6050)

int main(void)
{
    const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev)) {
        printk("I2C: Device not ready.\n");
        return;
    }

    uint8_t whoami_reg = 0x75;
    uint8_t whoami_val;

    int ret = i2c_write_read(i2c_dev, I2C_ADDR, &whoami_reg, sizeof(whoami_reg),
                             &whoami_val, sizeof(whoami_val));

    if (ret == 0) {
        printk("WHOAMI: 0x%02X\n", whoami_val);
    } else {
        printk("I2C read failed: %d\n", ret);
    }

    return 0;   
}
