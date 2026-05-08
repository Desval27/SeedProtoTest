#pragma once

#include <BasicApp.h>
#include <Singleton.h>
#include <SynthVoice.h>

template<std::size_t VOICE_COUNT,
         std::size_t MAX_DEGREES = Music::DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = Music::DEF_SCALE_DEGREES>
class App
  : public BasicApp<MAX_DEGREES, SCALE_DEGREES>
  , public Singleton<App<VOICE_COUNT, MAX_DEGREES, SCALE_DEGREES>>
{
  using BaseApp = BasicApp<MAX_DEGREES, SCALE_DEGREES>;
  using SingletonApp = Singleton<App<VOICE_COUNT, MAX_DEGREES, SCALE_DEGREES>>;

private:
  static_assert(VOICE_COUNT > 0, "App needs at least one voice.");

  App()
    : BaseApp()
  {
  }

  friend SingletonApp;

public:
  static constexpr std::size_t VoiceCount = VOICE_COUNT;

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param sample_rate
  void Init(float sample_rate) override
  {
    BaseApp::Init(sample_rate);

    for (SynthVoice& v : voices) {
      v.Init(sample_rate);
      v.Update(0UL); // Initial state
    }

    // For giggles at the moment.
    voices[0].SetFreq(110.0f);
    if constexpr (VOICE_COUNT > 1)
      voices[1].SetFreq(220.0f);
    if constexpr (VOICE_COUNT > 2)
      voices[2].SetFreq(440.0f);
    if constexpr (VOICE_COUNT > 3)
      voices[3].SetFreq(880.0f);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Processes the audio stream for all voices mixed into separate left
  /// & right values.
  /// @return left & right floating point values.
  std::tuple<float, float> Process(bool trigger = false) override
  {
    const float evenMix = 1.0 / VOICE_COUNT;
    float mixL = 0.0f;
    float mixR = 0.0f;
    for (SynthVoice& v : voices) {
      auto [sigL, sigR] = v.Process();
      mixL = mixL + (sigL * evenMix);
      mixR = mixR + (sigR * evenMix);
    }
    return { mixL, mixR };
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param index
  /// @return
  SynthVoice* GetVoicePtr(std::size_t index) { return &voices[index]; }

protected:
  void InternalUpdate(uint32_t nowMS) override
  {
    for (SynthVoice& v : voices)
      v.Update(nowMS);
  }

private:
  std::array<SynthVoice, VOICE_COUNT> voices;
  std::array<Music::EventSetManager<>, VOICE_COUNT> voiceEvents;
};
