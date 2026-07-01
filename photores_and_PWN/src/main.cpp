#include <Arduino.h>
#include "Button.h"

namespace AppConfig
{
  namespace Pins
  {
    constexpr uint8_t LED_PIN_OUTPUT = 16;
    constexpr uint8_t PIN_INPUT = 4;
    constexpr uint8_t BUTTON_PIN = 17;

  }
  namespace System
  {
    constexpr uint32_t SERIAL_BAUD_RATE = 115200;
    constexpr uint32_t STARTUP_DELAY_MS = 3000;
  }
  namespace PWM
  {
    constexpr uint8_t PWM_CHANNEL = 0;
    constexpr uint32_t PWM_FREQ = 5000;
    constexpr uint8_t PWM_RESOLUTION = 8;
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

const uint8_t LOWEST_VALUE = round(AppConfig::PWM::MAX_VALUE * AppConfig::PWM::PERCENTAGE_BOUNDS);
const uint8_t HIGHEST_VALUE = round(AppConfig::PWM::MAX_VALUE * (1 - AppConfig::PWM::PERCENTAGE_BOUNDS));
hw_timer_s *ventTimer = NULL;

volatile bool readyToRead = false;
uint8_t rawValue = 0;
uint8_t smoothedLdrValue = 0;
bool isOn = true;

Button button(AppConfig::Pins::BUTTON_PIN, AppConfig::Button::BUTTON_DELAY_MS);

void IRAM_ATTR onTimer()
{
  readyToRead = true;
}

uint8_t parseRawValue(uint8_t rawValue)
{
  smoothedLdrValue = .1 * rawValue + .9 * smoothedLdrValue;
  if (smoothedLdrValue < LOWEST_VALUE)
    return AppConfig::PWM::MIN_VALUE;
  if (smoothedLdrValue > HIGHEST_VALUE)
    return AppConfig::PWM::MAX_VALUE;
  return smoothedLdrValue;
}
void setup()
{
  Serial.begin(AppConfig::System::SERIAL_BAUD_RATE);
  delay(AppConfig::System::STARTUP_DELAY_MS);

  analogReadResolution(AppConfig::PWM::PWM_RESOLUTION);

  ledcSetup(AppConfig::PWM::PWM_CHANNEL, AppConfig::PWM::PWM_FREQ, AppConfig::PWM::PWM_RESOLUTION);

  ledcAttachPin(AppConfig::Pins::LED_PIN_OUTPUT, AppConfig::PWM::PWM_CHANNEL);

  ventTimer = timerBegin(0, AppConfig::Timer::PRESCALER, true);

  timerAttachInterrupt(ventTimer, &onTimer, true);
  timerAlarmWrite(ventTimer, AppConfig::Timer::TIMER_RESPONSE_MICROSEC, true);
  timerAlarmEnable(ventTimer);

  button.init();
}

void loop()
{
  button.listen();

  if (button.isShortPressDown())
  {
    isOn = !isOn;
  }

  if (readyToRead && isOn)
  {
    readyToRead = false;
    rawValue = analogRead(AppConfig::Pins::PIN_INPUT);
    ledcWrite(AppConfig::PWM::PWM_CHANNEL, parseRawValue(rawValue));
  }
}
