#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_err.h"

namespace AppConfig
{
    namespace Pins
    {
        constexpr gpio_num_t PIN_INPUT = 4;
        constexpr gpio_num_t PIN_OUT = 17;

    }
    namespace System
    {
        constexpr uint32_t SERIAL_BAUD_RATE = 115200;
        constexpr uint32_t STARTUP_DELAY_MS = 3000;
    }
    namespace ADC
    {
        constexpr float VREF = 3.3f;
        constexpr float RESOLUTION = 4095.0f;
        constexpr adc_unit_t UNIT = ADC_UNIT_1;

        constexpr adc_oneshot_unit_init_cfg_t INIT_CONFIG = {
            .unit_id = ADC_UNIT_1,
            .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
            .ulp_mode = ADC_ULP_MODE_DISABLE,
        };
        constexpr adc_oneshot_chan_cfg_t CONFIG = {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        constexpr adc_channel_t CHANNEL = ADC_CHANNEL_3;
        constexpr adc_cali_curve_fitting_config_t CALI_CONFIG = {
            .unit_id = ADC_UNIT_1,
            .chan = ADC_CHANNEL_3,
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
    };

};

int getVoltage(int adcValue)
{
    return static_cast<int>(((static_cast<float>(adcValue) / AppConfig::ADC::RESOLUTION) * AppConfig::ADC::VREF) * 1000.0f);
}

float calculate_errors(int num1, int num2)
{
    if (num2 == 0)
        return 0;
    float num1_float = static_cast<float>(num1);
    float num2_float = static_cast<float>(num2);
    return ((num1_float - num2_float) / num2_float) * 100.0f;
}

extern "C" void app_main(void)
{
    adc_oneshot_unit_handle_t adc1_handle;
    adc_cali_handle_t cali_handle = NULL;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&AppConfig::ADC::INIT_CONFIG, &adc1_handle));

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, AppConfig::ADC::CHANNEL, &AppConfig::ADC::CONFIG));
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&AppConfig::ADC::CALI_CONFIG, &cali_handle));

    while (true)
    {
        int adc_raw;
        int voltage;
        int voltage_calibrated;
        float error;

        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, AppConfig::ADC::CHANNEL, &adc_raw));
        voltage = getVoltage(adc_raw);
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_calibrated));
        error = calculate_errors(voltage, voltage_calibrated);

        ESP_LOGI("ADC", "Raw: %d, Voltage: %d mV, Voltage_calibrated: %d mV, Error %.2f %", adc_raw, voltage, voltage_calibrated, error);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}