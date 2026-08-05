#include <cstdint>
#include "bme280.h"

void bme280_adapter_init(struct bme280_dev &dev, uint8_t i2c_addr);
int8_t bme280_adapter_read_sensor(struct bme280_dev &dev, struct bme280_data &data);