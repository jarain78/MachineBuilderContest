#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include <string.h>

// My includes
#include "biosignal.h"
#include "ecg.h"
#include "i2c_helper.h"


// UART
#define UART_NODE DT_NODELABEL(uart1)
const struct device *uart_dev;

// Semaphore to trigger the ECG task
struct k_sem ecg_sem;
struct k_sem i2c_sem;


// Thread stacks and structures
K_THREAD_STACK_DEFINE(ecg_stack, 1024);
K_THREAD_STACK_DEFINE(uart_stack, 1024);
K_THREAD_STACK_DEFINE(i2c_stack, 1024);

struct k_thread ecg_thread_data;
struct k_thread uart_thread_data;
struct k_thread i2c_thread_data;
k_tid_t ecg_tid;
k_tid_t uart_tid;
k_tid_t i2c_tid;    

// ECG data capture simulation
void ecg_thread(void *arg1, void *arg2, void *arg3) {
    printk("[ECG] ECG thread started and waiting for signal...\n");

    while (1) {
        k_sem_take(&ecg_sem, K_FOREVER);
        printk("[ECG] Capturing data...\n");

        float *captured_ecg = ecg_sample_task();
        if (captured_ecg == NULL) {
            printk("[ECG] Error capturing data.\n");
            continue;
        } else {
            biosignal_send_data(captured_ecg, ECG_SAMPLES);
        }

        printk("[ECG] Capture complete. Returning to wait state.\n");
    }
}

void i2c_thread(void *arg1, void *arg2, void *arg3) {
    const struct device *i2c_dev = i2c_helper_init();

    if (i2c_dev == NULL) {
        printk("[I2C] Initialization failed.\n");
        return;
    }
    initMax30102(i2c_dev);

    while(1){
        uint8_t value;
        int ret = i2c_helper_read_reg(i2c_dev, DEFAULT_I2C_ADDR, 0x75, &value);
        if (ret < 0) {
            printk("[I2C] Read failed: %d\n", ret);
        } else {
            printk("[I2C] Read value: %02X\n", value);
        }
    }
}

void uart_thread(void *arg1, void *arg2, void *arg3) {
    unsigned char ch;
    char cmd_buf[64];
    int cmd_index = 0;

    printk("[UART] UART thread started. Waiting for commands...\n");

    while (1) {
        if (uart_poll_in(uart_dev, &ch) == 0) {
            uart_poll_out(uart_dev, ch);  // echo

            if (ch == '\r' || ch == '\n') {
                cmd_buf[cmd_index] = '\0';
                printk("\n[UART] Command received: %s\n", cmd_buf);

                if (strstr(cmd_buf, "<CMD>:START_ECG;")) {
                    printk("[UART] Sending signal to ECG thread...\n");
                    k_sem_give(&ecg_sem);
                }else if(strstr(cmd_buf, "<CMD>:START_PPG;")) {
                    printk("[I2C] Sending signal to PPG thread...\n");
                    k_sem_give(&i2c_sem);
                }

                cmd_index = 0;
                memset(cmd_buf, 0, sizeof(cmd_buf));
            } else {
                if (cmd_index < sizeof(cmd_buf) - 1) {
                    cmd_buf[cmd_index++] = ch;
                }
            }
        } else {
            k_msleep(1);
        }
    }
}


int main(void) {
    uart_dev = DEVICE_DT_GET(UART_NODE);
    if (!device_is_ready(uart_dev)) {
        printk("[ERROR] UART not ready\n");
        return -1;
    }

    printk("[SYSTEM] UART initialized\n");

    // Initialize ADC and other devices
    biosignal_init(uart_dev);
    ecg_adc_init();

    // Initialize semaphore
    k_sem_init(&ecg_sem, 0, 1);

    // Create threads
    ecg_tid = k_thread_create(&ecg_thread_data, ecg_stack,
                              K_THREAD_STACK_SIZEOF(ecg_stack),
                              ecg_thread,
                              NULL, NULL, NULL,
                              4, 0, K_NO_WAIT);
    k_thread_name_set(ecg_tid, "ECG Thread");

    uart_tid = k_thread_create(&uart_thread_data, uart_stack,
                               K_THREAD_STACK_SIZEOF(uart_stack),
                               uart_thread,
                               NULL, NULL, NULL,
                               5, 0, K_NO_WAIT);
    k_thread_name_set(uart_tid, "UART Thread");

    i2c_tid = k_thread_create(&i2c_thread_data, i2c_stack,
                              K_THREAD_STACK_SIZEOF(i2c_stack),
                              i2c_thread,
                              NULL, NULL, NULL,
                              3, 0, K_NO_WAIT);
    k_thread_name_set(i2c_tid, "I2C Thread");

    return 0;
}
