#pragma once

#include "driver/gpio.h"

void init_encoder_button(gpio_num_t pin);
bool btn_is_pressed();
