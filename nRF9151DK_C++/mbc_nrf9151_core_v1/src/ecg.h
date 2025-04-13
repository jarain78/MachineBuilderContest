#ifndef ECG_ADC_H
#define ECG_ADC_H

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>

#define ECG_SAMPLES         30000      // 30 segundos a 1kHz
#define ECG_ADC_RESOLUTION  12         // 12 bits
#define ECG_ADC_CHANNEL     0          // Canal AIN0
#define ECG_ADC_INPUT       NRF_SAADC_INPUT_AIN0
#define ECG_ADC_GAIN        ADC_GAIN_1_6
#define ECG_ADC_REFERENCE   ADC_REF_INTERNAL
#define ECG_ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)
#define ECG_ADC_NODE        DT_LABEL(DT_NODELABEL(adc))

#define SAADC_GAIN         (1.0f / 6.0f)
#define SAADC_REF_VOLTAGE  0.6f  // V
#define ADC_RESOLUTION     4096.0f  // 12 bits


extern int16_t ecg_buffer[ECG_SAMPLES];  // Buffer para los datos crudos
extern struct k_sem ecg_done;            // Semáforo para indicar fin de adquisición

int ecg_adc_init(void);
//void ecg_sample_task(void);
float* ecg_sample_task(void);

    
#endif // ECG_ADC_H
