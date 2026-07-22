#pragma once

#include "mqtt_client.h"
#include "app_config.h"

typedef struct
{
  const char *broker_uri;
  const char *client_id;
  esp_mqtt_client_handle_t client;
  bool connected;
  bool initialized;
} mqtt_context_t;

// Ініціалізація та запуск MQTT клієнта
esp_err_t mqtt_init(mqtt_context_t *ctx);

// Відправка повідомлення у заданий topic
esp_err_t mqtt_publish(mqtt_context_t *ctx, const char *topic, const char *data);