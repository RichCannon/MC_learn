#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

namespace AppConfig
{
    namespace ADC
    {
        constexpr adc_unit_t UNIT = ADC_UNIT_1;
        constexpr adc_channel_t CHANNEL = ADC_CHANNEL_3;
        constexpr adc_atten_t ATTEN = ADC_ATTEN_DB_12;
        constexpr adc_bitwidth_t BITWIDTH = ADC_BITWIDTH_DEFAULT;

        constexpr int THRESHOLD_DARK = 2500;
        constexpr int THRESHOLD_LIGHT = 2000;
    }

    constexpr gpio_num_t LED_PIN = GPIO_NUM_2;

    constexpr int SMA_WINDOW_SIZE = 10;
    constexpr int READ_DELAY_MS = 100;
}

static const char *TAG = "LDR_CTRL";

class SimpleMovingAverage
{
private:
    int window[AppConfig::SMA_WINDOW_SIZE] = {0};
    int sum = 0;
    int index = 0;
    int count = 0;

public:
    int update(int new_val)
    {
        sum -= window[index];
        window[index] = new_val;
        sum += window[index];

        index = (index + 1) % AppConfig::SMA_WINDOW_SIZE;

        if (count < AppConfig::SMA_WINDOW_SIZE)
        {
            count++;
        }

        return sum / count;
    }
};

extern "C" void app_main(void)
{
    gpio_reset_pin(AppConfig::LED_PIN);
    gpio_set_direction(AppConfig::LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(AppConfig::LED_PIN, 0);
    bool led_state = false;

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = AppConfig::ADC::UNIT,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = AppConfig::ADC::ATTEN,
        .bitwidth = AppConfig::ADC::BITWIDTH,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, AppConfig::ADC::CHANNEL, &config));

    SimpleMovingAverage sma;

    ESP_LOGI(TAG, "Систему запущено. Початок зчитування ADC...");

    while (true)
    {
        int adc_raw = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, AppConfig::ADC::CHANNEL, &adc_raw));
        int filtered_val = sma.update(adc_raw);

        // 4. Логіка гістерезису
        if (filtered_val > AppConfig::ADC::THRESHOLD_DARK && !led_state)
        {
            led_state = true;
            gpio_set_level(AppConfig::LED_PIN, 1);
            ESP_LOGI(TAG, "Темно (значення: %d). Світлодіод УВІМКНЕНО.", filtered_val);
        }
        else if (filtered_val < AppConfig::ADC::THRESHOLD_LIGHT && led_state)
        {
            led_state = false;
            gpio_set_level(AppConfig::LED_PIN, 0);
            ESP_LOGI(TAG, "Світло (значення: %d). Світлодіод ВИМКНЕНО.", filtered_val);
        }

        vTaskDelay(pdMS_TO_TICKS(AppConfig::READ_DELAY_MS));
    }
}