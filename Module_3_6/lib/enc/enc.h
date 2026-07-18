#pragma once

#include "driver/pulse_cnt.h"
#include "app_config.h"

pcnt_unit_handle_t init_encoder(gpio_num_t gpio_a, gpio_num_t gpio_b);
void init_encoder_button(gpio_num_t gpio);