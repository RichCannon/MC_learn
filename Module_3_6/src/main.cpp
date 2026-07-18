#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "app_config.h"
#include "enc.h"
#include "btn.h"
#include "pwm.h"

static const char *TAG = "Encoder";

static int calc_servo_duty(int enc_val)

{
	return (((enc_val / AppConfig::ENC::STEPS_PER_CLICK) + (AppConfig::ENC::HALF_ROTATE_STEPS / AppConfig::ENC::STEPS_PER_CLICK)) * AppConfig::Servo::STEP_SIZE) + AppConfig::Servo::MIN;
}

int servoDutyValue = calc_servo_duty(0);

int current_enc_val = 0;
int prev_enc_val = -1;

extern "C" void app_main()
{
	// Викликаємо ініціалізацію та отримуємо "дескриптор" лічильника
	pcnt_unit_handle_t encoder = init_encoder(AppConfig::PINS::ENCODER_A_INPUT, AppConfig::PINS::ENCODER_B_INPUT);
	init_encoder_button(AppConfig::PINS::ENCODER_BUTTON_INPUT);
	pwm_init(AppConfig::PINS::SERVO_OUTPUT);

	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(20));
		// Читаємо поточне значення лічильника енкодера
		ESP_ERROR_CHECK(pcnt_unit_get_count(encoder, &current_enc_val));

		int a_state = gpio_get_level((gpio_num_t)AppConfig::PINS::ENCODER_A_INPUT);
		int b_state = gpio_get_level((gpio_num_t)AppConfig::PINS::ENCODER_B_INPUT);
		if (btn_is_pressed())
		{
			ESP_LOGI(TAG, "Button pressed!");
			current_enc_val = 0;
			pcnt_unit_clear_count(encoder);
		}

		if (current_enc_val == prev_enc_val || current_enc_val % AppConfig::ENC::STEPS_PER_CLICK != 0 || current_enc_val > AppConfig::ENC::HALF_ROTATE_STEPS || current_enc_val < AppConfig::ENC::HALF_ROTATE_STEPS * -1)
		{
			continue;
		}
		prev_enc_val = current_enc_val;

		servoDutyValue = calc_servo_duty(current_enc_val);

		ledc_set_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL, servoDutyValue);
		ledc_update_duty(AppConfig::PWM::SPEED_MODE, AppConfig::PWM::CHANNEL);

		ESP_LOGI(TAG, "Position: %d | A: %d | B: %d", current_enc_val, a_state, b_state);
		ESP_LOGI(TAG, "Duty value: %d ", servoDutyValue);
	}
}