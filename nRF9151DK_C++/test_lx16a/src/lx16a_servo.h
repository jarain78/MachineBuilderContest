#ifndef LX16A_SERVO_H
#define LX16A_SERVO_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const struct device *uart_dev;
    uint32_t baud;
} LX16ABus;

typedef struct {
    LX16ABus *bus;
    uint8_t id;
} LX16AServo;

// --- Bus interface ---
void lx16a_bus_flush(LX16ABus *bus);
void lx16a_bus_write(LX16ABus *bus, const uint8_t *buf, int buflen);
bool lx16a_bus_available(LX16ABus *bus);
int  lx16a_bus_read(LX16ABus *bus, uint8_t *byte);
uint32_t lx16a_bus_time(LX16ABus *bus, uint8_t n);

// --- Servo commands ---
bool lx16a_write(LX16AServo *servo, uint8_t cmd, const uint8_t *params, int param_cnt);
bool lx16a_read(LX16AServo *servo, uint8_t cmd, uint8_t *params, int param_len);
bool lx16a_move_time(LX16AServo *servo, uint16_t angle, uint16_t time);

#endif // LX16A_SERVO_H

