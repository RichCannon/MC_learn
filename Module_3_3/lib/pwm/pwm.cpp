#include "app_config.h"

void pwm_init(uint8_t PWM_PIN1, uint8_t PWM_PIN2)
{
  ledc_timer_config_t timer_config = {
      .speed_mode = AppConfig::PWM::SPEED_MODE,
      .duty_resolution = AppConfig::PWM::DUTY_RES,
      .timer_num = AppConfig::PWM::TIMER,
      .freq_hz = AppConfig::PWM::FREQ_HZ,
      .clk_cfg = AppConfig::PWM::CLK_CFG,
      .deconfigure = false
  };

  ledc_timer_config(&timer_config);

  ledc_channel_config_t channel_1_config = {
      .gpio_num = PWM_PIN1,
      .speed_mode = AppConfig::PWM::SPEED_MODE,
      .channel = AppConfig::PWM::CHANNEL_0,
      .intr_type = AppConfig::PWM::INTR_TYPE,
      .timer_sel = AppConfig::PWM::TIMER,
      .duty = 0,
      .hpoint = 0,
      .sleep_mode = AppConfig::PWM::SLEEP_MODE,
      .flags = 0,
      .deconfigure = false};

  ledc_channel_config_t channel_2_config = {
      .gpio_num = PWM_PIN2,
      .speed_mode = AppConfig::PWM::SPEED_MODE,
      .channel = AppConfig::PWM::CHANNEL_1,
      .intr_type = AppConfig::PWM::INTR_TYPE,
      .timer_sel = AppConfig::PWM::TIMER,
      .duty = 0,
      .hpoint = 0,
      .sleep_mode = AppConfig::PWM::SLEEP_MODE,
      .flags = 0,
      .deconfigure = false};

  ledc_channel_config(&channel_1_config);
  ledc_channel_config(&channel_2_config);
}