#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "app_config.h"
#include "adc.h"
#include "mqtt_manager.h"
#include "wifi_manager.h"

static const char *TAG = "MAIN";

// Функція визначення текстового стану освітлення
const char *get_light_status(int raw_val)
{
    // Переводимо raw (0-4095) у відсотки (0-100%)
    int percent = (raw_val * 100) / AppConfig::ADC::MAX_RAW_VALUE;
    percent = 100 - percent;

    if (percent >= 0 && percent <= 25)
    {
        return "ДУЖЕ ТЕМНО";
    }
    else if (percent > 25 && percent <= 50)
    {
        return "ТЕМНО";
    }
    else if (percent > 50 && percent <= 75)
    {
        return "СВІТЛО";
    }
    else
    {
        return "ДУЖЕ СВІТЛО";
    }
}

extern "C" void app_main(void)
{
    //-------------ADC1 Init + Calibration ---------------
    adc_cali_handle_t cali_handles[AppConfig::ADC::CHANNEL_COUNT] = {NULL}; // Дескриптор калібрування
    adc_channel_t channels[AppConfig::ADC::CHANNEL_COUNT] = {AppConfig::ADC::CHANNEL};

    adc_context adc_ctx = {};
    adc_ctx.unit_id = AppConfig::ADC::UNIT;
    adc_ctx.bitwidth = AppConfig::ADC::BITWIDTH;
    adc_ctx.channels = channels;
    adc_ctx.channel_count = AppConfig::ADC::CHANNEL_COUNT;
    adc_ctx.cali_handles = cali_handles;

    wifi_context_t wifi_ctx = {};
    wifi_ctx.ssid = AppConfig::WIFI::SSID;
    wifi_ctx.password = AppConfig::WIFI::PASS;

    mqtt_context_t mqtt_ctx = {};
    mqtt_ctx.broker_uri = AppConfig::MQTT::BROKER_URI;
    mqtt_ctx.client_id = AppConfig::MQTT::CLIENT_ID;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_LOGI(TAG, "Ініціалізація Wi-Fi...");
    ESP_ERROR_CHECK(wifi_init_sta(&wifi_ctx));

    if (!wifi_wait_for_connection(&wifi_ctx, AppConfig::WIFI::TIMEOUT_MS))
    {
        ESP_LOGE(TAG, "Не вдалося підключитися до Wi-Fi. Перезавантаження...");
        vTaskDelay(pdMS_TO_TICKS(2000));
        esp_restart();
    }

    ESP_LOGI(TAG, "Ініціалізація MQTT...");
    ESP_ERROR_CHECK(mqtt_init(&mqtt_ctx));

    ESP_LOGI(TAG, "Ініціалізація ADC...");
    ESP_ERROR_CHECK(adc_init(&adc_ctx));

    int raw[AppConfig::ADC::CHANNEL_COUNT] = {0};
    int voltage_mv[AppConfig::ADC::CHANNEL_COUNT] = {0};
    char last_status[32] = "";

    while (true)
    {
        if (adc_read_all_voltage_mv(&adc_ctx, raw, voltage_mv) == ESP_OK)
        {
            const char *current_status = get_light_status(raw[0]);
            if (strcmp(last_status, current_status) != 0)
            {
                ESP_LOGI(TAG, "Освітлення змінилось: %s (Raw: %d, mV: %d). Відправка MQTT...",
                         current_status, raw[0], voltage_mv[0]);

                if (mqtt_publish(&mqtt_ctx, AppConfig::MQTT::TOPIC, current_status) == ESP_OK)
                {
                    strncpy(last_status, current_status, sizeof(last_status));
                }
            }
        }
        else
        {
            ESP_LOGE(TAG, "Помилка читання ADC");
        }

        // ESP_LOGI(TAG, "voltage_mv: %d; raw: %d", voltage_mv[0], raw[0]);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
