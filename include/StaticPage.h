#pragma once

#include <daisy.h>

#include <Pages/BasePage.h>

class StaticPage : public BasePage<true>
{
public:
  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  void OnShow() override
  {
    BasePage::OnShow();

    // const int16_t d[] = {-1, +1};
    // dx1 = +1;
    // dy1 = +1;
    // dx2 = -1;
    // dy2 = -1;
    // dx1_ = d[random_range(0, 1)];
    // dy1_ = d[random_range(0, 1)];
    // dx2_ = d[random_range(0, 1)];
    // dy2_ = d[random_range(0, 1)];
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param buttonID
  /// @param numberOfPresses
  /// @param isRetriggering
  /// @return
  bool OnButton(uint16_t buttonID,
                uint8_t numberOfPresses,
                bool isRetriggering) override
  {
    if (IsActive() && numberOfPresses > 0)
      Close();
    return true;
  }

protected:
  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param display
  /// @param nowMS
  void InternalDraw(daisy::OneBitGraphicsDisplay& display,
                    uint32_t nowMS) override
  {
    FontDef sfd = Font_11x18;
    FontDef pfd = Font_16x26;

    display.SetCursor(x1_, y1_);
    display.WriteString("STATIC", sfd, true);

    display.SetCursor(x2_, y2_);
    display.WriteString("PAGE", pfd, true);

    if ((nowMS % flashPeriodMS_) < flashOnMS_) {
      display.SetCursor((128 - 16 * 7) / 2, display.Height() - 11);
      display.WriteString("PRESS ANY BUTTON", Font_7x10, true);
    }

    uint32_t delta = nowMS - lastDriftMS_;
    if (delta >= driftMS_) {
      lastDriftMS_ = nowMS - (delta % driftMS_);

      // x1_ += dx1_;
      // y1_ += dy1_;
      x2_ += dx2_;
      y2_ += dy2_;

      if ((x1_ <= 0) || (x1_ >= display.Width() - (sfd.FontWidth * 6)))
        dx1_ = -dx1_;
      if ((y1_ <= 0) || (y1_ >= display.Height() - sfd.FontHeight))
        dy1_ = -dy1_;

      if ((x2_ <= 0) || (x2_ >= display.Width() - (pfd.FontWidth * 4)))
        dx2_ = -dx2_;
      if ((y2_ <= 0) || (y2_ >= display.Height() - pfd.FontHeight))
        dy2_ = -dy2_;
    }
  }

private:
  const uint32_t flashPeriodMS_ = 1000;
  const uint32_t flashOnMS_ = flashPeriodMS_ / 2;

  const uint32_t driftRate_ = 8;
  const uint32_t driftMS_ = 1000 / driftRate_;
  uint32_t lastDriftMS_ = 0;

  int8_t dx1_ = +1;
  int8_t dx2_ = +1;
  int8_t dy1_ = -1;
  int8_t dy2_ = -1;
  int16_t x1_ = 30;
  int16_t y1_ = 0;
  int16_t x2_ = 30;
  int16_t y2_ = 18;
};
