#include <daisy_seed.h>
#include <daisysp.h>
#include <dev/oled_ssd130x.h>

#include <monkey.hpp>

#include <Pages/FullScreenVerticalMenu.h>
#include <Pages/SynthVoicePage.h>
#include <shape_item.hpp>
#include <ui_overlord.hpp>

#include <MainPage.h>
#include <SeedProtoTestApp.h>
#include <StaticPage.h>
#include <TestPage.h>

using namespace daisysp;
using namespace daisy;

#define BPM 60.0f
#define ENCODER_COUNT 1
#define BUTTON_COUNT 5
#define POT_COUNT 4
#define VOICE_COUNT 4

using MyApp = App<VOICE_COUNT>;
using MyOverlord = UIOverlord<SSD130xI2c128x64Driver,
                              ENCODER_COUNT,
                              BUTTON_COUNT,
                              POT_COUNT,
                              ENCODER_1, // MenuEncoder
                              BUTTON_1,  // OK Button
                              BUTTON_2,  // Cancel Button
                              true>;
using MyMainPage = MainPage<MyApp>;
using MyTestPage = TestPage<ENCODER_COUNT, BUTTON_COUNT, POT_COUNT>;

DaisySeed hw;
MyApp& theApp = MyApp::get_instance();

MyOverlord uiOverlord;
MyMainPage mainPage;
MyTestPage testPage;
StaticPage staticPage;
std::array<SynthVoicePage, MyApp::VoiceCount> voicePages;

FullScreenItemMenu mainMenu;
FullScreenVerticalMenu<MenuFontSize::FONT_NORMAL> vertMenu1;
FullScreenVerticalMenu<MenuFontSize::FONT_SMALL> vertMenu2;

const MyOverlord::EncoderConfig encoderConfig[ENCODER_COUNT] = {
  { seed::D20, seed::D16 },
};
const MyOverlord::ButtonConfig buttonConfig[BUTTON_COUNT] = {
  { seed::D19 }, // Encoder
  { seed::D17 }, { seed::D18 }, { seed::D15 }, { seed::D21 },
};
const MyOverlord::PotConfig potConfig[POT_COUNT] = {
  { seed::A7 },
  { seed::A8 },
  { seed::A9 },
  { seed::A10 },
};

Metro clock;
HiHat<> hat;

void
NOPCallback(void* context)
{
}

////////////////////////////////////////////////////////////////////////////////
// UI & Menu Structure
////////////////////////////////////////////////////////////////////////////////
AbstractMenu::ItemConfig mainMenuItems[] = {
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "CONTROLS",
    .asOpenUiPageItem{ &testPage } },
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "MENU 1",
    .asOpenUiPageItem{ &vertMenu1 } },
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "MENU 2",
    .asOpenUiPageItem{ &vertMenu2 } },

//
// This is ugly but works for a test app
//

#if VOICE_COUNT > 0
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 1",
    .asOpenUiPageItem{ &voicePages[0] } },
#if VOICE_COUNT > 1
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 2",
    .asOpenUiPageItem{ &voicePages[1] } },
#if VOICE_COUNT > 2
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 3",
    .asOpenUiPageItem{ &voicePages[2] } },
#if VOICE_COUNT > 3
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 4",
    .asOpenUiPageItem{ &voicePages[3] } },
#if VOICE_COUNT > 4
#error "Unsupported VOICE_COUNT"
#endif
#endif
#endif
#endif
#endif
  { .type = AbstractMenu::ItemType::closeMenuItem, .text = "CLOSE" },
};

AbstractMenu::ItemConfig vertMenuItems[] = {
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "STATIC",
    .asOpenUiPageItem{ &staticPage } },
//
// This is ugly but works for a test app
//
#if VOICE_COUNT > 0
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 1",
    .asOpenUiPageItem{ &voicePages[0] } },
#if VOICE_COUNT > 1
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 2",
    .asOpenUiPageItem{ &voicePages[1] } },
#if VOICE_COUNT > 2
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 3",
    .asOpenUiPageItem{ &voicePages[2] } },
#if VOICE_COUNT > 3
  { .type = AbstractMenu::ItemType::openUiPageItem,
    .text = "VOICE 4",
    .asOpenUiPageItem{ &voicePages[3] } },
#if VOICE_COUNT > 4
#error "Unsupported VOICE_COUNT"
#endif
#endif
#endif
#endif
#endif
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 3",
    .asCallbackFunctionItem{ NOPCallback, (void*)3 } },
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 4",
    .asCallbackFunctionItem{ NOPCallback, (void*)4 } },
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 5",
    .asCallbackFunctionItem{ NOPCallback, (void*)5 } },
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 6",
    .asCallbackFunctionItem{ NOPCallback, (void*)6 } },
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 7",
    .asCallbackFunctionItem{ NOPCallback, (void*)7 } },
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 8",
    .asCallbackFunctionItem{ NOPCallback, (void*)8 } },
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 9",
    .asCallbackFunctionItem{ NOPCallback, (void*)9 } },
  { .type = AbstractMenu::ItemType::callbackFunctionItem,
    .text = "ITEM 10",
    .asCallbackFunctionItem{ NOPCallback, (void*)10 } },
  { .type = AbstractMenu::ItemType::closeMenuItem, .text = "CLOSE" },
};

////////////////////////////////////////////////////////////////////////////////
// Main Audio Loop
////////////////////////////////////////////////////////////////////////////////
void
AudioCallback(AudioHandle::InterleavingInputBuffer in,
              AudioHandle::InterleavingOutputBuffer out,
              size_t size)
{
  uiOverlord.ProcessControls();

  // Prepare the audio block
  for (size_t i = 0; i < size; i += 2) {
    bool trig = clock.Process();

    // float sigL = 0.0f;
    // float sigR = 0.0f;
    // float sig = hat.Process(trig);
    auto [sigL, sigR] = theApp.process(trig);

    out[i] = sigL;
    out[i + 1] = sigR;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param sample_rate
void
InitComponents(float sample_rate)
{
  theApp.init(sample_rate);
  clock.Init(BPM / 60.0f, sample_rate);
  hat.Init(sample_rate);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param sample_rate
void
InitUi(float sample_rate)
{
  for (std::size_t i = 0; i < theApp.VoiceCount; i++)
    voicePages[i].Bind(theApp.GetVoicePtr(i)->config_);

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
}

////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param
/// @return
int
main(void)
{
  hw.Configure();
  hw.Init();

  hw.SetAudioBlockSize(4);
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  float sample_rate = hw.AudioSampleRate();
  InitComponents(sample_rate);
  InitUi(sample_rate);

  hw.StartAudio(AudioCallback);
  while (1) {
    uiOverlord.ProcessUi(); // Update all Ui elements and and event queues.
    theApp.update(System::GetNow());
  }
}
