#ifndef MAX30102_H
#define MAX30102_H

#include <zephyr/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>  
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#define MAX30102_IIC_ADDRESS  0x57 //I2C Address

//Status Registers
#define MAX30102_INTSTAT1        0x00//Interrupt Status1
#define MAX30102_INTSTAT2        0x01//Interrupt Status2
#define MAX30102_INTENABLE1      0x02//Interrupt Enable1
#define MAX30102_INTENABLE2      0x03//Interrupt Enable2
//FIFO Registers
#define MAX30102_FIFOWRITEPTR    0x04//FIFO Write Pointer, The FIFO Write Pointer points to the location where the MAX30102 writes the next sample. This pointer advances for each sample pushed on to the FIFO. It can also be changed through the I2C interface when MODE[2:0] is 010, 011, or 111.
#define MAX30102_FIFOOVERFLOW    0x05//FIFO Overflow Counter, When the FIFO is full, samples are not pushed on to the FIFO, samples are lost. OVF_COUNTER counts the number of samples lost. It saturates at 0x1F. When a complete sample is “popped” (i.e., removal of old FIFO data and shifting the samples down) from the FIFO (when the read pointer advances), OVF_COUNTER is reset to zero.
#define MAX30102_FIFOREADPTR     0x06//FIFO Read Pointer, The FIFO Read Pointer points to the location from where the processor gets the next sample from the FIFO through the I2C interface. This advances each time a sample is popped from the FIFO. The processor can also write to this pointer after reading the samples to allow rereading samples from the FIFO if there is a data communication error.
#define MAX30102_FIFODATA        0x07//FIFO Data Register, The circular FIFO depth is 32 and can hold up to 32 samples of data. The sample size depends on the number of LED channels (a.k.a. channels) configured as active. As each channel signal is stored as a 3-byte data signal, the FIFO width can be 3 bytes or 6 bytes in size.
//Configuration Registers
#define MAX30102_FIFOCONFIG      0x08//FIFO Configuration
#define MAX30102_MODECONFIG      0x09//Mode Configuration
#define MAX30102_PARTICLECONFIG  0x0A//SpO2 Configuration
#define MAX30102_LED1_PULSEAMP   0x0C//LED1 Pulse Amplitude
#define MAX30102_LED2_PULSEAMP   0x0D//LED2 Pulse Amplitude
#define MAX30102_LED3_PULSEAMP   0x0E//RESERVED
#define MAX30102_LED_PROX_AMP    0x10//RESERVED
#define MAX30102_MULTILEDCONFIG1 0x11//Multi-LED Mode Control Registers
#define MAX30102_MULTILEDCONFIG2 0x12//Multi-LED Mode Control Registers
//Die Temperature Registers
#define MAX30102_DIETEMPINT      0x1F//Die Temp Integer
#define MAX30102_DIETEMPFRAC     0x20//Die Temp Fraction
#define MAX30102_DIETEMPCONFIG   0x21//Die Temperature Config
#define MAX30102_PROXINTTHRESH   0x30//RESERVED
//Part ID Registers
#define MAX30102_REVISIONID      0xFE//Revision ID
#define MAX30102_PARTID          0xFF//Part ID:0x15
#define MAX30102_EXPECTED_PARTID  0x15

#define MAX30102_REG_FIFO_DATA  0x07
#define MAX30102_BUF_SIZE 32

#define MAX30102_CAPTURE_DURATION_SEC 30
#define MAX30102_SAMPLING_FREQ_HZ     25  // Ajusta según tu configuración
#define MAX30102_BUFFER_SIZE          (MAX30102_CAPTURE_DURATION_SEC * MAX30102_SAMPLING_FREQ_HZ)

static float red_buffer[MAX30102_BUFFER_SIZE];


typedef struct {
    uint32_t red[MAX30102_BUF_SIZE];
    uint32_t ir[MAX30102_BUF_SIZE];
    uint8_t head;
    uint8_t tail;
} max30102_buffer_t;

typedef struct {
    const struct device *i2c_dev;
    uint8_t i2c_addr;
    max30102_buffer_t buffer;
    uint8_t active_leds;
} max30102_t;

/*typedef struct {
    uint32_t red[MAX30102_BUF_SIZE];
    uint32_t ir[MAX30102_BUF_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t active_leds;
    const struct device *i2c_dev;
    uint16_t i2c_addr;
} max30102_t;*/

typedef struct {
    uint8_t led_brightness;
    uint8_t sample_avg;
    uint8_t led_mode;
    uint8_t sample_rate;
    uint8_t pulse_width;
    uint8_t adc_range;
} max30102_config_t;

// API pública
int max30102_begin(max30102_t *dev, const struct device *i2c_dev, uint8_t addr);
int max30102_configure(max30102_t *dev, const max30102_config_t *cfg);
int max30102_read_fifo(max30102_t *dev, uint32_t *red, uint32_t *ir);
int max30102_read_temperature(max30102_t *dev, float *temp_c);
uint8_t max30102_get_part_id(max30102_t *dev);

void max30102_soft_reset(max30102_t *dev);
void max30102_sensor_configure(max30102_t *dev,
    uint8_t led_brightness,
    uint8_t sample_avg,
    uint8_t led_mode,
    uint8_t sample_rate,
    uint8_t pulse_width,
    uint8_t adc_range);

void max30102_read_fifo_and_calculate(
        max30102_t *dev,
        int32_t *spo2,
        int8_t *spo2_valid,
        int32_t *hr,
        int8_t *hr_valid);

void max30102_get_new_data(max30102_t *dev);
uint32_t max30102_get_red(max30102_t *dev);
uint32_t max30102_get_ir(max30102_t *dev);
void readIRLedPPG();
void readRedLedPPG();
float* capture_red_led_data(max30102_t *dev);


#endif // MAX30102_H