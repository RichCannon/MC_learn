#include <Arduino.h>

class AppConfig
{
public:
  // --- Pins ---
  static constexpr uint8_t RELAY_PIN_RECEIVER = 4;
  static constexpr uint8_t RELAY_PIN_TRIGGER = 19;

  // --- System settings ---
  static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
  static constexpr uint32_t STARTUP_DELAY_MS = 1e3;

  // ------------
  static constexpr float AMOUNT_OF_MEASURMENTS = 10;
  static constexpr uint32_t MEASURMENTS_DELAY = 1e3;
};

void ARDUINO_ISR_ATTR relayTriggered();

void setup()
{
  Serial.begin(AppConfig::SERIAL_BAUD_RATE);
  delay(AppConfig::STARTUP_DELAY_MS);

  pinMode(AppConfig::RELAY_PIN_RECEIVER, INPUT_PULLUP);
  pinMode(AppConfig::RELAY_PIN_TRIGGER, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(AppConfig::RELAY_PIN_RECEIVER), relayTriggered, FALLING);
}

uint32_t measur_delay_ts = 0;
uint32_t measurment_ts = 0;
volatile float combinedTime = 0;
uint16_t i = 0;

void loop()
{
  uint32_t current_ts = millis();

  if (i == AppConfig::AMOUNT_OF_MEASURMENTS)
  {
    i++;
    const float result = combinedTime / AppConfig::AMOUNT_OF_MEASURMENTS;
    Serial.print("Avg time of ");
    Serial.print(AppConfig::AMOUNT_OF_MEASURMENTS);
    Serial.print("measurments: ");
    Serial.println(result);
  }
  if (current_ts - measur_delay_ts > AppConfig::MEASURMENTS_DELAY && i < AppConfig::AMOUNT_OF_MEASURMENTS)
  {
    measur_delay_ts = millis();
    Serial.print("Num of measurment:");
    Serial.println(i);
    i++;

    digitalWrite(AppConfig::RELAY_PIN_TRIGGER, HIGH);
    measurment_ts = millis();
  }
}

void ARDUINO_ISR_ATTR relayTriggered()
{
  digitalWrite(AppConfig::RELAY_PIN_TRIGGER, LOW);
  uint32_t current_ts = millis();
  combinedTime += (current_ts - measurment_ts);
}
