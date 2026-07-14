#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "app_config.h"
#include "adc.h"
#include "pwm.h"

static const char *TAG = "LDR_CTRL";

uint16_t transformADCValueToDutyCycle(int adc_raw)
{
    return ((adc_raw * (AppConfig::Servo::MAX - AppConfig::Servo::MIN)) / AppConfig::ADC::MAX_VALUE) + AppConfig::Servo::MIN;
}
uint16_t transformADCValueToDeg(int adc_raw)
{
    return ((adc_raw * 180) / AppConfig::ADC::MAX_VALUE);
}

extern "C" void app_main(void)
{

    adc_oneshot_unit_handle_t adc1_handle;
    adc_cali_handle_t adc_cali_handle;
    adc_init(&adc1_handle);
    adc_calibration_init(&adc_cali_handle);
    pwm_init(AppConfig::PINS::SERVO_PIN);

    ESP_LOGI(TAG, "Систему запущено. Початок зчитування ADC...");

    while (true)
    {
        int adc_raw = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, AppConfig::ADC::CHANNEL, &adc_raw));

        int dutyCycle = transformADCValueToDutyCycle(adc_raw);
        int deg = transformADCValueToDeg(adc_raw);

        ESP_LOGI("ADC", "Deg: %d", deg);

        ledc_set_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL, dutyCycle);
        ledc_update_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL);
    }
}