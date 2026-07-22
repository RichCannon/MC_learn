#pragma once

#include "driver/gpio.h"
#include <stdint.h>

typedef enum {
    BTN_EVENT_PRESSED,
    BTN_EVENT_RELEASED,
    BTN_EVENT_CLICK,
    BTN_EVENT_LONG_PRESS,
} btn_event_t;

typedef void (*btn_callback_t)(btn_event_t event);

class BTN {
public:
    BTN(gpio_num_t pin);
    void begin(uint32_t debounce_ms = 50, uint32_t long_press_ms = 1000);
    void setCallback(btn_callback_t cb);
    void enableInterrupt();
    void disableInterrupt();
    bool isPressed();

private:
    gpio_num_t _pin;
    uint32_t _debounce_ms;
    uint32_t _long_press_ms;
    btn_callback_t _callback;

    volatile bool _stable_state;
    volatile bool _long_press_fired;
    void *_timer;

    static void _isr_handler(void *arg);
    static void _timer_callback(void *arg);
    void _handle_edge();
};
