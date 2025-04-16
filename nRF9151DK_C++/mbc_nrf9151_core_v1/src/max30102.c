#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>  
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "spo2.h"
#include "max30102.h"

int max30102_write_reg(max30102_t *dev, uint8_t reg, const void *buf, uint8_t len)
{
    uint8_t tx_buf[len + 1];
    tx_buf[0] = reg;
    memcpy(&tx_buf[1], buf, len);
    return i2c_write(dev->i2c_dev, tx_buf, len + 1, dev->i2c_addr);
}

int max30102_read_reg(max30102_t *dev, uint8_t reg, void *buf, uint8_t len)
{
    int ret = i2c_write_read(dev->i2c_dev, dev->i2c_addr, &reg, 1, buf, len);
    return ret;
}

int max30102_begin(max30102_t *dev, const struct device *i2c_dev, uint8_t addr)
{
    dev->i2c_dev = i2c_dev;
    dev->i2c_addr = addr;
    dev->active_leds = 2;
    memset(&dev->buffer, 0, sizeof(dev->buffer));

    uint8_t part_id;
    if (max30102_get_part_id(dev) != MAX30102_EXPECTED_PARTID) {
        printk("MAX30102: Invalid part ID\n");
        return -1;
    }

    max30102_soft_reset(dev);
    return 0;
}

void max30102_soft_reset(max30102_t *dev)
{
    uint8_t mode_cfg;
    max30102_read_reg(dev, MAX30102_MODECONFIG, &mode_cfg, 1);
    mode_cfg |= 0x40;  // Set RESET bit
    max30102_write_reg(dev, MAX30102_MODECONFIG, &mode_cfg, 1);

    // Esperar a que se borre el bit RESET
    uint32_t start_time = k_uptime_get();
    while (k_uptime_get() - start_time < 100) {
        max30102_read_reg(dev, MAX30102_MODECONFIG, &mode_cfg, 1);
        if ((mode_cfg & 0x40) == 0) break;
        k_msleep(1);
    }
}

uint8_t max30102_get_part_id(max30102_t *dev)
{
    uint8_t part_id = 0;
    max30102_read_reg(dev, MAX30102_PARTID, &part_id, 1);
    return part_id;
}

float max30102_read_temp_c(max30102_t *dev)
{
    uint8_t config = 0x01;
    max30102_write_reg(dev, MAX30102_DIETEMPCONFIG, &config, 1);

    uint32_t start = k_uptime_get();
    while (k_uptime_get() - start < 100) {
        max30102_read_reg(dev, MAX30102_DIETEMPCONFIG, &config, 1);
        if ((config & 0x01) == 0) break;
        k_msleep(1);
    }

    uint8_t temp_int, temp_frac;
    max30102_read_reg(dev, MAX30102_DIETEMPINT, &temp_int, 1);
    max30102_read_reg(dev, MAX30102_DIETEMPFRAC, &temp_frac, 1);

    return temp_int + (temp_frac * 0.0625f);
}

float max30102_read_temp_f(max30102_t *dev)
{
    return max30102_read_temp_c(dev) * 1.8f + 32.0f;
}

static uint8_t max30102_get_write_pointer(max30102_t *dev)
{
    uint8_t wp = 0;
    max30102_read_reg(dev, MAX30102_FIFOWRITEPTR, &wp, 1);
    return wp;
}

static uint8_t max30102_get_read_pointer(max30102_t *dev)
{
    uint8_t rp = 0;
    max30102_read_reg(dev, MAX30102_FIFOREADPTR, &rp, 1);
    return rp;
}

void max30102_get_new_data(max30102_t *dev)
{
    uint8_t read_ptr = max30102_get_read_pointer(dev);
    uint8_t write_ptr = max30102_get_write_pointer(dev);
    int32_t num_samples = write_ptr - read_ptr;

    if (num_samples < 0)
        num_samples += 32;

    int32_t bytes_to_read = num_samples * dev->active_leds * 3;

    while (bytes_to_read > 0) {
        uint8_t fifo_data[6] = {0};
        max30102_read_reg(dev, MAX30102_FIFODATA, fifo_data, dev->active_leds * 3);

        uint32_t ir = 0, red = 0;
        if (dev->active_leds > 1) {
            ir = ((fifo_data[0] << 16) | (fifo_data[1] << 8) | fifo_data[2]) & 0x3FFFF;
            red = ((fifo_data[3] << 16) | (fifo_data[4] << 8) | fifo_data[5]) & 0x3FFFF;
        } else {
            red = ((fifo_data[0] << 16) | (fifo_data[1] << 8) | fifo_data[2]) & 0x3FFFF;
        }

        dev->buffer.head = (dev->buffer.head + 1) % MAX30102_BUF_SIZE;
        dev->buffer.red[dev->buffer.head] = red;
        dev->buffer.ir[dev->buffer.head] = ir;

        bytes_to_read -= dev->active_leds * 3;
    }
}

uint32_t max30102_get_red(max30102_t *dev)
{
    max30102_get_new_data(dev);
    return dev->buffer.red[dev->buffer.head];
}

uint32_t max30102_get_ir(max30102_t *dev)
{
    max30102_get_new_data(dev);
    return dev->buffer.ir[dev->buffer.head];
}


static void max30102_set_fifo_average(max30102_t *dev, uint8_t samples)
{
    uint8_t fifo;
    max30102_read_reg(dev, MAX30102_FIFOCONFIG, &fifo, 1);
    fifo = (fifo & 0x8F) | ((samples & 0x07) << 4);
    max30102_write_reg(dev, MAX30102_FIFOCONFIG, &fifo, 1);
}

static void max30102_set_adc_range(max30102_t *dev, uint8_t range)
{
    uint8_t cfg;
    max30102_read_reg(dev, MAX30102_PARTICLECONFIG, &cfg, 1);
    cfg = (cfg & 0x9F) | ((range & 0x03) << 5);
    max30102_write_reg(dev, MAX30102_PARTICLECONFIG, &cfg, 1);
}

static void max30102_set_sample_rate(max30102_t *dev, uint8_t rate)
{
    uint8_t cfg;
    max30102_read_reg(dev, MAX30102_PARTICLECONFIG, &cfg, 1);
    cfg = (cfg & 0xE3) | ((rate & 0x07) << 2);
    max30102_write_reg(dev, MAX30102_PARTICLECONFIG, &cfg, 1);
}

static void max30102_set_pulse_width(max30102_t *dev, uint8_t width)
{
    uint8_t cfg;
    max30102_read_reg(dev, MAX30102_PARTICLECONFIG, &cfg, 1);
    cfg = (cfg & 0xFC) | (width & 0x03);
    max30102_write_reg(dev, MAX30102_PARTICLECONFIG, &cfg, 1);
}

static void max30102_set_led_mode(max30102_t *dev, uint8_t mode)
{
    uint8_t cfg;
    max30102_read_reg(dev, MAX30102_MODECONFIG, &cfg, 1);
    cfg = (cfg & 0xF8) | (mode & 0x07);
    max30102_write_reg(dev, MAX30102_MODECONFIG, &cfg, 1);
}

static void max30102_set_led_amplitude(max30102_t *dev, uint8_t red, uint8_t ir)
{
    max30102_write_reg(dev, MAX30102_LED1_PULSEAMP, &red, 1);
    max30102_write_reg(dev, MAX30102_LED2_PULSEAMP, &ir, 1);
}

static void max30102_enable_slot1(max30102_t *dev, uint8_t value)
{
    uint8_t reg;
    max30102_read_reg(dev, MAX30102_MULTILEDCONFIG1, &reg, 1);
    reg = (reg & 0xF0) | (value & 0x0F);
    max30102_write_reg(dev, MAX30102_MULTILEDCONFIG1, &reg, 1);
}

static void max30102_enable_slot2(max30102_t *dev, uint8_t value)
{
    uint8_t reg;
    max30102_read_reg(dev, MAX30102_MULTILEDCONFIG1, &reg, 1);
    reg = (reg & 0x0F) | ((value & 0x0F) << 4);
    max30102_write_reg(dev, MAX30102_MULTILEDCONFIG1, &reg, 1);
}

static void max30102_enable_fifo_rollover(max30102_t *dev)
{
    uint8_t fifo;
    max30102_read_reg(dev, MAX30102_FIFOCONFIG, &fifo, 1);
    fifo |= 0x10; // bit 4 = FIFO_ROLLOVER_EN
    max30102_write_reg(dev, MAX30102_FIFOCONFIG, &fifo, 1);
}

static void max30102_reset_fifo(max30102_t *dev)
{
    uint8_t zero = 0;
    max30102_write_reg(dev, MAX30102_FIFOWRITEPTR, &zero, 1);
    max30102_write_reg(dev, MAX30102_FIFOOVERFLOW, &zero, 1);
    max30102_write_reg(dev, MAX30102_FIFOREADPTR, &zero, 1);
}

void max30102_sensor_configure(max30102_t *dev,
    uint8_t led_brightness,
    uint8_t sample_avg,
    uint8_t led_mode,
    uint8_t sample_rate,
    uint8_t pulse_width,
    uint8_t adc_range)
{
    max30102_set_fifo_average(dev, sample_avg);
    max30102_set_adc_range(dev, adc_range);
    max30102_set_sample_rate(dev, sample_rate);
    max30102_set_pulse_width(dev, pulse_width);
    max30102_set_led_amplitude(dev, led_brightness, led_brightness);

    max30102_enable_slot1(dev, 1); // RED
    if (led_mode > 2)
        max30102_enable_slot2(dev, 2); // IR

    max30102_set_led_mode(dev, led_mode);
    dev->active_leds = (led_mode == 2) ? 1 : 2;

    max30102_enable_fifo_rollover(dev);
    max30102_reset_fifo(dev);
}

void max30102_read_fifo_and_calculate(
    max30102_t *dev,
    int32_t *spo2,
    int8_t *spo2_valid,
    int32_t *hr,
    int8_t *hr_valid)
{
    uint32_t ir_buffer_fifo[100];
    uint32_t red_buffer_fifo[100];
    int32_t collected = 0;

    while (collected < 100) {
        max30102_get_new_data(dev);  // llena buffer interno

        int8_t samples = dev->buffer.head - dev->buffer.tail;
        if (samples < 0) samples += MAX30102_BUF_SIZE;

        while (samples-- && collected < 100) {
            red_buffer_fifo[collected] = dev->buffer.red[dev->buffer.tail];
            ir_buffer_fifo[collected] = dev->buffer.ir[dev->buffer.tail];
            dev->buffer.tail = (dev->buffer.tail + 1) % MAX30102_BUF_SIZE;
            collected++;
        }

        k_msleep(5);  // breve espera para nuevas muestras
    }

    maxim_heart_rate_and_oxygen_saturation(
        ir_buffer_fifo,
        100,
        red_buffer_fifo,
        spo2,
        spo2_valid,
        hr,
        hr_valid
    );
}


float* capture_red_led_data(max30102_t *dev)
{
    uint32_t start_time = k_uptime_get(); 
    uint32_t elapsed = 0;
    int index = 0;

    memset(red_buffer, 0, sizeof(red_buffer)); // Asegura datos limpios

    while (elapsed < MAX30102_CAPTURE_DURATION_SEC * 1000 && index < MAX30102_BUFFER_SIZE) {
        uint32_t red_val = max30102_get_red(dev); 
        red_buffer[index++] = (float)red_val;

        printk("[MAX30102] Sample %d: %u (elapsed: %ums)\n", index, red_val, elapsed);

        k_msleep(1000 / MAX30102_SAMPLING_FREQ_HZ);  
        elapsed = k_uptime_get() - start_time;
    }

    printk("[MAX30102] Finished capture. Total samples: %d\n", index);
    return red_buffer;
}

