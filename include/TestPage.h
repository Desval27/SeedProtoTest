#pragma once

#include <BasePage.h>

#include <array>

template <std::size_t ENCODER_COUNT = 1, std::size_t BUTTON_COUNT = 4,
          std::size_t POT_COUNT = 4>
class TestPage : public BasePage {
public:
  TestPage() : BasePage() {}

  bool OnEncoderTurned(uint16_t encoderID, int16_t turns,
                       uint16_t stepsPerRevolution) override {
    if (encoderID < ENCODER_COUNT) {
      encoderValues[encoderID] = turns;
      encoderSteps[encoderID] = stepsPerRevolution;
      return true;
    }
    return false;
  }

  bool OnEncoderActivityChanged(uint16_t encoderID,
                                        bool isCurrentlyActive) override {
    if (encoderID < ENCODER_COUNT) {
      if (!isCurrentlyActive) {
        encoderValues[encoderID] = 0;
        encoderSteps[encoderID] = 0;
      }
      return true;
    }
    return false;
  }

  bool OnButton(uint16_t buttonID, uint8_t numberOfPresses,
                bool isRetriggering) override {
    if (buttonID < BUTTON_COUNT) {
      buttonValues[buttonID] = numberOfPresses;
      buttonRetrig[buttonID] = isRetriggering;
      return true;
    }
    return false;
  }

  bool OnPotMoved(uint16_t potID, float newPosition) override {
    if (potID < POT_COUNT) {
      potValues[potID] = newPosition;
      return true;
    }
    return false;
  }

protected:
  void InternalDraw(daisy::OneBitGraphicsDisplay &display,
                    const daisy::Rectangle bounds) override {
    Rectangle textRect = bounds.WithHeight(12);
    constexpr uint16_t textH = 8;
    display.DrawRect(textRect, true, false);
    display.WriteStringAligned("Control Test", Font_6x8, textRect,
                               Alignment::centered, true);

    FixedCapStr<48> text;
    uint16_t lineY = textRect.GetBottom() + 1;

    // Encoders
    for (size_t i = 0; i < ENCODER_COUNT; i++) {
      text.AppendInt(i + 1);
      text.Append(":");
      text.AppendInt(encoderValues[i], true);
      text.Append("_");
      text.AppendInt(encoderSteps[i]);
      if ((i % 2) == 1 || i == ENCODER_COUNT - 1) {
        display.SetCursor(0, lineY);
        display.WriteString(text.Cstr(), Font_6x8, true);
        text.Clear();
        lineY += textH;
      } else {
        text.Append("  ");
      }
    }
    lineY += 2;

    // Buttons
    for (size_t i = 0; i < BUTTON_COUNT; i++) {
      text.AppendInt(i + 1);
      text.Append(":");
      text.AppendInt(buttonValues[i]);
      if (buttonRetrig[i])
        text.Append('Y');
      else
        text.Append('N');
      if ((i % 3) == 1 || i == BUTTON_COUNT - 1) {
        display.SetCursor(0, lineY);
        display.WriteString(text.Cstr(), Font_6x8, true);
        text.Clear();
        lineY += textH;
      } else {
        text.Append("  ");
      }
    }
    lineY += 2;

    // Potentiometers
    for (size_t i = 0; i < POT_COUNT; i++) {
      text.AppendInt(i + 1);
      text.Append(":");
      text.AppendFloat(potValues[i], 4);
      if ((i % 2) == 1 || i == POT_COUNT - 1) {
        display.SetCursor(0, lineY);
        display.WriteString(text.Cstr(), Font_6x8, true);
        text.Clear();
        lineY += textH;
      } else {
        text.Append("  ");
      }
    }
  }

private:
  std::array<int16_t, ENCODER_COUNT> encoderValues{};
  std::array<int16_t, ENCODER_COUNT> encoderSteps{};
  std::array<uint8_t, BUTTON_COUNT> buttonValues{};
  std::array<bool, BUTTON_COUNT> buttonRetrig{};
  std::array<float, POT_COUNT> potValues{};
};
