#include "biosignal.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

float fake_ecg_data[ECG_LEN];
float temp = 36.7f;
int hr = 78;

static char rx_buffer[256];
static int rx_index = 0;

static const struct device *uart_dev = NULL;

char buf[1024];
int offset = 0;


void biosignal_handle_rx(char c) {
    if (c == '\n' || c == '\r') {
        rx_buffer[rx_index] = '\0';  // finalizar cadena

        if (strncmp(rx_buffer, "<CMD>:", 6) == 0) {
            char *cmd_ptr = strstr(rx_buffer, "ACTION=");
            if (cmd_ptr) {
                cmd_ptr += 7;  // avanzar a valor de acción
                char ack_buf[64];
                snprintf(ack_buf, sizeof(ack_buf), "<RESP>:ACK=%s;\n", cmd_ptr);
                biosignal_send_uart(ack_buf);
            }
        }

        rx_index = 0;  // reiniciar índice
    } else {
        if (rx_index < sizeof(rx_buffer) - 1) {
            rx_buffer[rx_index++] = c;
        } else {
            rx_index = 0;  // overflow: reinicia
        }
    }
}

void biosignal_init(const struct device *uart) {
    uart_dev = uart;
}


void biosignal_generate_fake_ecg(void) {
    for (int i = 0; i < ECG_LEN; i++) {
        fake_ecg_data[i] = 0.5f + 0.2f * sinf((float)i / 5.0f);  // señal simulada
    }
}

void biosignal_send_uart(const char *data) {
    for (size_t i = 0; i < strlen(data); i++) {
        uart_poll_out(uart_dev, data[i]);
    }
}

void biosignal_send_data(float *ecg_array, int len) {


    printk("Sending ECG data.\n");
    offset += snprintf(buf + offset, sizeof(buf) - offset, "<DATA>:ECG=[");
    

    for (int i = 0; i < len; i++) {
        
        offset += snprintf(buf + offset, sizeof(buf) - offset,
                           "%.4f%s", (double)ecg_array[i], (i < len - 1) ? "," : "");
        
        if (offset >= sizeof(buf) - 20) {  // Evita overflow
            break;
        }
    }

    offset += snprintf(buf + offset, sizeof(buf) - offset,
                       "];TEMP=%.1f;HR=%d;\n", (double)temp, hr);

    printk("Sending ECG data.\n");
    biosignal_send_uart(buf);
}
