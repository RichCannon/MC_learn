#include "u8g2_display.h"
#include "app_config.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"

uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static i2c_cmd_handle_t cmd;
    switch (msg)
    {
    case U8X8_MSG_BYTE_START_TRANSFER:
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (AppConfig::ADDRESSES::OLED << 1) | I2C_MASTER_WRITE, true);
        break;

    case U8X8_MSG_BYTE_SEND:
        i2c_master_write(cmd, (const uint8_t *)arg_ptr, arg_int, true);
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(AppConfig::I2C::PORT, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        break;
    }
    return 1;
}

void oled_init(u8g2_t &u8g2)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2,
        U8G2_R0,
        u8g2_esp32_i2c_byte_cb,
        u8x8_dummy_cb);

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}
