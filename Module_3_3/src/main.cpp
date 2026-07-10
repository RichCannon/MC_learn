#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "app_config.h"
#include "adc.h"
#include "pwm.h"

static const char *TAG = "LDR_CTRL";

extern "C" void app_main(void)
{

    adc_oneshot_unit_handle_t adc1_handle;
    adc_init(&adc1_handle);
    pwm_init(AppConfig::PINS::LED_PIN, AppConfig::PINS::MOTOR_PIN);

    ESP_LOGI(TAG, "Систему запущено. Початок зчитування ADC...");

    while (true)
    {
        int adc_raw = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, AppConfig::ADC::CHANNEL, &adc_raw));

        ESP_LOGI("ADC", "Raw: %d, Voltage_calibrated: %d mV", adc_raw);

        // Маштабуємо до duty cycle (0-1023)
        int dutyCycle = adc_raw / 4;

        ledc_set_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL_0, dutyCycle);
        ledc_update_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL_0);
        ledc_set_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL_1, dutyCycle);
        ledc_update_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL_1);

        vTaskDelay(pdMS_TO_TICKS(AppConfig::READ_DELAY_MS));
    }
}