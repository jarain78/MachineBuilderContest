#include "lx16a_servo.h"

// === Bus methods ===
void lx16a_bus_flush(LX16ABus *bus) {
    uint8_t byte;
    while (uart_poll_in(bus->uart_dev, &byte) == 0);
}

void lx16a_bus_write(LX16ABus *bus, const uint8_t *buf, int buflen) {
    for (int i = 0; i < buflen; i++) {
        uart_poll_out(bus->uart_dev, buf[i]);
    }
}

bool lx16a_bus_available(LX16ABus *bus) {
    uint8_t dummy;
    return uart_poll_in(bus->uart_dev, &dummy) == 0;
}

int lx16a_bus_read(LX16ABus *bus, uint8_t *byte) {
    return uart_poll_in(bus->uart_dev, byte);
}

uint32_t lx16a_bus_time(LX16ABus *bus, uint8_t n) {
    return n * 10 * 1000 / bus->baud;
}

// === Servo methods ===
bool lx16a_write(LX16AServo *servo, uint8_t cmd, const uint8_t *params, int param_cnt) {
    if (param_cnt < 0 || param_cnt > 4) return false;

    int buflen = 6 + param_cnt;
    uint8_t buf[10];
    buf[0] = 0x55;
    buf[1] = 0x55;
    buf[2] = servo->id;
    buf[3] = buflen - 3;
    buf[4] = cmd;

    for (int i = 0; i < param_cnt; i++) {
        buf[5 + i] = params[i];
    }

    uint8_t cksum = 0;
    for (int i = 2; i < buflen - 1; i++) {
        cksum += buf[i];
    }
    buf[buflen - 1] = ~cksum;

    lx16a_bus_flush(servo->bus);
    lx16a_bus_write(servo->bus, buf, buflen);

    // Leer el loopback
    uint8_t rbyte;
    int got = 0;
    bool ok = true;
    uint32_t timeout = lx16a_bus_time(servo->bus, buflen) + 10;
    uint64_t start = k_uptime_get();

    while (got < buflen && k_uptime_get() - start < timeout) {
        if (lx16a_bus_read(servo->bus, &rbyte) == 0) {
            if (rbyte != buf[got]) {
                ok = false;
            }
            got++;
        }
    }

    return ok;
}

bool lx16a_read(LX16AServo *servo, uint8_t cmd, uint8_t *params, int param_len) {
    if (!lx16a_write(servo, cmd, NULL, 0)) return false;

    uint8_t buf[12];
    int got = 0;
    uint8_t sum = 0;
    int len = 7;

    uint32_t timeout = lx16a_bus_time(servo->bus, param_len + 6) + 20;
    uint64_t start = k_uptime_get();

    while (got < len && k_uptime_get() - start < timeout) {
        uint8_t byte;
        if (lx16a_bus_read(servo->bus, &byte) == 0) {
            buf[got] = byte;
            switch (got) {
            case 0:
            case 1:
                if (byte != 0x55) return false;
                break;
            case 2:
                if (byte != servo->id && servo->id != 0xfe) return false;
                break;
            case 3:
                if (byte < 3 || byte > 7) return false;
                len = byte + 3;
                if (len > param_len + 6) return false;
                break;
            case 4:
                if (byte != cmd) return false;
                break;
            default:
                if (got == len - 1) {
                    if ((uint8_t)byte == (uint8_t)~sum) {
                        for (int i = 0; i < param_len; i++) {
                            params[i] = buf[5 + i];
                        }
                        return true;
                    } else return false;
                }
            }
            if (got > 1) sum += byte;
            got++;
        }
    }
    return false;
}

bool lx16a_move_time(LX16AServo *servo, uint16_t angle, uint16_t time) {
    angle = angle / 24;
    uint8_t params[] = {
        (uint8_t)angle, (uint8_t)(angle >> 8),
        (uint8_t)time,  (uint8_t)(time >> 8)
    };
    return lx16a_write(servo, 1, params, sizeof(params));
}

