#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"

namespace AppConfig
{
    namespace PINS
    {

        constexpr gpio_num_t BUZER = GPIO_NUM_4;
    }
    namespace PWM
    {
        constexpr ledc_mode_t SPEED_MODE = LEDC_LOW_SPEED_MODE;
        constexpr ledc_sleep_mode_t SLEEP_MODE = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;
        constexpr ledc_clk_cfg_t CLK_CFG = LEDC_AUTO_CLK;
        constexpr ledc_timer_t TIMER = LEDC_TIMER_0;
        constexpr uint32_t DUTY = 2; // Повинне бути 128, але я хотів зробити тихіше
        constexpr ledc_mode_t MODE = LEDC_LOW_SPEED_MODE;
        constexpr ledc_channel_t CHANNEL = LEDC_CHANNEL_0;
        constexpr ledc_timer_bit_t DUTY_RES = LEDC_TIMER_8_BIT;
        constexpr ledc_intr_type_t INTR_TYPE = LEDC_INTR_DISABLE;
    }

    namespace Notes_HZ
    {
        constexpr uint32_t C4 = 262;
        constexpr uint32_t CS4 = 277;
        constexpr uint32_t D4 = 294;
        constexpr uint32_t DS4 = 311;
        constexpr uint32_t E4 = 330;
        constexpr uint32_t F4 = 349;
        constexpr uint32_t FS4 = 370;
        constexpr uint32_t G4 = 392;
        constexpr uint32_t GS4 = 415;
        constexpr uint32_t A4 = 440;
        constexpr uint32_t AS4 = 466;
        constexpr uint32_t B4 = 494;

        constexpr uint32_t C5 = 523;
        constexpr uint32_t CS5 = 554;
        constexpr uint32_t D5 = 587;
        constexpr uint32_t DS5 = 622;
        constexpr uint32_t E5 = 659;
        constexpr uint32_t F5 = 698;
        constexpr uint32_t FS5 = 740;
        constexpr uint32_t G5 = 784;
        constexpr uint32_t GS5 = 831;
        constexpr uint32_t A5 = 880;
        constexpr uint32_t AS5 = 932;
        constexpr uint32_t B5 = 988;
    }
};
