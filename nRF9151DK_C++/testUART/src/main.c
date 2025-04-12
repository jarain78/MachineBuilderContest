#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define UART_NODE DT_NODELABEL(uart1)


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

#define MAX_LINE_LEN 128

const struct device *uart_dev;
char rx_buffer[MAX_LINE_LEN];
int rx_index = 0;

void read_uart_line_blocking(struct device *uart_dev) {
    uint8_t ch;
    int ret;

    while (1) {
        ret = uart_poll_in(uart_dev, &ch);
        if (ret == 0) {
            // Eco inmediato del carácter
            uart_poll_out(uart_dev, ch);

            if (ch == '\r' || ch == '\n') {
                // Fin de línea
                rx_buffer[rx_index] = '\0';  // Termina el string
                uart_poll_out(uart_dev, '\n');

                //printk("Línea recibida: %s\n", rx_buffer);

                // Procesar la línea aquí si deseas...

                // Reiniciar para la siguiente línea
                rx_index = 0;
                memset(rx_buffer, 0, sizeof(rx_buffer));
            } else {
                // Agregar carácter al buffer si hay espacio
                if (rx_index < MAX_LINE_LEN - 1) {
                    rx_buffer[rx_index++] = ch;
                }
            }
        } else {
            k_sleep(K_MSEC(1));  // Pequeña pausa si no hay datos
        }
    }
}


void main(void) {
    const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart_dev)) {
        return;
    }

    /*uint8_t ch;
    int ret;

    while (1) {
        uart_poll_out(uart_dev, 'H');
        uart_poll_out(uart_dev, 'i');
        uart_poll_out(uart_dev, '\n');

        ret = uart_poll_in(uart_dev, &ch);
        if (ret == 0) {
            uart_poll_out(uart_dev, ch); // Eco simple
            uart_poll_out(uart_dev, '\n');
        }

        k_sleep(K_MSEC(100));
    }*/
    read_uart_line_blocking(uart_dev);
}


/*
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define UART_NODE DT_NODELABEL(uart1)

void main(void) {
    const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart_dev)) {
        return;
    }

    uint8_t ch;
    int ret;

    while (1) {
        uart_poll_out(uart_dev, 'H');
        uart_poll_out(uart_dev, 'i');
        uart_poll_out(uart_dev, '\n');

        ret = uart_poll_in(uart_dev, &ch);
        if (ret == 0) {
            uart_poll_out(uart_dev, ch); // Eco simple
            uart_poll_out(uart_dev, '\n');
            printk("Received: %c\n", ch);
        }

        k_sleep(K_MSEC(100));
    }
}


*/