#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "i2c.h"
#include "ds1307.h"
#include "wifi.h"
#include "u8g2_display.h"

DateTime dt = {};
u8g2_t u8g2;

void renderDateTime()
{
    static char date_str[32];
    static char time_str[32];
    ds1307_read_time(dt);
    ESP_LOGI("TIMER", "Time: %.2d:%.2d:%.2d %.2d/%.2d/%.2d", dt.h, dt.m, dt.s, dt.D, dt.M, dt.Y);
    snprintf(time_str, sizeof(time_str), "%.2d:%.2d:%.2d ", dt.h, dt.m, dt.s);
    snprintf(date_str, sizeof(date_str), "%.2d/%.2d/%.2d", dt.D, dt.M, dt.Y);

    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB12_tr);
    u8g2_DrawStr(&u8g2, 15, 40, time_str);
    u8g2_DrawStr(&u8g2, 15, 58, date_str);
    u8g2_SendBuffer(&u8g2);
}

extern "C" void app_main(void)
{
    init_i2c();
    oled_init(u8g2);

    wifi_init_sta();
    sntp_sync_and_set_ds1307();

    while (true)
    {
        renderDateTime();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
