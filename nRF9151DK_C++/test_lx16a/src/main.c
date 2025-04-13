#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>


// My custom includes
#include "lx16a_servo.h"

// Cambia según el UART que estés usando en tu overlay (ej: &uart0)
#define UART_NODE DT_NODELABEL(uart1)
#define BUTTON_NODE DT_ALIAS(sw0)


static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(BUTTON_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

volatile bool capture_requested = false;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    capture_requested = true;
}


int main(void) {
    printk("🔧 Inicializando servo LX-16A...\n");

    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);


    const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);
    if (!device_is_ready(uart_dev)) {
        printk("❌ UART no está lista\n");
        return;
    }

    // Inicializar el bus del servo
    LX16ABus bus = {
        .uart_dev = uart_dev,
        .baud = 115200
    };

    // Crear servo con ID 1
    LX16AServo servo = {
        .bus = &bus,
        .id = 1
    };

    while (1) {
        if (capture_requested) {
            
            printk("Capturando posicion...\n");
        
            printk("Mover a 45 grados\n");
            lx16a_move_time(&servo, 4500, 500); // 45.00 grados, 500 ms
            k_sleep(K_MSEC(1000));

            printk("Mover a 135 grados\n");
            lx16a_move_time(&servo, 13500, 500); // 135.00 grados, 500 ms
            k_sleep(K_MSEC(1000));
            capture_requested = false;
        }
    }

    return 0;
}
