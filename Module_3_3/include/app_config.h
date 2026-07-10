#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

namespace AppConfig
{
    namespace PINS
    {
        constexpr gpio_num_t MOTOR_PIN = GPIO_NUM_16;
        constexpr gpio_num_t LED_PIN = GPIO_NUM_17;
    }

    namespace ADC
    {
        constexpr adc_channel_t CHANNEL = ADC_CHANNEL_3;
        constexpr adc_atten_t ATTEN = ADC_ATTEN_DB_12;
        constexpr adc_bitwidth_t BITWIDTH = ADC_BITWIDTH_DEFAULT;
    }

    namespace PWM
    {
        constexpr ledc_mode_t SPEED_MODE = LEDC_LOW_SPEED_MODE;
        constexpr ledc_timer_bit_t DUTY_RES = LEDC_TIMER_10_BIT;
        constexpr ledc_timer_t TIMER = LEDC_TIMER_0;
        constexpr ledc_channel_t CHANNEL_0 = LEDC_CHANNEL_0;
        constexpr ledc_channel_t CHANNEL_1 = LEDC_CHANNEL_1;
        constexpr ledc_clk_cfg_t CLK_CFG = LEDC_AUTO_CLK;
        constexpr ledc_intr_type_t INTR_TYPE = LEDC_INTR_DISABLE;
        constexpr ledc_sleep_mode_t SLEEP_MODE = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;
        constexpr uint32_t FREQ_HZ = 5000;
    }

    constexpr int READ_DELAY_MS = 100;
}
