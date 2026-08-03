#include "i2c.h"
#include "app_config.h"

void init_i2c(void)
{
    i2c_config_t config = {};
    config.mode = I2C_MODE_MASTER,
    config.sda_io_num = AppConfig::Pins::SDA,
    config.scl_io_num = AppConfig::Pins::SCL,
    config.scl_pullup_en = GPIO_PULLUP_ENABLE,
    config.sda_pullup_en = GPIO_PULLUP_ENABLE,
    config.master.clk_speed = AppConfig::I2C::SPEED,

    i2c_param_config(AppConfig::I2C::PORT, &config);
    i2c_driver_install(AppConfig::I2C::PORT, config.mode, 0, 0, 0);
}
