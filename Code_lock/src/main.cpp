#include <Arduino.h>

#define BUTTON_1_PIN 4
#define BUTTON_2_PIN 17

void setup()
{
  Serial.begin(115200);
  delay(2000);

  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);

  // Disable built in LED
  digitalWrite(LED_BUILTIN, LOW);
}

bool buttonState1 = HIGH;
bool prevButtonState1 = HIGH;
bool buttonState2 = HIGH;
bool prevButtonState2 = HIGH;

void loop()
{
  prevButtonState1 = buttonState1;
  prevButtonState2 = buttonState2;

  buttonState1 = !digitalRead(BUTTON_1_PIN);
  buttonState2 = !digitalRead(BUTTON_2_PIN);

  if (buttonState1 && !prevButtonState1)
  {
    Serial.print(buttonState1);
    Serial.println("  ");
  }

  if (buttonState2 && !prevButtonState2)
  {
    Serial.print(buttonState2);
    Serial.println("  ");
  }
}