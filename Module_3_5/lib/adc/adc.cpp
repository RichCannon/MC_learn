#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "app_config.h"
#include "adc.h"

void adc_init(adc_oneshot_unit_handle_t *adc1_handle)
{
	adc_oneshot_unit_init_cfg_t init_config1 = {
			.unit_id = ADC_UNIT_1,
			.clk_src = ADC_RTC_CLK_SRC_DEFAULT,
			.ulp_mode = ADC_ULP_MODE_DISABLE,
	};

	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, adc1_handle));

	adc_oneshot_chan_cfg_t config = {
			.atten = AppConfig::ADC::ATTEN,
			.bitwidth = AppConfig::ADC::BITWIDTH,
	};

	ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc1_handle, AppConfig::ADC::CHANNEL, &config));
}

void adc_calibration_init(adc_cali_handle_t *cali_handle)
{
	adc_cali_curve_fitting_config_t cali_config = {
			.unit_id = ADC_UNIT_1,
			.chan = AppConfig::ADC::CHANNEL,
			.atten = AppConfig::ADC::ATTEN,
			.bitwidth = AppConfig::ADC::BITWIDTH,
	};

	ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, cali_handle));
}
