#include "ds1307.h"
#include "app_config.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"

uint8_t bcd_to_dec(uint8_t val)
{
    uint8_t parsedVal = val & 0x7F;
    constexpr uint8_t mask = 0b00001111;
    uint8_t first_part = parsedVal >> 4;
    uint8_t second_part = parsedVal & mask;
    return first_part * 10 + second_part;
}

uint8_t dec_to_bcd(uint8_t val)
{
    uint8_t first_part = val / 10;
    uint8_t second_part = val % 10;
    return (first_part << 4) | second_part;
}

void ds1307_set_time(uint8_t sec, uint8_t min, uint8_t hour,
                     uint8_t dayOfWeek, uint8_t date, uint8_t month, uint8_t year)
{
    const uint8_t time_data[] = {
        dec_to_bcd(sec),
        dec_to_bcd(min),
        dec_to_bcd(hour),
        dec_to_bcd(dayOfWeek),
        dec_to_bcd(date),
        dec_to_bcd(month),
        dec_to_bcd(year)
    };

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AppConfig::ADDRESSES::TIMER << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_write(cmd, time_data, sizeof(time_data), true);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(AppConfig::I2C::PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void ds1307_read_time(DateTime &dt)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t buffer[7] = {0};

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AppConfig::ADDRESSES::TIMER << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (AppConfig::ADDRESSES::TIMER << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, (uint8_t *)buffer, 7, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(AppConfig::I2C::PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    dt.s = bcd_to_dec(buffer[0]);
    dt.m = bcd_to_dec(buffer[1]);
    dt.h = bcd_to_dec(buffer[2]);
    dt.W = bcd_to_dec(buffer[3]);
    dt.D = bcd_to_dec(buffer[4]);
    dt.M = bcd_to_dec(buffer[5]);
    dt.Y = bcd_to_dec(buffer[6]);
}
