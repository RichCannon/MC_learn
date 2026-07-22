#include "wifi_manager.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include <string.h>
#include "app_config.h"

static const char *TAG = "WIFI_MGR";
static int s_retry_num = 0;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    wifi_context_t *ctx = (wifi_context_t *)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < AppConfig::WIFI::MAX_RETRIES)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Повторна спроба підключення до AP...");
        }
        else
        {
            xEventGroupSetBits(ctx->wifi_event_group, AppConfig::WIFI::FAIL_BIT);
        }
        ctx->connected = false;
        ESP_LOGI(TAG, "Не вдалося підключитися до AP");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Отримано IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        ctx->connected = true;
        xEventGroupSetBits(ctx->wifi_event_group, AppConfig::WIFI::CONNECTED_BIT);
    }
}

esp_err_t wifi_init_sta(wifi_context_t *ctx)
{
    if (ctx == NULL || ctx->ssid == NULL)
        return ESP_ERR_INVALID_ARG;

    ctx->wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, ctx, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, ctx, &instance_got_ip));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ctx->ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, ctx->password, sizeof(wifi_config.sta.password) - 1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WIFI_MODE_STA ініціалізовано.");
    ctx->initialized = true;
    return ESP_OK;
}

bool wifi_wait_for_connection(wifi_context_t *ctx, uint32_t timeout_ms)
{
    EventBits_t bits = xEventGroupWaitBits(ctx->wifi_event_group,
                                           AppConfig::WIFI::CONNECTED_BIT | AppConfig::WIFI::FAIL_BIT,
                                           pdFALSE, pdFALSE, pdMS_TO_TICKS(timeout_ms));

    if (bits & AppConfig::WIFI::CONNECTED_BIT)
    {
        return true;
    }
    return false;
}
