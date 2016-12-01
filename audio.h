// Interface with MSGEQ7 chip for audio analysis

#define AUDIODELAY 10

// Pin definitions
#define ANALOGPIN A0
#define STROBEPIN 5
#define RESETPIN  4

// Smooth/average settings
#define SPECTRUMSMOOTH 0.08
#define PEAKDECAY 0.01
#define NOISEFLOOR 65

// AGC settings
#define AGCSMOOTH 0.004
#define GAINUPPERLIMIT 15.0
#define GAINLOWERLIMIT 0.1

#define NOISEFLOOR 200 // NOISEFLOOR allows to cut noise in the low power values

// Global variables
unsigned int spectrumValue[7];  // holds raw adc values
float prev_value[7] = {0};      // holds previous values, useful if we want to apply a low pass filter.
                                // spectrumValue[i]  = prev_value[i] + (input - prev_value[i]) * lowPass_audio;
float spectrumDecay[7] = {0};   // holds time-averaged values
float spectrumPeaks[7] = {0};   // holds peak values
float audioAvg = 270.0;
float gainAGC = 0.0;

void doAnalogs() {

  static PROGMEM const byte spectrumFactors[7] = {9, 11, 13, 13, 12, 12, 13};
  //  static PROGMEM const byte spectrumFactors[7] = {100, 11, 13, 13, 14, 14, 1};

  // reset MSGEQ7 to first frequency bin
  digitalWrite(RESETPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(RESETPIN, LOW);

  // store sum of values for AGC
  int analogsum = 0;

  // cycle through each MSGEQ7 bin and read the analog values
  for (int i = 0; i < 7; i++) {

    // set up the MSGEQ7
    digitalWrite(STROBEPIN, LOW);
    delayMicroseconds(30); // to allow the output to settle

    // read the analog value
    prev_value[i] = spectrumValue[i];
    spectrumValue[i] = analogRead(ANALOGPIN);
    digitalWrite(STROBEPIN, HIGH);

    // There are two ways of using the NOISEFLOOR value: cut the lower values and then stretch the interval
//    spectrumValue[i] = constrain(spectrumValue[i], NOISEFLOOR, spectrumValue[i]);
//    spectrumValue[i] = map(spectrumValue[i], NOISEFLOOR, 1023, 0, 800);

    // Or move the values by the NOISEFLOOR value.
    // The selection of the algo determines the influence in the AGC.
        if (spectrumValue[i] < NOISEFLOOR) {
          spectrumValue[i] = 0;
        } else {
          spectrumValue[i] -= NOISEFLOOR;
        }

    // apply correction factor per frequency bin
    spectrumValue[i] = (spectrumValue[i] * pgm_read_byte_near(spectrumFactors + i)) / 10;

    // prepare average for AGC
    analogsum += spectrumValue[i];

    // apply current gain value
    spectrumValue[i] *= gainAGC;

    // process time-averaged values
    spectrumDecay[i] = (1.0 - SPECTRUMSMOOTH) * spectrumDecay[i] + SPECTRUMSMOOTH * spectrumValue[i];

    // process peak values
    if (spectrumPeaks[i] < spectrumDecay[i]) spectrumPeaks[i] = spectrumDecay[i];
    spectrumPeaks[i] = spectrumPeaks[i] * (1.0 - PEAKDECAY);
  }

  // Calculate audio levels for automatic gain
  audioAvg = (1.0 - AGCSMOOTH) * audioAvg + AGCSMOOTH * (analogsum / 7.0);

  // Calculate gain adjustment factor
  gainAGC = 270.0 / audioAvg;
  if (gainAGC > GAINUPPERLIMIT) gainAGC = GAINUPPERLIMIT;
  if (gainAGC < GAINLOWERLIMIT) gainAGC = GAINLOWERLIMIT;





}

// Attempt at beat detection
byte beatTriggered = 0;
#define beatLevel 20.0
#define beatDeadzone 30.0
#define beatDelay 50
float lastBeatVal = 0;
byte beatDetect() {
  static float beatAvg = 0;
  static unsigned long lastBeatMillis;
  float specCombo = (spectrumDecay[0] + spectrumDecay[1]) / 2.0;
  beatAvg = (1.0 - AGCSMOOTH) * beatAvg + AGCSMOOTH * specCombo;

  if (lastBeatVal < beatAvg) lastBeatVal = beatAvg;
  if ((specCombo - beatAvg) > beatLevel && beatTriggered == 0 && currentMillis - lastBeatMillis > beatDelay) {
    beatTriggered = 1;
    lastBeatVal = specCombo;
    lastBeatMillis = currentMillis;
    return 1;
  } else if ((lastBeatVal - specCombo) > beatDeadzone) {
    beatTriggered = 0;
    return 0;
  } else {
    return 0;
  }

}

