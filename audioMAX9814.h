#define AUDIODELAY 1
#define SPECTRUMSMOOTH 0.08
#define PEAKDECAY 0.01
#define NOISEFLOOR 65

// AGC settings
#define AGCSMOOTH 0.004
#define GAINUPPERLIMIT 15.0
#define GAINLOWERLIMIT 0.1

// Global variables
unsigned int spectrumValue[7];  // holds raw adc values
float spectrumDecay[7] = {0};   // holds time-averaged values
float spectrumPeaks[7] = {0};   // holds peak values
float audioAvg = 270.0;
float gainAGC = 0.0;

int maximum = 600;
uint16_t sampleWindow = 10;
unsigned int sample;

void doAnalogs() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 600;

  // store sum of values for AGC
  int analogsum = 0;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0);

    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }

  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  if (peakToPeak < 0) peakToPeak = 0;
  else if (peakToPeak > 1023) peakToPeak = 0;
  
  // prepare average for AGC
  analogsum += peakToPeak;

  // apply current gain value
  peakToPeak *= gainAGC;

  for (int i = 0; i < 7; i++) {
    spectrumValue[i] = peakToPeak;
    // process time-averaged values
    spectrumDecay[i] = (1.0 - SPECTRUMSMOOTH) * spectrumDecay[i] + SPECTRUMSMOOTH * spectrumValue[i];
  }
  // Calculate audio levels for automatic gain
  audioAvg = (1.0 - AGCSMOOTH) * audioAvg + AGCSMOOTH * (analogsum);

  // Calculate gain adjustment factor
  gainAGC = 270.0 / audioAvg;
  if (gainAGC > GAINUPPERLIMIT) gainAGC = GAINUPPERLIMIT;
  if (gainAGC < GAINLOWERLIMIT) gainAGC = GAINLOWERLIMIT;

}
