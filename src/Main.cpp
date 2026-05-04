#include <daisy_seed.h>
#include <daisysp.h>
#include <dev/oled_ssd130x.h>

#include <Monkey.h>

#include <Pages/FullScreenVerticalMenu.h>
#include <Pages/VoicePage.h>
#include <ShapeItem.h>
#include <UIOverlord.h>
#include <BasicApp.h>

#include <MainPage.h>
#include <StaticPage.h>
#include <TestPage.h>

using namespace daisysp;
using namespace daisy;

#define BPM 60.0f
#define ENCODER_COUNT 1
#define BUTTON_COUNT 5
#define POT_COUNT 4

constexpr int VOICE_COUNT = 4;

using TheApp      = BasicApp<VOICE_COUNT>;
using TheOverlord = UIOverlord<SSD130xI2c128x64Driver,
                               ENCODER_COUNT,
                               BUTTON_COUNT,
                               POT_COUNT,
                               ENCODER_1, // MenuEncoder
                               BUTTON_1,  // OK Button
                               BUTTON_2,  // Cancel Button
                               true>;
using TheTestPage = TestPage<ENCODER_COUNT, BUTTON_COUNT, POT_COUNT>;

DaisySeed          hw;
TheOverlord        uiOverlord;
MainPage           mainPage;
FullScreenItemMenu mainMenu;
TheTestPage        testPage;
StaticPage         staticPage;

std::array<VoicePage, VOICE_COUNT> voicePages;

TheApp theApp;

// Custom Items
ShapeItem shapeItem;

FullScreenVerticalMenu<MenuFontSize::FONT_NORMAL> vertMenu1;
FullScreenVerticalMenu<MenuFontSize::FONT_SMALL>  vertMenu2;

const TheOverlord::EncoderConfig encoderConfig[ENCODER_COUNT] = {
    {seed::D20, seed::D16},
};
const TheOverlord::ButtonConfig buttonConfig[BUTTON_COUNT] = {
    {seed::D19}, // Encoder
    {seed::D17},
    {seed::D18},
    {seed::D15},
    {seed::D21},
};
const TheOverlord::PotConfig potConfig[POT_COUNT] = {
    {seed::A7},
    {seed::A8},
    {seed::A9},
    {seed::A10},
};

Metro   clock;
HiHat<> hat;

void NOPCallback(void *context) {}

////////////////////////////////////////////////////////////////////////////////
// UI & Menu Structure
////////////////////////////////////////////////////////////////////////////////
AbstractMenu::ItemConfig mainMenuItems[] = {
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "CONTROLS",
     .asOpenUiPageItem{&testPage}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "MENU 1",
     .asOpenUiPageItem{&vertMenu1}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "MENU 2",
     .asOpenUiPageItem{&vertMenu2}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 1",
     .asOpenUiPageItem{&voicePages[0]}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 2",
     .asOpenUiPageItem{&voicePages[1]}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 3",
     .asOpenUiPageItem{&voicePages[2]}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 4",
     .asOpenUiPageItem{&voicePages[3]}},
    {.type = AbstractMenu::ItemType::closeMenuItem, .text = "CLOSE"},
};

AbstractMenu::ItemConfig vertMenuItems[] = {
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "STATIC",
     .asOpenUiPageItem{&staticPage}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 1",
     .asOpenUiPageItem{&voicePages[0]}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 2",
     .asOpenUiPageItem{&voicePages[1]}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 3",
     .asOpenUiPageItem{&voicePages[2]}},
    {.type = AbstractMenu::ItemType::openUiPageItem,
     .text = "VOICE 4",
     .asOpenUiPageItem{&voicePages[3]}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 3",
     .asCallbackFunctionItem{NOPCallback, (void *)3}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 4",
     .asCallbackFunctionItem{NOPCallback, (void *)4}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 5",
     .asCallbackFunctionItem{NOPCallback, (void *)5}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 6",
     .asCallbackFunctionItem{NOPCallback, (void *)6}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 7",
     .asCallbackFunctionItem{NOPCallback, (void *)7}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 8",
     .asCallbackFunctionItem{NOPCallback, (void *)8}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 9",
     .asCallbackFunctionItem{NOPCallback, (void *)9}},
    {.type = AbstractMenu::ItemType::callbackFunctionItem,
     .text = "ITEM 10",
     .asCallbackFunctionItem{NOPCallback, (void *)10}},
    {.type = AbstractMenu::ItemType::closeMenuItem, .text = "CLOSE"},
};

////////////////////////////////////////////////////////////////////////////////
// Main Audio Loop
////////////////////////////////////////////////////////////////////////////////
void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    uiOverlord.ProcessControls();

    // Prepare the audio block
    for(size_t i = 0; i < size; i += 2)
    {
        bool  trig = clock.Process();
        float sig  = hat.Process(trig);

        out[i]     = sig;
        out[i + 1] = sig;
    }
}

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    hw.Configure();
    hw.Init();

    // hw.StartLog(false);

    hw.SetAudioBlockSize(4);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    float sample_rate = hw.AudioSampleRate();

    clock.Init(BPM / 60.0f, sample_rate);
    hat.Init(sample_rate);

    for (int i = 0; i < VOICE_COUNT; i++)
        voicePages[i].Init(&theApp.voices[i]);

    mainPage.Init(mainMenu);
    mainMenu.Init(mainMenuItems,
                  ArrayLen(mainMenuItems),
                  AbstractMenu::Orientation::leftRightSelectUpDownModify,
                  true);
    vertMenu1.Init(vertMenuItems,
                   ArrayLen(vertMenuItems),
                   AbstractMenu::Orientation::leftRightSelectUpDownModify,
                   true);
    vertMenu2.Init(vertMenuItems,
                   ArrayLen(vertMenuItems),
                   AbstractMenu::Orientation::leftRightSelectUpDownModify,
                   true);


    uiOverlord.Init(
        sample_rate, mainPage, &hw.adc, encoderConfig, buttonConfig, potConfig);

    hw.StartAudio(AudioCallback);
    while(1)
    {
        uiOverlord.ProcessUi();
    }
}
