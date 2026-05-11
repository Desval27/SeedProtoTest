#pragma once

#include <BasicApp.h>
#include <SynthVoice.h>
#include <singleton.hpp>

template<std::size_t VOICE_COUNT,
         std::size_t MAX_DEGREES = music::DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = music::DEF_SCALE_DEGREES>
class App
  : public BasicApp<MAX_DEGREES, SCALE_DEGREES>
  , public Singleton<App<VOICE_COUNT, MAX_DEGREES, SCALE_DEGREES>>
{
  using TBasicApp = BasicApp<MAX_DEGREES, SCALE_DEGREES>;
  using TSingletonApp = Singleton<App<VOICE_COUNT, MAX_DEGREES, SCALE_DEGREES>>;
  using TSynthVoice = SynthVoice<MAX_DEGREES, SCALE_DEGREES>;

private:
  static_assert(VOICE_COUNT > 0, "App needs at least one voice.");

  App()
    : TBasicApp()
  {
  }

  friend TSingletonApp;

public:
  static constexpr std::size_t VoiceCount = VOICE_COUNT;

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param sample_rate
  void init(float sample_rate) override
  {
    TBasicApp::init(sample_rate);

    for (TSynthVoice& v : voices) {
      v.init(TBasicApp::setup, 0, sample_rate);
      v.update(0UL); // Initial state
    }

    // For giggles at the moment.
    // voices[0].set_freq(110.0f);
    // if constexpr (VOICE_COUNT > 1)
    //   voices[1].set_freq(220.0f);
    // if constexpr (VOICE_COUNT > 2)
    //   voices[2].set_freq(440.0f);
    // if constexpr (VOICE_COUNT > 3)
    //   voices[3].set_freq(880.0f);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Processes the audio stream for all voices mixed into separate left
  /// & right values.
  /// @return left & right floating point values.
  std::tuple<float, float> process(bool trigger = false) override
  {
    const float evenMix = 1.0 / VOICE_COUNT;
    float mixL = 0.0f;
    float mixR = 0.0f;
    for (TSynthVoice& v : voices) {
      auto [sigL, sigR] = v.process();
      mixL = mixL + (sigL * evenMix);
      mixR = mixR + (sigR * evenMix);
    }
    return { mixL, mixR };
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param index
  /// @return
  TSynthVoice* GetVoicePtr(std::size_t index) { return &voices[index]; }

protected:
  void internal_update(uint32_t nowMS) override
  {
    for (TSynthVoice& v : voices)
      v.update(nowMS);
  }

private:
  std::array<TSynthVoice, VOICE_COUNT> voices;
};
