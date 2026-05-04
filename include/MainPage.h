#pragma once


#include <daisy.h>
#include <BasicApp.h>
#include <Pages/BasePage.h>

class MainPage : public BasePage<true> {
public:
  MainPage() : BasePage(), startPage_(nullptr) {}

  void Init(daisy::UiPage &startPage) { startPage_ = &startPage; }

  bool OnButton(uint16_t buttonID, uint8_t numberOfPresses,
                bool isRetriggering) override {
    if (buttonID == BUTTON_1) {
      if (startPage_ == nullptr || numberOfPresses == 0)
        return false;
      if (auto *ui = GetParentUI())
        ui->OpenPage(*startPage_);
    }
    return true;
  }

  bool OnPotMoved(uint16_t potID, float newPosition) override
  {
    // Pot id corresponds to voice id
    BasicApp<4> &theApp = BasicApp<4>::instance();
    auto *v = theApp.GetVoicePtr(potID);
    v->config_.volume.SetFrom0to1(newPosition);
    return true;
  }


    protected : void InternalDraw(daisy::OneBitGraphicsDisplay &display,
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