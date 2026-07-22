#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "app_config.h"
#include "pwm.h"
#include "iot_button.h"
#include "button_gpio.h"

void play_note(uint32_t note)
{
    ledc_set_freq(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::TIMER, note);

    ledc_set_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL, AppConfig::PWM::DUTY);
    ledc_update_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL);
}

void pause()
{
    ledc_set_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL, 0);
    ledc_update_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL);
}

bool btn1Pressed = false;
bool btn2Pressed = false;

static void onButtonEvent(void *button_handle, void *usr_data)
{

    int btn_id = (int)(uintptr_t)usr_data;
    button_event_t event = iot_button_get_event((button_handle_t)button_handle);

    if (event == BUTTON_PRESS_DOWN)
    {
        if (btn_id == 1)
            btn1Pressed = true;
        if (btn_id == 2)
            btn2Pressed = true;
    }
    else if (event == BUTTON_PRESS_UP)
    {
        if (btn_id == 1)
            btn1Pressed = false;
        if (btn_id == 2)
            btn2Pressed = false;
    }
}

extern "C" void app_main(void)
{

    button_handle_t btn1_handle = NULL;
    button_config_t btn1_cfg = {
        .long_press_time = 2000,
        .short_press_time = 50,
    };
    button_gpio_config_t gpio1_cfg = {
        .gpio_num = GPIO_NUM_16,
        .active_level = 1,
        .enable_power_save = false,
        .disable_pull = false};
    iot_button_new_gpio_device(&btn1_cfg, &gpio1_cfg, &btn1_handle);
    iot_button_register_cb(btn1_handle, BUTTON_PRESS_DOWN, NULL, onButtonEvent, (void *)1);
    iot_button_register_cb(btn1_handle, BUTTON_PRESS_UP, NULL, onButtonEvent, (void *)1);

    button_handle_t btn2_handle = NULL;
    button_config_t btn2_cfg = {
        .long_press_time = 2000,
        .short_press_time = 50,
    };
    button_gpio_config_t gpio2_cfg = {
        .gpio_num = GPIO_NUM_18,
        .active_level = 1,
        .enable_power_save = false,
        .disable_pull = false};
    iot_button_new_gpio_device(&btn2_cfg, &gpio2_cfg, &btn2_handle);
    iot_button_register_cb(btn2_handle, BUTTON_PRESS_DOWN, NULL, onButtonEvent, (void *)2);
    iot_button_register_cb(btn2_handle, BUTTON_PRESS_UP, NULL, onButtonEvent, (void *)2);
    pwm_init(AppConfig::PINS::BUZER);

    while (true)
    {
        if (btn1Pressed)
        {
            play_note(AppConfig::Notes_HZ::C5);
        }
        else if (btn2Pressed)
        {
            play_note(AppConfig::Notes_HZ::D5);
        }
        else
        {
            pause();
        }
    }
}