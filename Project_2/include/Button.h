
#include <Arduino.h>
#pragma once

enum class ButtonState
{
  UP,
  GOING_DOWN,
  DOWN,
  GOING_UP
};

class Button
{
private:
  ButtonState buttonState_{ButtonState::UP};
  ButtonState prevButtonState_{ButtonState::UP};
  uint8_t pin_;
  uint32_t delay_;
  uint32_t ts_{millis()};
  uint8_t mode_;

public:
  explicit Button(uint8_t pin, uint32_t delay, uint8_t mode = INPUT);

  void init();

  void listen();

  ButtonState getState() const
  {
    return buttonState_;
  }
  ButtonState getPrevState() const
  {
    return prevButtonState_;
  }
  bool isShortPressDown() const
  {
    return getState() == ButtonState::DOWN && getPrevState() == ButtonState::GOING_DOWN;
  }
};
