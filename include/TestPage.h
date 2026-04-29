#pragma once

#include <array>
#include <BasePage.h>

template <std::size_t POT_COUNT = 4>
class TestPage : public BasePage
{
public:
    TestPage() : BasePage() {}

    bool OnPotMoved(uint16_t potID, float newPosition) override
    {
        if(potID < POT_COUNT)
        {
            potValues[potID] = newPosition;
            return true;
        }
        return false;
    }

protected:
    void InternalDraw(daisy::OneBitGraphicsDisplay &display, const daisy::Rectangle bounds) override
    {
        Rectangle textRect = bounds.WithHeight(12);
        display.DrawRect(textRect, true, false);
        display.WriteStringAligned("Test Page", Font_7x10, textRect, Alignment::centered, true);

        FixedCapStr<32> text;
        uint16_t lineY = 14;
        for(size_t i = 0; i < POT_COUNT; i++)
        {
            text.AppendInt(i + 1); 
            text.Append(":");
            text.AppendFloat(potValues[i], 4);
            if((i % 2) == 1 || i == POT_COUNT - 1)
            {
                display.SetCursor(0, lineY);
                display.WriteString(text.Cstr(), Font_7x10, true);
                text.Clear();
                lineY += 10;
            }
            else
            {
                text.Append("  ");
            }
        }
    }

private:
    std::array<float, POT_COUNT> potValues{};
};
