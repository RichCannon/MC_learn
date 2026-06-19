#include <Arduino.h>

class AppConfig
{
public:
  // --- Pins ---
  static constexpr uint8_t LED_GREEN = 4;
  static constexpr uint8_t LED_BLUE = 16;
  static constexpr uint8_t LED_RED = 17;

  // --- System settings ---
  static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
  static constexpr uint32_t STARTUP_DELAY_MS = 1e3;

  // --- Blink delays ---
  static constexpr uint16_t LED_GREEN_DELAY = 200;
  static constexpr uint16_t LED_BLUE_DELAY = 500;
  static constexpr uint16_t LED_RED_DELAY = 1000;
};

enum class LedState
{
  OFF,
  ON
};

class Led
{
private:
  LedState state_{LedState::OFF};
  uint8_t pin_;
  uint32_t delay_;
  uint32_t ts{millis()};

public:
  explicit Led(uint8_t pin, uint32_t delay) : pin_(pin), delay_(delay) {};

  void init()
  {
    pinMode(pin_, OUTPUT);
  };

  void changeLedState(LedState newLedState)
  {
    state_ = newLedState;
    digitalWrite(pin_, state_ == LedState::ON ? HIGH : LOW);
  }

  void blink()
  {
    uint32_t currentTS = millis();
    if (currentTS - ts > delay_)
    {
      ts = currentTS;
      changeLedState(state_ == LedState::ON ? LedState::OFF : LedState::ON);
    }
  }
};

Led led_green(AppConfig::LED_GREEN, AppConfig::LED_GREEN_DELAY);
Led led_blue(AppConfig::LED_BLUE, AppConfig::LED_BLUE_DELAY);
Led led_red(AppConfig::LED_RED, AppConfig::LED_RED_DELAY);

void setup()
{
  Serial.begin(AppConfig::SERIAL_BAUD_RATE);
  delay(AppConfig::STARTUP_DELAY_MS);

  led_green.init();
  led_blue.init();
  led_red.init();
}

void loop()
{

  led_green.blink();
  led_blue.blink();
  led_red.blink();
}
