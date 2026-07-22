#include "btn.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_timer.h"

BTN::BTN(gpio_num_t pin)
    : _pin(pin), _debounce_ms(50), _long_press_ms(1000),
      _callback(nullptr), _stable_state(true), _long_press_fired(false),
      _timer(nullptr)
{
}

void BTN::begin(uint32_t debounce_ms, uint32_t long_press_ms)
{
    _debounce_ms = debounce_ms;
    _long_press_ms = long_press_ms;
    _stable_state = true;
    _long_press_fired = false;

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << _pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);

    _timer = xTimerCreate("btn_lp", pdMS_TO_TICKS(_long_press_ms),
                          pdFALSE, this, _timer_callback);
}

void BTN::setCallback(btn_callback_t cb)
{
    _callback = cb;
}

void BTN::enableInterrupt()
{
    gpio_install_isr_service(0);
    gpio_isr_handler_add(_pin, _isr_handler, this);
    gpio_set_intr_type(_pin, GPIO_INTR_ANYEDGE);
}

void BTN::disableInterrupt()
{
    gpio_set_intr_type(_pin, GPIO_INTR_DISABLE);
    gpio_isr_handler_remove(_pin);
}

bool BTN::isPressed()
{
    return gpio_get_level(_pin) == 0;
}

void IRAM_ATTR BTN::_isr_handler(void *arg)
{
    BTN *btn = static_cast<BTN *>(arg);
    uint32_t now = xTaskGetTickCountFromISR();

    static uint32_t last_isr_tick = 0;
    if ((now - last_isr_tick) < pdMS_TO_TICKS(btn->_debounce_ms))
        return;
    last_isr_tick = now;

    bool raw = gpio_get_level(btn->_pin);

    if (raw == 0 && btn->_stable_state == true)
    {
        btn->_stable_state = false;
        btn->_long_press_fired = false;

        if (btn->_timer)
            xTimerStartFromISR((TimerHandle_t)btn->_timer, nullptr);

        if (btn->_callback)
            btn->_callback(BTN_EVENT_PRESSED);
    }
    else if (raw == 1 && btn->_stable_state == false)
    {
        btn->_stable_state = true;

        if (btn->_timer)
            xTimerStopFromISR((TimerHandle_t)btn->_timer, nullptr);

        if (btn->_callback)
        {
            if (btn->_long_press_fired)
                btn->_callback(BTN_EVENT_RELEASED);
            else
                btn->_callback(BTN_EVENT_CLICK);
        }
    }
}

void BTN::_timer_callback(void *arg)
{
    BTN *btn = static_cast<BTN *>(arg);
    if (!btn->_stable_state && !btn->_long_press_fired)
    {
        btn->_long_press_fired = true;
        if (btn->_callback)
            btn->_callback(BTN_EVENT_LONG_PRESS);
    }
}
