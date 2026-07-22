#include "esp_log.h"

#include "mqtt_manager.h"

static const char *TAG = "MQTT_MGR";

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  mqtt_context_t *ctx = (mqtt_context_t *)handler_args;

  switch ((esp_mqtt_event_id_t)event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT Підключено");
    ctx->connected = true;
    break;
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "MQTT Відключено");
    ctx->connected = false;
    break;
  case MQTT_EVENT_PUBLISHED:
    ESP_LOGD(TAG, "Повідомлення успішно відправлено, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGE(TAG, "Помилка MQTT події");
    break;
  default:
    break;
  }
}

esp_err_t mqtt_init(mqtt_context_t *ctx)
{
  if (ctx == NULL || ctx->broker_uri == NULL)
    return ESP_ERR_INVALID_ARG;

  esp_mqtt_client_config_t mqtt_cfg = {};
  mqtt_cfg.broker.address.uri = ctx->broker_uri;
  mqtt_cfg.credentials.client_id = ctx->client_id;

  ctx->client = esp_mqtt_client_init(&mqtt_cfg);
  if (ctx->client == NULL)
    return ESP_FAIL;

  esp_mqtt_client_register_event(ctx->client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, ctx);
  esp_mqtt_client_start(ctx->client);

  ctx->initialized = true;
  return ESP_OK;
}

esp_err_t mqtt_publish(mqtt_context_t *ctx, const char *topic, const char *data)
{
  if (ctx == NULL || !ctx->connected)
    return ESP_ERR_INVALID_STATE;

  int msg_id = esp_mqtt_client_publish(ctx->client, topic, data, 0, 1, 0);
  if (msg_id < 0)
  {
    ESP_LOGE(TAG, "Помилка відправки повідомлення");
    return ESP_FAIL;
  }
  return ESP_OK;
}