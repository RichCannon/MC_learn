#include <Arduino.h>
#include <Preferences.h>

Preferences preferences;

// put function declarations here:

#define LED_RED 13
#define LED_GREEN 18
#define BUTTON_1_PIN 4
#define BUTTON_2_PIN 17

const uint8_t BUTTON_ERROR_PREVENT_DELAY = 100;
const uint16_t DELAY_BEFORE_CODE_RESET = 5000;
const uint8_t BLINKS_SPEED = 200;
const uint8_t BLINKS_COUNT = 3;
const uint8_t double_hold_check_cycles_success = 15; // 3000 ms / 200 ms = 15;

const uint16_t LOG_DELAY = 300;

const bool BUTTON_A_VALUE = 0;
const bool BUTTON_B_VALUE = 1;

typedef struct
{
  uint8_t pin;
  bool input_value;
  bool button_state;
  bool prev_button_state;
  uint32_t ts;
} Button;

void changeInputValue(uint8_t input_value);
void blinkWithLed(uint8_t LED_PIN);
void registerButtonCheck(Button *btn);
void resetSequenceEntering();

uint8_t target_code;
uint8_t input_sequence = 0;
uint8_t button_presses = 0;
uint32_t test_ts = millis();
uint32_t double_hold_check_ts = millis();
uint8_t double_hold_check_cycles = 0;
bool code_reset_enabled = false;

uint32_t ts = millis();

Button btn1 = {
    BUTTON_1_PIN,
    HIGH,
    0,
};
Button btn2 = {BUTTON_2_PIN, BUTTON_B_VALUE, HIGH, 0};

void changeInputValue(uint8_t input_value)
{
  input_sequence = (input_sequence << 1) | input_value;
  button_presses++;
}

void blinkWithLed(uint8_t LED_PIN)
{
  uint8_t i = 0;
  while (BLINKS_COUNT > i)
  {
    i++;
    digitalWrite(LED_PIN, HIGH);
    delay(BLINKS_SPEED);
    digitalWrite(LED_PIN, LOW);
    delay(BLINKS_SPEED);
  };
}

void registerButtonCheck(Button *btn)
{
  const bool buttonState = digitalRead(btn->pin);
  if (millis() - btn->ts > BUTTON_ERROR_PREVENT_DELAY)
  {
    btn->prev_button_state = btn->button_state;
    btn->button_state = buttonState;
    btn->ts = millis();
    if (buttonState == LOW && btn->prev_button_state == HIGH)
    {
      ts = millis(); // Reset the global timeout for DELAY_BEFORE_CODE_RESET
      changeInputValue(btn->input_value);
    }
  }
}

void resetSequenceEntering()
{
  input_sequence = 0;
  button_presses = 0;
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  preferences.begin("my-app", false);
  target_code = preferences.getUInt("target_code", 0b0110);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);

  // Disable built in LED
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  // Log input input_value into serial monitor
  if (millis() - test_ts > LOG_DELAY)
  {
    test_ts = millis();
    Serial.print("Input value: ");
    Serial.println(input_sequence, BIN);
    Serial.print("Button pressed value: ");
    Serial.println(button_presses);
  }

  registerButtonCheck(&btn1);
  registerButtonCheck(&btn2);

  if (code_reset_enabled)
  {
    if (button_presses >= 4)
    {
      target_code = input_sequence;
      preferences.putUInt("target_code", target_code);
      code_reset_enabled = false;
      resetSequenceEntering();
      digitalWrite(LED_RED, LOW);
      blinkWithLed(LED_GREEN);
    }
  }
  else
  {
    if (btn1.button_state == LOW && btn2.button_state == LOW)
    {
      if (millis() - double_hold_check_ts > 200)
      {
        resetSequenceEntering();
        double_hold_check_ts = millis();
        if (double_hold_check_cycles_success <= double_hold_check_cycles)
        {
          double_hold_check_cycles = 0;
          Serial.println("Code change request");
          code_reset_enabled = true;
          uint8_t i = 0;
          while (BLINKS_COUNT > i)
          {
            i++;
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_RED, HIGH);
            delay(BLINKS_SPEED);
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_RED, LOW);
            delay(BLINKS_SPEED);
          };
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_RED, HIGH);
        }
        else
        {
          double_hold_check_cycles++;
        }
      }
    }
    else
    {

      if (button_presses >= 4)
      {
        Serial.print("Full input value: ");
        Serial.println(input_sequence, BIN);
        if (input_sequence == target_code)
        {
          Serial.println("CORRECT CODE");
          blinkWithLed(LED_GREEN);
        }
        else
        {
          Serial.println("WRONG CODE");
          blinkWithLed(LED_RED);
        }
        resetSequenceEntering();
      }

      if (millis() - ts > DELAY_BEFORE_CODE_RESET && button_presses != 0)
      {
        resetSequenceEntering();
        blinkWithLed(LED_RED);
      }
    }
  }
}