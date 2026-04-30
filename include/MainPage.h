#pragma once

#include <BasePage.h>
#include <daisy.h>

class MainPage : public BasePage<false> {
public:
  MainPage() : BasePage(), startPage_(nullptr) {}

  void Init(daisy::UiPage &startPage) { startPage_ = &startPage; }

  bool OnOkayButton(uint8_t numberOfPresses, bool isRetriggering) override {
    if (startPage_ == nullptr || numberOfPresses == 0)
      return false;
    if (auto *ui = GetParentUI())
      ui->OpenPage(*startPage_);
    return true;
  }

protected:
  void InternalDraw(daisy::OneBitGraphicsDisplay &display,
                    uint32_t nowMS) override {
    uint16_t h = display.GetBounds().GetHeight() / 2;
    daisy::Rectangle rect = display.GetBounds().WithHeight(h);
    display.SetCursor(26, 0);
    display.WriteString("DAYSEYE", Font_11x18, true);
    display.SetCursor(30, 20);
    display.WriteString("TESTER", Font_11x18, true);

    if ((nowMS % flashPeriodMS_) < flashOnMS_) {
      display.SetCursor(12, 40);
      display.WriteString("PRESS OK BUTTON", Font_7x10, true);
    }

    daisy::FixedCapStr<20> text = "TIME:";
    text.AppendInt(nowMS);
    display.SetCursor(0, 53);
    display.WriteString(text.Cstr(), Font_6x8, true);
  }

private:
  daisy::UiPage *startPage_;
  const uint32_t flashPeriodMS_ = 1000;
  const uint32_t flashOnMS_ = flashPeriodMS_ / 2;
};