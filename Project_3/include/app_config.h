#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "env.h"

namespace AppConfig
{
    namespace ADC
    {
        constexpr uint8_t CHANNEL_COUNT = 1;
        constexpr adc_channel_t CHANNEL = ADC_CHANNEL_3;
        constexpr adc_unit_t UNIT = ADC_UNIT_1;
        constexpr adc_bitwidth_t BITWIDTH = ADC_BITWIDTH_12;
        constexpr int MAX_RAW_VALUE = 4095;

    }
    namespace MQTT
    {
        constexpr const char *BROKER_URI = ENV::BROKER_URI;
        constexpr const char *TOPIC = ENV::TOPIC;
        constexpr const char *CLIENT_ID = ENV::CLIENT_ID;
    }

    namespace WIFI
    {
        constexpr int MAX_RETRIES = 5;
        constexpr const char *SSID = ENV::WIFI_SSID;
        constexpr const char *PASS = ENV::WIFI_PASS;
        constexpr int CONNECTED_BIT = BIT0;
        constexpr int FAIL_BIT = BIT1;
        constexpr int TIMEOUT_MS = 10000;
    }

}
