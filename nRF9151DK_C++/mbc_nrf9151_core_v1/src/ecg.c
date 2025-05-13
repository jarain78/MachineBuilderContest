// ecg_adc.c
#include <nrfx_saadc.h>

#include "ecg.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
#include <string.h>


const struct device *ecg_adc_dev = NULL;
uint16_t ecg_data[ECG_SAMPLES];
static struct adc_channel_cfg adc_cfg;
static struct adc_sequence sequence;
static uint16_t sample_buffer[ECG_SAMPLES];

int ecg_adc_init(void) {
    ecg_adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));
    if (!device_is_ready(ecg_adc_dev)) {
        printk("ADC device not ready\n");
        return -1;
    }

    adc_cfg.channel_id = ECG_ADC_CHANNEL;
    adc_cfg.gain = ECG_ADC_GAIN;
    adc_cfg.reference = ECG_ADC_REFERENCE;
    adc_cfg.acquisition_time = ECG_ADC_ACQUISITION_TIME;
    adc_cfg.input_positive = ECG_ADC_INPUT;

    int ret = adc_channel_setup(ecg_adc_dev, &adc_cfg);
    if (ret != 0) {
        printk("ADC channel setup failed (%d)\n", ret);
        return ret;
    }

    sequence.options = NULL;
    sequence.channels = BIT(ECG_ADC_CHANNEL);
    sequence.buffer = sample_buffer;
    sequence.buffer_size = sizeof(sample_buffer);
    sequence.resolution = ECG_ADC_RESOLUTION;

    return 0;
}


int fcg_adc_init(void) {
    ecg_adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));
    if (!device_is_ready(ecg_adc_dev)) {
        printk("ADC device not ready\n");
        return -1;
    }

    adc_cfg.channel_id = FCG_ADC_CHANNEL;
    adc_cfg.gain = ECG_ADC_GAIN;
    adc_cfg.reference = ECG_ADC_REFERENCE;
    adc_cfg.acquisition_time = ECG_ADC_ACQUISITION_TIME;
    adc_cfg.input_positive = FCG_ADC_INPUT;

    int ret = adc_channel_setup(ecg_adc_dev, &adc_cfg);
    if (ret != 0) {
        printk("ADC channel setup failed (%d)\n", ret);
        return ret;
    }

    sequence.options = NULL;
    sequence.channels = BIT(ECG_ADC_CHANNEL);
    sequence.buffer = sample_buffer;
    sequence.buffer_size = sizeof(sample_buffer);
    sequence.resolution = ECG_ADC_RESOLUTION;

    return 0;
}


/*void ecg_sample_task(void) {
    for (int i = 0; i < ECG_SAMPLES; i++) {
        int ret = adc_read(adc_dev, &sequence);
        if (ret != 0) {
            printk("ADC read failed: %d\n", ret);
            break;
        }
        ecg_data[i] = sample_buffer[0];
        k_sleep(K_MSEC(1));  // 1kHz sampling
    }
    printk("ECG sampling complete\n");
}*/

float* ecg_sample_task(void) {
    static float ecg_data[ECG_SAMPLES];

    for (int i = 0; i < ECG_SAMPLES; i++) {
        int ret = adc_read(ecg_adc_dev, &sequence);
        if (ret != 0) {
            printk("ADC read failed: %d\n", ret);
            return NULL;
        }
        // Convert ADC sample to millivolts
        float millivolts = ((float)sample_buffer[0]) * (SAADC_REF_VOLTAGE / SAADC_GAIN) * 1000.0f / ADC_RESOLUTION;
        //printk("Sample %d: %d mV\n", i, (float)millivolts);
        ecg_data[i] = millivolts;
        k_sleep(K_MSEC(1));  // 1kHz sampling rate
    }

    printk("ECG sampling complete\n");
    return ecg_data;
}

float* fcg_sample_task(void) {
    static float ecg_data[ECG_SAMPLES];

    for (int i = 0; i < ECG_SAMPLES; i++) {
        int ret = adc_read(ecg_adc_dev, &sequence);
        if (ret != 0) {
            printk("ADC read failed: %d\n", ret);
            return NULL;
        }
        // Convert ADC sample to millivolts
        float millivolts = ((float)sample_buffer[0]) * (SAADC_REF_VOLTAGE / SAADC_GAIN) * 1000.0f / ADC_RESOLUTION;
        //printk("Sample %d: %d mV\n", i, (float)millivolts);
        ecg_data[i] = millivolts;
        k_sleep(K_MSEC(1));  // 1kHz sampling rate
    }

    printk("FCG sampling complete\n");
    return ecg_data;
}