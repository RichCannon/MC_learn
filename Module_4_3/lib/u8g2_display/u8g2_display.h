#ifndef U8G2_DISPLAY_H
#define U8G2_DISPLAY_H

#include <u8g2.h>

extern u8g2_t u8g2;
extern uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

void oled_init(u8g2_t &u8g2);

#endif
