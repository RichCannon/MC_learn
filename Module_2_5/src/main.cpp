#include <Arduino.h>

class AppConfig
{
public:
  // --- Pins ---
  static constexpr uint8_t PIN_OUTPUT = 16;

  // --- System settings ---
  static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
  static constexpr uint32_t STARTUP_DELAY_MS = 3e3;

  // ------------
  // static constexpr uint32_t TIME_IN_MICSEC = 15 * 60 * 1e6; // 15 хв (Значенния для завдання)
  static constexpr uint32_t TIME_IN_MICSEC = 2 * 1e6; // 2 с (Значення для тесту 8с - Увімкнений; 2с - Вимкнений)
  static constexpr uint32_t PRESCALER = 80;           // 80 МГц -> 1 мкС
};
hw_timer_s *ventTimer = NULL;

volatile uint8_t i = 0;

void IRAM_ATTR onTimer()
{
  i++;
}

void setup()
{
  Serial.begin(AppConfig::SERIAL_BAUD_RATE);
  delay(AppConfig::STARTUP_DELAY_MS);

  digitalWrite(AppConfig::PIN_OUTPUT, HIGH);

  ventTimer = timerBegin(0, AppConfig::PRESCALER, true);

  timerAttachInterrupt(ventTimer, &onTimer, true);
  // timerAlarmWrite(ventTimer, 15 * 60 * 1e6, true);
  timerAlarmWrite(ventTimer, AppConfig::TIME_IN_MICSEC, true);
  timerAlarmEnable(ventTimer);
}

void loop()
{
  if (i == 3) // Коли таймер спрацював 4 рази по 15 хв (пройшла 1 ш)
  {
    i++; // Збільшуємо значення щоб в цю умову більше не заходили
    digitalWrite(AppConfig::PIN_OUTPUT, LOW);
    Serial.println("Vent is on pause");
  }
  else if (i > 4)
  { // Коли таймер спрацював 5 разів (пройшло 1г 15хв)
    digitalWrite(AppConfig::PIN_OUTPUT, HIGH);
    Serial.println("Vent has been resumed");
    i = 0;
  }
}
