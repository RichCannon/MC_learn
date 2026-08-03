#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "driver/gpio.h"
#include "driver/i2c.h"

namespace AppConfig
{
    namespace Pins
    {
        constexpr gpio_num_t SCL = GPIO_NUM_16;
        constexpr gpio_num_t SDA = GPIO_NUM_17;
    }
    namespace I2C
    {
        constexpr i2c_port_t PORT = I2C_NUM_0;
        constexpr uint32_t SPEED = 400000;
    }

    namespace ADDRESSES
    {
        constexpr uint32_t OLED = 0x3c;
        constexpr uint32_t TIMER = 0x68;
    }

    namespace WiFi
    {
        constexpr const char *SSID = "TP-Link_BB98";
        constexpr const char *PASSWORD = "17373807";
        constexpr uint8_t MAX_RETRY = 10;
    }
};

#endif
