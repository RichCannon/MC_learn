#include "driver/i2c.h"
#include "app_config.h"
#include "esp_rom_sys.h"
#include "bme280_hal.h"
#include "esp_log.h"

static BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
                                             void *intf_ptr)
{
  uint8_t dev_addr = *(uint8_t *)intf_ptr;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg_addr, true);

  i2c_master_write(cmd, reg_data, len, true);

  i2c_master_stop(cmd);
  esp_err_t result = i2c_master_cmd_begin(AppConfig::I2C::PORT, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  return result == ESP_OK ? BME280_INTF_RET_SUCCESS : BME280_E_COMM_FAIL;
}

static BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  uint8_t dev_addr = *(uint8_t *)intf_ptr;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg_addr, true);

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
  i2c_master_read(cmd, reg_data, len, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);

  esp_err_t result = i2c_master_cmd_begin(AppConfig::I2C::PORT, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  return result == ESP_OK ? BME280_INTF_RET_SUCCESS : BME280_E_COMM_FAIL;
}

static void bme280_delay_us(uint32_t period, void *intf_ptr)
{
  esp_rom_delay_us(period);
}

void bme280_adapter_init(struct bme280_dev &dev, uint8_t i2c_addr)
{
  static uint8_t dev_addr = i2c_addr;

  dev.intf_ptr = &dev_addr;
  dev.intf = BME280_I2C_INTF;
  dev.read = bme280_i2c_read;
  dev.write = bme280_i2c_write;
  dev.delay_us = bme280_delay_us;

  int8_t result = bme280_init(&dev);

  if (result != BME280_OK)
  {
    ESP_LOGE("BME280", "bme280_init failed: %d", result);
    return;
  }

  ESP_LOGI("BME280", "Initialized, chip_id=0x%02X", dev.chip_id);
  ESP_LOGI("BME280", "Calib: T1=%u T2=%d T3=%d",
           dev.calib_data.dig_t1, dev.calib_data.dig_t2, dev.calib_data.dig_t3);

  uint8_t settings_sel = BME280_SEL_ALL_SETTINGS;
  struct bme280_settings settings = {};

  result = bme280_get_sensor_settings(&settings, &dev);
  if (result != BME280_OK)
  {
    ESP_LOGE("BME280", "get_sensor_settings failed: %d", result);
    return;
  }

  settings.osr_t = BME280_OVERSAMPLING_1X;
  settings.osr_p = BME280_OVERSAMPLING_1X;
  settings.osr_h = BME280_OVERSAMPLING_1X;
  settings.filter = BME280_FILTER_COEFF_OFF;
  settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

  result = bme280_set_sensor_settings(settings_sel, &settings, &dev);
  if (result != BME280_OK)
  {
    ESP_LOGE("BME280", "set_sensor_settings failed: %d", result);
    return;
  }

  result = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev);
  if (result != BME280_OK)
  {
    ESP_LOGE("BME280", "set_sensor_mode failed: %d", result);
    return;
  }

  ESP_LOGI("BME280", "Normal mode started");
}

int8_t bme280_adapter_read_sensor(struct bme280_dev &dev, struct bme280_data &data)
{
  return bme280_get_sensor_data(BME280_ALL, &data, &dev);
};