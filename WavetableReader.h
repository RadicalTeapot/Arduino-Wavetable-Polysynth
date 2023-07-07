#ifndef WAVETABLEREADER_H
#define WAVETABLEREADER_H

#include "Wavetable.h"

class WavetableReader {
  public:
    static void setModRawValues(const float &mod1, const float &mod2) {
        mod1Idx = static_cast<uint8_t>(floor(mod1 / 32));
        mod1T = mod1 / 32 - mod1Idx;

        mod2Idx = static_cast<uint8_t>(floor(mod2 / 32));
        mod2T = mod2 / 32 - mod2Idx;
    }

    static float getInterpolatedSample(const uint8_t &sampleIndex) {
      float sample = (float)wavetable[mod2Idx << 11 | mod1Idx << 8 | sampleIndex];
      
      const uint8_t mod1InterpIdx = (mod1Idx + 1) & 0x7;
      const float mod1InterpSample = (float)wavetable[mod2Idx << 11 | mod1InterpIdx << 8 | sampleIndex];
      sample = sample * (1 - mod1T) + mod1InterpSample * mod1T;

      const uint8_t mod2InterpIdx = (mod2Idx + 1) & 0x7;
      const float mod2InterpSample = (float)wavetable[mod2InterpIdx << 11 | mod1Idx << 8 | sampleIndex];
      sample = sample * (1 - mod2T) + mod2InterpSample * mod2T;

      return sample;
    }

  private:
    static uint8_t mod1Idx;
    static float mod1T;

    static uint8_t mod2Idx;
    static float mod2T;
};

uint8_t WavetableReader::mod1Idx = 0;
float WavetableReader::mod1T = 0.0f;

uint8_t WavetableReader::mod2Idx = 0;
float WavetableReader::mod2T = 0.0f;

#endif /* WAVETABLEREADER_H */