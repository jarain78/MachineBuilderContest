#ifndef BIOSIGNAL_H
#define BIOSIGNAL_H

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
// Después (correcto)
#include <stddef.h>  // para size_t

#define ECG_LEN 128

extern float ecg_data[ECG_LEN];
extern float temp;
extern int hr;

void biosignal_init(const struct device *uart);
void biosignal_generate_fake_ecg(void);
void biosignal_handle_rx(char c);
void biosignal_send_data(float *ecg_array, int len);
void biosignal_send_uart(const char *data);

#endif // BIOSIGNAL_H
