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
#include "robot.h"  
#include "max30102.h"

// UART
#define UART_NODE DT_NODELABEL(uart1)
const struct device *uart_dev;

// Semaphore to trigger each task
struct k_sem ecg_sem;
struct k_sem max30102_sem;
struct k_sem i2c_sem;

// Thread stacks and structures
K_THREAD_STACK_DEFINE(ecg_stack, 1024);
K_THREAD_STACK_DEFINE(uart_stack, 1024);
K_THREAD_STACK_DEFINE(max30102_stack, 1024);
K_THREAD_STACK_DEFINE(i2c_stack, 1024);

struct k_thread ecg_thread_data;
struct k_thread uart_thread_data;
struct k_thread i2c_thread_data;
struct k_thread max30102_thread_data;

k_tid_t ecg_tid;
k_tid_t uart_tid;
k_tid_t i2c_tid;    
k_tid_t max30102_tid;    

robot_t robot;


void i2c_thread(void *arg1, void *arg2, void *arg3) {
    const struct device *i2c_dev = i2c_helper_init();

    if (i2c_dev == NULL) {
        printk("[I2C] Initialization failed.\n");
        return;
    }

    printk("Starting I2C bus scan...\n");

    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        struct i2c_msg msg;
        uint8_t dummy_tx = 0x00;

        msg.buf = &dummy_tx;
        msg.len = 1;
        msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

        int ret = i2c_transfer(i2c_dev, &msg, 1, addr);

        if (ret == 0) {
            printk("Found device at 0x%02X\n", addr);
        }
        k_msleep(10); // pequeña espera para no saturar el bus
    }

    printk("I2C scan complete.\n");


    while (1) {
        // Wait for the semaphore to be given
        // Uncomment the next line to block the thread until the semaphore is given
        //k_sem_take(&i2c_sem, K_FOREVER);


        //printk("[I2C] Thread.\n");

    }
}


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
            //biosignal_send_data_generic("ECG", captured_ecg, ECG_SAMPLES, 10.5, 78);
            biosignal_send_data("ECG", captured_ecg, ECG_SAMPLES);

        }

        printk("[ECG] Capture complete. Returning to wait state.\n");
    }
}

void max30102_thread(void *arg1, void *arg2, void *arg3) {
    const struct device *i2c_dev = i2c_helper_init();

    if (i2c_dev == NULL) {
        printk("[MAX30102] Initialization failed.\n");
        return;
    }

    initMax30102(i2c_dev);
    printk("[MAX30102] PPG thread started and waiting for signal...\n");

    while (1) {
        k_sem_take(&max30102_sem, K_FOREVER);
        printk("[MAX30102] Capturing data...\n");

        float *red_data = get30sRedLed();
        if (red_data == NULL) {
            printk("[MAX30102] Error capturing data.\n");
            continue;
        } else {
    
            //biosignal_send_data_generic("PPG", red_data, MAX30102_BUFFER_SIZE, 10.5, 78);
            biosignal_send_data("PPG", red_data, MAX30102_BUFFER_SIZE);
        }

        printk("[MAX30102] Capture complete. Returning to wait state.\n");
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

                if (strstr(cmd_buf, "<CMD>:START:ECG;")) {
                    printk("[UART] Trigger ECG capture.\n");
                    k_sem_give(&ecg_sem);
                } else if (strstr(cmd_buf, "<CMD>:START:PPG;")) {
                    printk("[UART] Trigger PPG capture.\n");
                    k_sem_give(&max30102_sem);
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

    biosignal_init(uart_dev);
    ecg_adc_init();

    k_sem_init(&ecg_sem, 0, 1);
    k_sem_init(&max30102_sem, 0, 1);

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

    max30102_tid = k_thread_create(&max30102_thread_data, max30102_stack,
                                   K_THREAD_STACK_SIZEOF(max30102_stack),
                                   max30102_thread,
                                   NULL, NULL, NULL,
                                   3, 0, K_NO_WAIT);
    k_thread_name_set(max30102_tid, "PPG Thread");

    i2c_tid = k_thread_create(&i2c_thread_data, i2c_stack,
        K_THREAD_STACK_SIZEOF(i2c_stack),
        i2c_thread,
        NULL, NULL, NULL,
        3, 0, K_NO_WAIT);
    k_thread_name_set(i2c_tid, "I2C Thread");


    return 0;
}
