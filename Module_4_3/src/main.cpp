#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "i2c.h"
#include "ds1307.h"
#include "wifi.h"
#include "u8g2_display.h"
#include "bme280_hal.h"
#include "app_config.h"

DateTime dt = {};
u8g2_t u8g2;
struct bme280_dev bme280_dev;

void renderDateTime()
{
    static char date_str[32];
    static char time_str[32];
    ds1307_read_time(dt);
    ESP_LOGI("TIMER", "Time: %.2d:%.2d:%.2d %.2d/%.2d/%.2d", dt.h, dt.m, dt.s, dt.D, dt.M, dt.Y);
    snprintf(time_str, sizeof(time_str), "%.2d:%.2d:%.2d ", dt.h, dt.m, dt.s);
    snprintf(date_str, sizeof(date_str), "%.2d/%.2d/%.2d", dt.D, dt.M, dt.Y);

    u8g2_SetFont(&u8g2, u8g2_font_ncenB12_tr);
    u8g2_DrawStr(&u8g2, 25, 40, time_str);
    u8g2_DrawStr(&u8g2, 25, 58, date_str);
}

void renderBME280Data()
{

    static char T_str[10];
    static char P_str[12];
    static char H_str[8];
    static struct bme280_data sensor_data;

    int8_t result = bme280_adapter_read_sensor(bme280_dev, sensor_data);
    if (result != BME280_OK)
    {
        ESP_LOGI("BME280", "error bme280_adapter_read_sensor: %d", result);
        return;
    }

    ESP_LOGI("BME280", "T:%.2f C  P:%.2f hPa  H:%.2f %%",
             sensor_data.temperature,
             sensor_data.pressure,
             sensor_data.humidity);

    snprintf(T_str, sizeof(T_str), "%.2f C",
             sensor_data.temperature);
    snprintf(P_str, sizeof(P_str), "%.2f hPa",
             sensor_data.pressure);
    snprintf(H_str, sizeof(H_str), "%.2f %%",
             sensor_data.humidity);

    u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(&u8g2, 2, 10, T_str);
    u8g2_DrawStr(&u8g2, 2, 20, P_str);
    u8g2_DrawStr(&u8g2, 50, 10, H_str);
}

void draw()
{
    u8g2_ClearBuffer(&u8g2);
    renderBME280Data();
    renderDateTime();
    u8g2_SendBuffer(&u8g2);
}

extern "C" void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(2000));
    init_i2c();
    oled_init(u8g2);

    bme280_adapter_init(bme280_dev, AppConfig::ADDRESSES::BME280);

    wifi_init_sta();
    sntp_sync_and_set_ds1307();

    while (true)
    {

        draw();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
