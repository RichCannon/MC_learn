#include "esp_err.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef struct
{
  const char *ssid;
  const char *password;
  EventGroupHandle_t wifi_event_group;
  bool initialized;
  bool connected;
} wifi_context_t;

// Ініціалізація та підключення до Wi-Fi
esp_err_t wifi_init_sta(wifi_context_t *ctx);

// Очікування підключення (блокує потік до отримання IP або помилки)
bool wifi_wait_for_connection(wifi_context_t *ctx, uint32_t timeout_ms);
