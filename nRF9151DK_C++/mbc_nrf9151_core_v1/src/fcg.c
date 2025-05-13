// fcg_adc.c
#include <nrfx_saadc.h>

#include "fcg.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
#include <string.h>


const struct device *fcg_adc_dev = NULL;
uint16_t fcg_data[FCG_SAMPLES];
static struct adc_channel_cfg adc_cfg;
static struct adc_sequence sequence;
static uint16_t sample_buffer[FCG_SAMPLES];

int fcg_adc_init(void) {
    fcg_adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));
    if (!device_is_ready(fcg_adc_dev)) {
        printk("ADC device not ready\n");
        return -1;
    }

    adc_cfg.channel_id = FCG_ADC_CHANNEL;
    adc_cfg.gain = FCG_ADC_GAIN;
    adc_cfg.reference = FCG_ADC_REFERENCE;
    adc_cfg.acquisition_time = FCG_ADC_ACQUISITION_TIME;
    adc_cfg.input_positive = FCG_ADC_INPUT;

    int ret = adc_channel_setup(fcg_adc_dev, &adc_cfg);
    if (ret != 0) {
        printk("ADC channel setup failed (%d)\n", ret);
        return ret;
    }

    sequence.options = NULL;
    sequence.channels = BIT(FCG_ADC_CHANNEL);
    sequence.buffer = sample_buffer;
    sequence.buffer_size = sizeof(sample_buffer);
    sequence.resolution = FCG_ADC_RESOLUTION;

    return 0;
}

/*void fcg_sample_task(void) {
    for (int i = 0; i < FCG_SAMPLES; i++) {
        int ret = adc_read(adc_dev, &sequence);
        if (ret != 0) {
            printk("ADC read failed: %d\n", ret);
            break;
        }
        fcg_data[i] = sample_buffer[0];
        k_sleep(K_MSEC(1));  // 1kHz sampling
    }
    printk("FCG sampling complete\n");
}*/

float* fcg_sample_task(void) {
    static float fcg_data[FCG_SAMPLES];

    for (int i = 0; i < FCG_SAMPLES; i++) {
        int ret = adc_read(fcg_adc_dev, &sequence);
        if (ret != 0) {
            printk("ADC read failed: %d\n", ret);
            return NULL;
        }
        // Convert ADC sample to millivolts
        float millivolts = ((float)sample_buffer[0]) * (SAADC_REF_VOLTAGE / SAADC_GAIN) * 1000.0f / ADC_RESOLUTION;
        //printk("Sample %d: %d mV\n", i, (float)millivolts);
        fcg_data[i] = millivolts;
        k_sleep(K_MSEC(1));  // 1kHz sampling rate
    }

    printk("FCG sampling complete\n");
    return fcg_data;
}