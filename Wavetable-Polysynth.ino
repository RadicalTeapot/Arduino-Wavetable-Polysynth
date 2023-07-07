#include "PolyphonyManager.h"
#include "WavetableReader.h"
#include "Wavetable.h"

#include "pwm.h"
#include "FspTimer.h"

const static uint8_t mod1Pin = A1;
const static uint8_t mod2Pin = A2;
const static uint8_t leftOutPin = D2;
const static uint8_t rightOutPin = D4;

const static uint16_t sampleRate = 22050;

const static size_t noteCount = 10;
const static float notes[noteCount] = { // Two octaves of minor pentatonic starting on C3
  130.8127826503f, 155.56349186104f, 174.6141157165f, 195.99771799087f, 233.08188075904f, 
  261.6255653006f, 311.12698372208f, 349.228231433f, 391.99543598175f, 466.16376151809f };

PwmOut leftOutPwm(leftOutPin);
PwmOut rightOutPwm(rightOutPin);
float left, right;
void timerCallback(timer_callback_args_t __attribute((unused)) *p_args) {
  PolyphonyManager::update(left, right);
  leftOutPwm.pulse_perc(left);
  rightOutPwm.pulse_perc(right);
};

FspTimer timer;
bool setupTimer() {
  uint8_t type = GPT_TIMER;
  int8_t index = FspTimer::get_available_timer(type);
  if (index == 0) {
    FspTimer::force_use_of_pwm_reserved_timer();
    index = FspTimer::get_available_timer(type);
  }

  if (index == 0) return false;
  if (!timer.begin(TIMER_MODE_PERIODIC, type, index, (float)sampleRate, 0.0f, timerCallback)) return false;
  if (!timer.setup_overflow_irq()) return false;
  if (!timer.open()) return false;
  if (!timer.start()) return false;
}

const static float onePoleCoeff = 0.5;
inline float onePole(float &out, const float &in) {
  out += onePoleCoeff * (in - out);
}

void setup() {
  Serial.begin();
  analogReadResolution(8);

  PolyphonyManager::Init(sampleRate, tableSize);
  leftOutPwm.begin(255, 0, true);
  rightOutPwm.begin(255, 0, true);
  setupTimer();
}

const static uint8_t readPeriod = 100;
uint32_t last = 0;
float mod1 = 0, mod2 = 0;
uint32_t noteLast = 0;
uint16_t noteNext = 0;
void loop() {
  const uint32_t now = millis();

  if (now >= last + readPeriod) {
    onePole(mod1, analogRead(mod1Pin));
    onePole(mod2, analogRead(mod2Pin));
    WavetableReader::setModRawValues(mod1, mod2);
    last = now;
  }

  if (now >= noteLast + noteNext) {
    noteNext = random(1, 9) * 250;
    PolyphonyManager::addNote(
      notes[random(noteCount)], // freq
      static_cast<uint32_t>(noteNext * (sampleRate / 1000.0f) * random(2, 5)), // duration
      random(4, 13) / 16.0f // pan
    );
    noteLast = now;
  }
}
