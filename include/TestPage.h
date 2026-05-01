#pragma once

#include <BasePage.h>

#include <array>

constexpr int DOUBLE_CLICK_TIMEOUT = 3000;

template <std::size_t ENCODER_COUNT = 1, std::size_t BUTTON_COUNT = 4,
          std::size_t POT_COUNT = 4>
class TestPage : public BasePage<true> {
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
    // Menu encoder button retriggering = close
    if ((buttonID == GetParentUI()->GetSpecialControlIds().okBttnId) &&
        numberOfPresses > 0 && !isRetriggering) {
      okayCnt_++;
      if ((okayCnt_ == 2) && IsActive())
         Close();
      else
        lastOkayMS_ = daisy::System::GetNow();
    }

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
                    uint32_t nowMS) override {

    // Has our OK count expired?
    if (nowMS - lastOkayMS_ >= DOUBLE_CLICK_TIMEOUT)
    {
      okayCnt_ = 0;
    }

    FontDef fd = Font_6x8;

    int16_t lineY = 0;
    uint16_t textH = fd.FontHeight + 1;
    display.SetCursor((128-12*7)/2, 0);
    display.WriteString("CONTROL TEST", Font_7x10, true);
    lineY += textH + 2;

    daisy::FixedCapStr<20> text;
    // Encoders
    for (size_t i = 0; i < ENCODER_COUNT; i++) {
      text.AppendInt(i + 1);
      text.Append(":");
      text.AppendInt(encoderValues[i], true);
      text.Append("_");
      text.AppendInt(encoderSteps[i]);

      if (((i + 1) % 3 == 0) || (i == ENCODER_COUNT - 1)) {
        display.SetCursor(0, lineY);
        display.WriteString(text.Cstr(), fd, true);
        text.Clear();
        lineY += textH;
      } else {
        text.Append(" ");
      }
    }
    lineY += 0;

    // Buttons
    for (size_t i = 0; i < BUTTON_COUNT; i++) {
      text.AppendInt(i + 1);
      text.Append(":");
      text.AppendInt(buttonValues[i]);
      if (buttonRetrig[i])
        text.Append('Y');
      else
        text.Append('N');

      if (((i + 1) % 4 == 0) || (i == BUTTON_COUNT - 1)) {
        display.SetCursor(0, lineY);
        display.WriteString(text.Cstr(), fd, true);
        text.Clear();
        lineY += textH;
      } else {
        text.Append(" ");
      }
    }

    // Potentiometers
    for (size_t i = 0; i < POT_COUNT; i++) {
      text.AppendInt(i + 1);
      text.Append(":");
      text.AppendFloat(potValues[i], 4);

      if (((i + 1) % 2 == 0) || (i == POT_COUNT - 1)) {
        display.SetCursor(0, lineY);
        display.WriteString(text.Cstr(), fd, true);
        text.Clear();
        lineY += textH;
      } else {
        text.Append(" ");
      }
    }

    if (okayCnt_ > 0) {
      display.SetCursor((128 -20*6)/2, lineY);
      display.WriteString("PRESS AGAIN TO EXIT", Font_6x8, true);
    }
  }

private:
  int okayCnt_ = 0;
  uint32_t lastOkayMS_ = 0;
  std::array<int16_t, ENCODER_COUNT> encoderValues{};
  std::array<int16_t, ENCODER_COUNT> encoderSteps{};
  std::array<uint8_t, BUTTON_COUNT> buttonValues{};
  std::array<bool, BUTTON_COUNT> buttonRetrig{};
  std::array<float, POT_COUNT> potValues{};
};
