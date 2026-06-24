
#include <Arduino.h>

// ========== TASK 1 =============

class AppConfig
{
public:
  // --- Pins ---
  static constexpr uint8_t BUTTON_PIN = 4;

  // --- System settings ---
  static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
  static constexpr uint32_t STARTUP_DELAY_MS = 1e3;
};

volatile uint32_t buttonPressedCount = 0;

void IRAM_ATTR handleButtonPressDown()
{
  ++buttonPressedCount;
}

void setup()
{
  Serial.begin(AppConfig::SERIAL_BAUD_RATE);
  delay(AppConfig::STARTUP_DELAY_MS);
  pinMode(AppConfig::BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(AppConfig::BUTTON_PIN), handleButtonPressDown, FALLING);
}

uint32_t ts = millis();

void loop()
{
  Serial.print("Button pressed times: ");
  Serial.println(buttonPressedCount);
  delay(500);
}

// ========== TASK 2 =============

// class AppConfig
// {
// public:
//   // --- Pins ---
//   static constexpr uint8_t BUTTON_PIN = 4;

//   // --- System settings ---
//   static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
//   static constexpr uint32_t STARTUP_DELAY_MS = 1e3;

//   static constexpr uint32_t BUTTON_DEBOUNCE = 25;
// };

// volatile bool isPressed = false;
// uint32_t buttonPressedCount = 0;

// void IRAM_ATTR handleButtonPressDown()
// {
//   isPressed = true;
// }

// void setup()
// {
//   Serial.begin(AppConfig::SERIAL_BAUD_RATE);
//   delay(AppConfig::STARTUP_DELAY_MS);
//   pinMode(AppConfig::BUTTON_PIN, INPUT_PULLUP);
//   attachInterrupt(digitalPinToInterrupt(AppConfig::BUTTON_PIN), handleButtonPressDown, FALLING);
// }

// uint32_t ts = millis();

// void loop()
// {
//   if (isPressed)
//   {
//     isPressed = false;
//     const uint32_t currentTs = millis();
//     if (currentTs - ts >= AppConfig::BUTTON_DEBOUNCE)
//     {
//       ts = millis();
//       buttonPressedCount++;
//       Serial.print("Button pressed times: ");
//       Serial.println(buttonPressedCount);
//     }
//   }
// }

// ========== TASK 3 =============

// class AppConfig
// {
// public:
//   // --- Pins ---
//   static constexpr uint8_t BUTTON_PIN = 4;

//   // --- System settings ---
//   static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
//   static constexpr uint32_t STARTUP_DELAY_MS = 1e3;
// };

// volatile bool buttonPressed = false;
// uint32_t buttonPressedCount = 0;

// void IRAM_ATTR handleButtonPressDown()
// {
//   buttonPressed = true;
// }

// void setup()
// {
//   Serial.begin(AppConfig::SERIAL_BAUD_RATE);
//   delay(AppConfig::STARTUP_DELAY_MS);
//   pinMode(AppConfig::BUTTON_PIN, INPUT_PULLUP);
//   attachInterrupt(digitalPinToInterrupt(AppConfig::BUTTON_PIN), handleButtonPressDown, FALLING);
// }

// void loop()
// {
//   if (buttonPressed)
//   {
//     buttonPressedCount++;
//     buttonPressed = false;
//     Serial.print("Button pressed times: ");
//     Serial.println(buttonPressedCount);
//   }
// }

// ========== TASK 4 =============

// class AppConfig
// {
// public:
//   // --- Pins ---
//   static constexpr uint8_t BUTTON_PIN = 4;

//   // --- System settings ---
//   static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
//   static constexpr uint32_t STARTUP_DELAY_MS = 1e3;

//   static constexpr uint32_t BUTTON_DELAY_MS = 50;
// };

// enum class ButtonState
// {
//   UP,
//   GOING_DOWN,
//   DOWN,
//   GOING_UP
// };

// class Button
// {
// private:
//   ButtonState buttonState_{ButtonState::UP};
//   ButtonState prevButtonState_{ButtonState::UP};
//   uint8_t pin_;
//   uint32_t delay_;
//   uint32_t ts_{millis()};

// public:
//   explicit Button(uint8_t pin, uint32_t delay) : pin_(pin), delay_(delay) {};

//   void init()
//   {
//     pinMode(AppConfig::BUTTON_PIN, INPUT_PULLUP);
//   }

//   void listen()
//   {
//     const uint32_t isPressed = (digitalRead(pin_) == LOW);
//     const uint32_t currentTs = millis();
//     prevButtonState_ = buttonState_;
//     switch (buttonState_)
//     {
//     case ButtonState::UP:
//       if (isPressed)
//       {
//         buttonState_ = ButtonState::GOING_DOWN;
//         ts_ = millis();
//       };
//       break;
//     case ButtonState::GOING_DOWN:
//       if (currentTs - ts_ >= delay_)
//       {
//         if (isPressed)
//         {
//           buttonState_ = ButtonState::DOWN;
//         }
//         else
//         {
//           buttonState_ = ButtonState::UP;
//         }
//         break;
//       }
//     case ButtonState::DOWN:
//       if (!isPressed)
//       {
//         buttonState_ = ButtonState::GOING_UP;
//         ts_ = millis();
//       }
//       break;
//     case ButtonState::GOING_UP:
//       if (currentTs - ts_ >= delay_)
//       {
//         if (isPressed)
//         {
//           buttonState_ = ButtonState::DOWN;
//         }
//         else
//         {
//           buttonState_ = ButtonState::UP;
//         }
//         break;
//       }

//     default:
//       break;
//     }
//   }

//   ButtonState getState() const
//   {
//     return buttonState_;
//   }
//   ButtonState getPrevState() const
//   {
//     return prevButtonState_;
//   }
// };

// Button button(AppConfig::BUTTON_PIN, AppConfig::BUTTON_DELAY_MS);

// uint32_t buttonPressedCount = 0;

// void setup()
// {
//   Serial.begin(AppConfig::SERIAL_BAUD_RATE);
//   delay(AppConfig::STARTUP_DELAY_MS);
//   button.init();
// }

// void loop()
// {
//   button.listen();

//   if (button.getState() == ButtonState::DOWN && button.getPrevState() == ButtonState::GOING_DOWN)
//   {
//     buttonPressedCount++;
//     Serial.print("Button pressed times: ");
//     Serial.println(buttonPressedCount);
//   }
// }
