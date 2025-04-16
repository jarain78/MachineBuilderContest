#include "i2c_helper.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h>
#include "max30102.h"
#include "spo2.h"



#define LED_BRIGHTNESS   0x1F
#define SAMPLE_AVG       2  // SAMPLEAVG_4
#define LED_MODE         3  // MODE_RED_IR
#define SAMPLE_RATE      3  // SAMPLERATE_400
#define PULSE_WIDTH      3  // PULSEWIDTH_411
#define ADC_RANGE        1  // ADCRANGE_4096
 
max30102_t sensor;

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
    
    if (max30102_begin(&sensor, i2c_dev, MAX30102_IIC_ADDRESS) != 0) {
        printk("MAX30102 not found\n");
        return;
    }

    printk("MAX30102 found and initialized\n");

    max30102_sensor_configure(&sensor, LED_BRIGHTNESS, SAMPLE_AVG, LED_MODE, SAMPLE_RATE, PULSE_WIDTH, ADC_RANGE);

}

void getHearthRate(){
    
    int32_t spo2, heart_rate;
    int8_t spo2_valid, hr_valid;
    max30102_read_fifo_and_calculate(&sensor, &spo2, &spo2_valid, &heart_rate, &hr_valid);

    printk("Heart Rate: %d bpm [%s], SpO2: %d %% [%s]\n",
           heart_rate,
           hr_valid ? "valid" : "invalid",
           spo2,
           spo2_valid ? "valid" : "invalid");

    k_sleep(K_SECONDS(5));  // Espera entre lecturas
}

void readRedLedPPG(){
    uint32_t red_values =  max30102_get_red(&sensor);
    printk("%d\n", red_values);
    k_sleep(K_MSEC(100));
}

void readIRLedPPG(){
    uint32_t red_values =  max30102_get_ir(&sensor);
    printk("%d\n", red_values);
    k_sleep(K_MSEC(100));
}

float* get30sRedLed(){
    printk("Capturing 30s of Red LED data...\n");
    float* red_data = capture_red_led_data(&sensor);
    //printk("Captured Red LED data: %u\n", red_data[0]);

    return red_data;
}


void configI2C(const struct device *i2c_dev) {
    printk("Configuring I2C...\n");
}



