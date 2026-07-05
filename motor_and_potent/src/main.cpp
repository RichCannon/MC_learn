#include <Arduino.h>

namespace AppConfig
{
  namespace Pins
  {
    constexpr uint8_t PIN_INPUT = 4;
    constexpr uint8_t PIN_OUT = 17;

  }
  namespace System
  {
    constexpr uint32_t SERIAL_BAUD_RATE = 115200;
    constexpr uint32_t STARTUP_DELAY_MS = 3000;
  }
  namespace PWM
  {
    constexpr uint8_t PWM_CHANNEL = 0;
    constexpr uint32_t PWM_FREQ = 1000;
    constexpr uint8_t PWM_RESOLUTION = 12;
    constexpr float PERCENTAGE_BOUNDS = .15f;
    constexpr uint8_t MAX_VALUE = 255;
    constexpr uint8_t MIN_VALUE = 0;
  }
  namespace Button
  {
    static constexpr uint32_t BUTTON_DELAY_MS = 50;
  }
  namespace Timer
  {
    static constexpr uint32_t PRESCALER = 80; // 80 МГц -> 1 мкС
    static constexpr uint32_t TIMER_RESPONSE_MICROSEC = 20000;
  }
}

uint16_t rawValue = 0;
uint16_t smoothedLdrValue = 0;

uint16_t parseRawValue(uint16_t rawValue)
{
  smoothedLdrValue = .1 * rawValue + .9 * smoothedLdrValue;
  // if (smoothedLdrValue < LOWEST_VALUE)
  //   return AppConfig::PWM::MIN_VALUE;
  // if (smoothedLdrValue > HIGHEST_VALUE)
  //   return AppConfig::PWM::MAX_VALUE;
  // return map(smoothedLdrValue, 0, 4096, 0, 255);
  return smoothedLdrValue;
}

void setup()
{
  Serial.begin(AppConfig::System::SERIAL_BAUD_RATE);
  delay(AppConfig::System::STARTUP_DELAY_MS);

  analogReadResolution(AppConfig::PWM::PWM_RESOLUTION);
  ledcSetup(AppConfig::PWM::PWM_CHANNEL, AppConfig::PWM::PWM_FREQ, AppConfig::PWM::PWM_RESOLUTION);
  ledcAttachPin(AppConfig::Pins::PIN_OUT, AppConfig::PWM::PWM_CHANNEL);
}

void loop()
{
  rawValue = analogRead(AppConfig::Pins::PIN_INPUT);
  ledcWrite(AppConfig::PWM::PWM_CHANNEL, parseRawValue(rawValue));

  Serial.print("POTENT VALUE: ");
  Serial.println(smoothedLdrValue);

  delay(20);
}
