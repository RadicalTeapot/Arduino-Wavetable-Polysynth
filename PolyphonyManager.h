#ifndef POLYPHONYMANAGER_H
#define POLYPHONYMANAGER_H

#ifndef MAX_POLYPHONY
#define MAX_POLYPHONY (6)
#endif

#include "WavetableReader.h"
#include "Logger.h"

const static float maxNoteAmplitude = 1.0f/MAX_POLYPHONY;

class PolyphonyManager {
  public:

    static void Init(uint16_t sampleRate, size_t wavetableSize)
    {
      PolyphonyManager::sampleRate = max(sampleRate, 1); 
      PolyphonyManager::wavetableSize = wavetableSize;
    }

    static void addNote(float freq, uint32_t duration, float pan = 0.5f) {
      const size_t index = nextFreeOrOldestNoteIndex();
      setNoteFreq(notes[index], freq);
      notes[index].duration = duration + 1; // To avoid dividing by 0
      notes[index].time = duration;
      notes[index].pan = pan;
      notes[index].free = false;
    }

    static void update(float &left, float &right) {
      left = 0; right = 0;
      
      float amplitude;
      bool sampleIncremented;
      for (size_t i = 0; i < MAX_POLYPHONY; i++) {
        if (notes[i].free) continue;

        // Update envelope
        // TODO Add attack and release control to envelope
        amplitude = 0;
        if (notes[i].time > 0) {
          amplitude = ((float)notes[i].time) / notes[i].duration;
          amplitude *= amplitude;
          notes[i].time--;
        }
        else {
          notes[i].free = true;
        }
        amplitude *= maxNoteAmplitude;

        // Update samples
        sampleIncremented = floor(notes[i].sampleIndex) < floor(notes[i].sampleIndex + notes[i].sampleIndexIncrement);
        notes[i].sampleIndex += notes[i].sampleIndexIncrement;
        if (sampleIncremented) {
          notes[i].sample = WavetableReader::getInterpolatedSample(static_cast<uint8_t>(notes[i].sampleIndex));
        }
        if (notes[i].sampleIndex >= wavetableSize) notes[i].sampleIndex -= wavetableSize;

        // TODO better panning code
        left += notes[i].sample * amplitude * (1 - notes[i].pan);
        right += notes[i].sample * amplitude * notes[i].pan;
      }
    }

  private:
    struct Note {
      float freq;
      uint32_t duration;
      float pan;

      uint32_t time;
      bool free;
      
      float sample;
      float sampleIndexIncrement;
      float sampleIndex;
    };

    static size_t nextFreeOrOldestNoteIndex() {
      uint32_t oldestNoteTime = 0xFFFFFFFF;
      size_t oldestNoteIndex = 0;
      
      for (size_t i = 0; i < MAX_POLYPHONY; i++) {
        const Note note = notes[i];
        if (note.free) return i;
        if (note.time < oldestNoteTime) {
          oldestNoteTime = note.time;
          oldestNoteIndex = i;
        }
      }

      return oldestNoteIndex;
    }

    static void setNoteFreq(Note &note, float freq) {
      note.freq = freq;
      note.sampleIndexIncrement = (freq * wavetableSize) / sampleRate;
      note.sampleIndex = 0;
    }

    static Note notes[MAX_POLYPHONY];
    static uint16_t sampleRate;
    static size_t wavetableSize;
};

PolyphonyManager::Note PolyphonyManager::notes[MAX_POLYPHONY];
uint16_t PolyphonyManager::sampleRate = 1;
size_t PolyphonyManager::wavetableSize = 1;

#endif /* POLYPHONYMANAGER_H */