#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

namespace AppConfig
{
    namespace PINS
    {
        constexpr gpio_num_t ENCODER_A_INPUT = GPIO_NUM_17;
        constexpr gpio_num_t ENCODER_B_INPUT = GPIO_NUM_16;
        constexpr gpio_num_t ENCODER_BUTTON_INPUT = GPIO_NUM_15;
        constexpr gpio_num_t SERVO_OUTPUT = GPIO_NUM_4;

    }

    namespace ENC
    {
        constexpr int DEBOUNCE_NS = 1000;
        constexpr int MAX_COUNT = 32767;
        constexpr int MIN_COUNT = -32768;
        constexpr int FULL_ROTATE_STEPS = 80;
        constexpr int HALF_ROTATE_STEPS = AppConfig::ENC::FULL_ROTATE_STEPS / 2;
        constexpr int STEPS_PER_CLICK = 4;

    }

    namespace PWM
    {
        constexpr ledc_mode_t SPEED_MODE = LEDC_LOW_SPEED_MODE;
        constexpr ledc_timer_bit_t DUTY_RES = LEDC_TIMER_12_BIT;
        constexpr ledc_timer_t TIMER = LEDC_TIMER_0;
        constexpr ledc_channel_t CHANNEL = LEDC_CHANNEL_0;
        constexpr ledc_clk_cfg_t CLK_CFG = LEDC_AUTO_CLK;
        constexpr ledc_intr_type_t INTR_TYPE = LEDC_INTR_DISABLE;
        constexpr ledc_sleep_mode_t SLEEP_MODE = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;
        constexpr uint32_t FREQ_HZ = 50;
    }

    namespace Servo
    {
        constexpr int MAX = 491;
        constexpr int MIN = 103;
        constexpr int RANGE = AppConfig::Servo::MAX - AppConfig::Servo::MIN;
        constexpr int STEP_SIZE = AppConfig::Servo::RANGE / (AppConfig::ENC::FULL_ROTATE_STEPS / AppConfig::ENC::STEPS_PER_CLICK);
    }
}
