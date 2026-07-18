#include "btn.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static gpio_num_t btn_pin;
static bool btn_stable_state = true;
static bool btn_prev_state = true;

void init_encoder_button(gpio_num_t pin)
{
    btn_pin = pin;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}

bool btn_is_pressed()
{
    static TickType_t last_change = 0;
    static bool last_raw_state = true;
    bool raw_state = gpio_get_level(btn_pin);

    if (raw_state != last_raw_state)
    {
        last_change = xTaskGetTickCount();
        last_raw_state = raw_state;
    }

    if ((xTaskGetTickCount() - last_change) >= pdMS_TO_TICKS(50))
    {
        btn_stable_state = raw_state;
    }

    bool isPressed = btn_stable_state == 0 && btn_prev_state != btn_stable_state;
    btn_prev_state = btn_stable_state;
    return isPressed;
}
