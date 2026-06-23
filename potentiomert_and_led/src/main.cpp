#include <Arduino.h>

class AppConfig
{
public:
  // --- Pins ---
  static constexpr uint8_t LED_PIN_OUTPUT = 16;
  static constexpr uint8_t PIN_INPUT = 4;

  // --- System settings ---
  static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
  static constexpr uint32_t STARTUP_DELAY_MS = 3e3;

  // ------------
  static constexpr uint8_t PWM_CHANNEL = 0;
  static constexpr uint32_t PWM_FREQ = 5000;
  static constexpr uint8_t PWM_RESOLUTION = 8;
};
hw_timer_s *adcTimer = NULL;

volatile bool readyToRead = false;
uint32_t potValue = 0;

void IRAM_ATTR onTimer()
{
  readyToRead = true;
}

void setup()
{
  Serial.begin(AppConfig::SERIAL_BAUD_RATE);
  delay(AppConfig::STARTUP_DELAY_MS);

  analogReadResolution(AppConfig::PWM_RESOLUTION);

  ledcSetup(AppConfig::PWM_CHANNEL, AppConfig::PWM_FREQ, AppConfig::PWM_RESOLUTION);

  ledcAttachPin(AppConfig::LED_PIN_OUTPUT, AppConfig::PWM_CHANNEL);

  adcTimer = timerBegin(0, 80, true);

  timerAttachInterrupt(adcTimer, &onTimer, true);
  timerAlarmWrite(adcTimer, 20000, true);
  timerAlarmEnable(adcTimer);
}

void loop()
{
  if (readyToRead)
  {
    readyToRead = false;
    potValue = analogRead(AppConfig::PIN_INPUT);
    ledcWrite(AppConfig::PWM_CHANNEL, potValue);
  }
}
