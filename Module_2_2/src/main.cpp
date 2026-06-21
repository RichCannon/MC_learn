#include <Arduino.h>

class AppConfig
{
public:
  // --- Pins ---
  static constexpr uint8_t RELAY_PIN_RECEIVER = 16;
  static constexpr uint8_t RELAY_PIN_TRIGGER = 4;

  // --- System settings ---
  static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
  static constexpr uint32_t STARTUP_DELAY_MS = 3e3;

  // ------------
  static constexpr uint32_t AMOUNT_OF_MEASURMENTS = 10;
  static constexpr uint32_t MEASURMENTS_DELAY = 200;
  static constexpr uint32_t RELAY_STARTUP_MS = 500;
};

void IRAM_ATTR relayTriggered();

volatile uint32_t numOfRelayTrigger = 0;
volatile uint32_t measurmentTs = 0;
volatile uint32_t combinedTime = 0;
volatile bool measurmentDone = 0;

uint32_t measur_delay_ts = 0;
uint16_t i = 0;
bool currentRelayTriggerState = false;

void setup()
{
  Serial.begin(AppConfig::SERIAL_BAUD_RATE);
  delay(AppConfig::STARTUP_DELAY_MS);

  pinMode(AppConfig::RELAY_PIN_RECEIVER, INPUT_PULLUP);
  pinMode(AppConfig::RELAY_PIN_TRIGGER, OUTPUT);
  digitalWrite(AppConfig::RELAY_PIN_TRIGGER, currentRelayTriggerState);

  delay(AppConfig::RELAY_STARTUP_MS);

  attachInterrupt(digitalPinToInterrupt(AppConfig::RELAY_PIN_RECEIVER), relayTriggered, FALLING);
}

void loop()
{
  uint32_t current_ts = millis();

  if (i == AppConfig::AMOUNT_OF_MEASURMENTS)
  {
    i++;
    float result = (combinedTime / 1000.0) / AppConfig::AMOUNT_OF_MEASURMENTS;
    Serial.println("======================");
    Serial.print("Avg time of ");
    Serial.print(AppConfig::AMOUNT_OF_MEASURMENTS);
    Serial.print(" measurments: ");
    Serial.println(result, 3);
    Serial.println("======================");
    Serial.print("Num of triggers: ");
    Serial.println(numOfRelayTrigger);
  }

  if (i < AppConfig::AMOUNT_OF_MEASURMENTS)
  {
    measur_delay_ts = millis();
    i++;
    Serial.print("Num of measurment:");
    Serial.println(i);

    digitalWrite(AppConfig::RELAY_PIN_TRIGGER, LOW);
    delay(AppConfig::MEASURMENTS_DELAY);

    currentRelayTriggerState = !currentRelayTriggerState;
    measurmentDone = false;
    measurmentTs = micros();
    digitalWrite(AppConfig::RELAY_PIN_TRIGGER, HIGH);
    delay(AppConfig::MEASURMENTS_DELAY);
  }
}

void IRAM_ATTR relayTriggered()
{
  if (!measurmentDone)
  {
    combinedTime = combinedTime + (micros() - measurmentTs);
    numOfRelayTrigger++;
    measurmentDone = true;
  }
}
