#include <Arduino.h>
#include "Button.h"

Button::Button(uint8_t pin, uint32_t delay, uint8_t mode) : pin_(pin), delay_(delay), mode_(mode) {};

void Button::init()
{
  pinMode(pin_, mode_);
}

void Button::listen()
{
  const uint8_t activeState = (mode_ == INPUT_PULLUP) ? LOW : HIGH;
  const bool isPressed = (digitalRead(pin_) == activeState);
  const uint32_t currentTs = millis();

  prevButtonState_ = buttonState_;
  switch (buttonState_)
  {
  case ButtonState::UP:
    if (isPressed)
    {
      buttonState_ = ButtonState::GOING_DOWN;
      ts_ = millis();
    };
    break;
  case ButtonState::GOING_DOWN:
    if (currentTs - ts_ >= delay_)
    {
      if (isPressed)
      {
        buttonState_ = ButtonState::DOWN;
      }
      else
      {
        buttonState_ = ButtonState::UP;
      }
    }
    break;
  case ButtonState::DOWN:
    if (!isPressed)
    {
      buttonState_ = ButtonState::GOING_UP;
      ts_ = millis();
    }
    break;
  case ButtonState::GOING_UP:
    if (currentTs - ts_ >= delay_)
    {
      if (isPressed)
      {
        buttonState_ = ButtonState::DOWN;
      }
      else
      {
        buttonState_ = ButtonState::UP;
      }
    }
    break;

  default:
    break;
  }
}
