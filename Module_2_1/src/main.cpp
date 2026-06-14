#include <Arduino.h>

class AppConfig
{
public:
  // --- Pins ---
  static constexpr uint8_t LED_PIN = 4;
  static constexpr uint8_t BUTTON_PIN = 0;

  // --- LED settings ---
  static constexpr uint32_t LED_DEFAULT_DELAY_MS = 500;
  static constexpr uint8_t SHORT_PRESS_BLINKS = 3; // Кількість блимань при короткому натисканні

  // --- Buttons settings ---
  static constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;

  // --- System settings ---
  static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
  static constexpr uint32_t STARTUP_DELAY_MS = 1e3;

  // --- Loop tick delay measure settings ---
  static constexpr uint32_t LOOP_TICKS_TO_MEASURE = 1e6;
};

void buttonClick();

enum class LedState
{
  OFF,
  ON
};

enum class LedMode
{
  BLINK,
  STATIC_LOW,
  STATIC_HIGH,
};

class Led
{
private:
  LedState state_{LedState::OFF};
  uint8_t pin_;
  uint32_t delay_;
  uint32_t ts{0};

public:
  static uint32_t blinksDuringPressedState;

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

  void setNewBlinkDelayMs(uint32_t delayMs)
  {
    delay_ = delayMs;
  }

  LedState getLedState()
  {
    return state_;
  }
};

class LoopTicksMeasure
{
private:
  float loopTs_ = 0;
  uint32_t loopTicks_ = 0;
  uint32_t loopTicksToMeasure_ = 0;

public:
  explicit LoopTicksMeasure(uint32_t loopTicksToMeasure) : loopTicksToMeasure_(loopTicksToMeasure) {}
  void getMeasurments()
  {
    loopTicks_ += 1;
    if (loopTicks_ >= loopTicksToMeasure_)
    {
      float currentTs = millis();
      float avgTickTime = (currentTs - loopTs_) / loopTicksToMeasure_;
      uint32_t tickTimeForAllLoops = (currentTs - loopTs_);
      loopTs_ = currentTs;
      loopTicks_ = 0;

      Serial.println("====================");
      Serial.print("Time for 1 loop: ");
      Serial.println(avgTickTime, 6);
      Serial.print("Time for ");
      Serial.print(loopTicksToMeasure_);
      Serial.print(" loops: ");
      Serial.println(tickTimeForAllLoops);
    }
  }
};

LoopTicksMeasure loopTickMeasurments(AppConfig::LOOP_TICKS_TO_MEASURE);
Led led(AppConfig::LED_PIN, AppConfig::LED_DEFAULT_DELAY_MS);

volatile LedMode ledMode = LedMode::BLINK;

uint32_t buttonTs = 0;

void ARDUINO_ISR_ATTR buttonClick()
{
  Serial.println("Button pressed:");
  uint32_t currentTime = millis();
  if (currentTime - buttonTs < 100)
  {
    return;
  }
  buttonTs = currentTime;
  switch (ledMode)
  {
  case LedMode::BLINK:
    ledMode = LedMode::STATIC_HIGH;
    break;
  case LedMode::STATIC_HIGH:
    ledMode = LedMode::STATIC_LOW;
    break;
  default:
    ledMode = LedMode::BLINK;
    break;
  }
}

void setup()
{
  Serial.begin(AppConfig::SERIAL_BAUD_RATE);
  delay(AppConfig::STARTUP_DELAY_MS);

  led.init();
  pinMode(AppConfig::BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(AppConfig::BUTTON_PIN), buttonClick, FALLING);
}

uint32_t buttonCheckTs = 0;

void loop()
{

  loopTickMeasurments.getMeasurments();
  switch (ledMode)
  {
  case LedMode::BLINK:
    led.blink();
    break;
  case LedMode::STATIC_HIGH:
    led.changeLedState(LedState::ON);
    break;
  default:
    led.changeLedState(LedState::OFF);
    break;
  }
}
