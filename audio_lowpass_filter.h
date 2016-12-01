// the following variables obviously shouldn't be declared here,
// just putting them here for convinience

float half_factor = 0.00f;
int   mono_volume;

int left[7];
int right[7];
int mono[7];
int half_mapped[7];

int left_value;
int right_value;
int mono_value;

int filter_min = 100; // eliminate msgeq7 noise
int filter_max = 1023;
int value_floor = 0;
int value_ceiling = 255;

// higher value for lowPass_audio means the animation is more responsive to audio,
// BUT the brightness is less smooth, naturally. keep it lower to get that silky data goodness
float lowPass_audio = 0.18f; // "f" is to just cut the floating integer off at 2 decimals
// .18 is a good starting point for smoothing, I think. 0.14 starts to get less responsive

void ReadAudio() {

  half_factor = 0.00f;
  mono_volume  = 0;

  digitalWrite(reset, HIGH); // reset dat thang
  digitalWrite(reset, LOW);
  delayMicroseconds(1);

  for (int band = 0; band < 7; band++)
  {
    digitalWrite(strobe, LOW);
    delayMicroseconds(35);

    prev_value  = left[band]; // store what is now the PREVIOUS value for this band as prev_value
    left_value  = analogRead(left_in); // store the CURRENT value for this band as a new variable
    
    //anddddd map/constrain that!
    left_value  = constrain(left_value, filter_min, filter_max);
    left_value  = map(left_value, filter_min, filter_max, value_floor, value_ceiling);

    //then smooth that thang out
    left[band]  = prev_value + (left_value - prev_value) * lowPass_audio;


    //and if you have enough processing power/two msgeq7 chips like the spectrum shield, read the other channel
    //I've found that averaging both bands to mono inherently makes an animation smoother 
//    prev_value  = right[band];
//    right_value = analogRead(right_in);
//    right_value = constrain(right_value, filter_min, filter_max);
//    right_value = map(right_value, filter_min, filter_max, value_floor, value_ceiling);
//    right[band] = prev_value + (right_value - prev_value) * lowPass_audio;

    // average to mono,
//    mono[band]  = (left[band] + right[band]) * 0.5;
    // and add each  new mono band value to mono_volume, which is used to calculate the
    // factor to multiply each band by below. this is how the stretching/contracting stuff works
    // be sure to reset half_factor and mono_volume with each iteration
    mono_volume += left[band];

    digitalWrite(strobe, HIGH);
    delayMicroseconds(1);

  }

  // now that we have the sum of each band, average dat
  mono_volume  /= 7;
  //NOW, half_factor is v important. i have 240 leds in total, so here i'm dividing
  // half of that by mono_volume so that I can mirror the effect on both sides of the lamp
  // say the average volume is 100 in this current cycle, and I have 120 leds on both sides.
  // 120/100 is 1.2
  // in the following loop, I basically multiply each megeq7 band value by 1.2 so that
  // all leds are filled with the msgeq7 data, but the bands that are not as loud don't stretch as far.
  // hope that makes sense
  half_factor = HALF_NUM_LEDS / mono_volume;

  // here is where the length of each band is calculated by multiplying the band
  // by that "1.2" variable above (which onbiously changes with each iteration)
  // if you were to print these values to the serial monitor, they would all
  // add up to HALF_NUM_LEDS.
  for (int band = 0; band < 7; band++)
  {
    half_MAPPED_AMPLITUDE = mono[band] * half_factor;
    half_mapped[band] = half_MAPPED_AMPLITUDE;
  }

// anddd print the audio if you want
//#ifdef PRINT_AUDIO
//  printAudio();
//#endif

}

//declare this somewhere else, just here as a reminder
//float divFactor;


void flex_radiate() {

  readMSGEQ7_mono(); // or ReadAudio(), whatever you want to call it

  ledindex = HALF_POS;
  k = 0;

  for (int band = 0; band < 7; band++) {

    k = (band + 1) % 7; // doesn't work perfectly, but serves as a value for the NEXT band number
    half_MAPPED_AMPLITUDE = half_mapped[band];
    // there is a lot of math here that helps smooth brightness values
    // at the EDGES of each band on the led strip, so it doesn't immediately go from one color to another
    // this helps a TON for having it wrapped around a tube
    divFactor = 1.0f / half_MAPPED_AMPLITUDE;

    // segment is a variable (declare it somewhere) for FINAL led number of each band, with ledindex
    // indicating the START of each band. it is incremented after each cycle
    segment = half_MAPPED_AMPLITUDE + ledindex;
    hue = band * 35; // or whatever you want. just determines how much of the hue spectrum fits on the strip
    //hue += mono[0] * 0.01;

    for (index = ledindex; index < segment; index++) {
      brightness = (mono[band] * (half_MAPPED_AMPLITUDE - cnt) * divFactor) + (mono[k] * cnt * divFactor);
      //brightness = map(brightness, 0, 255, 100, 255);
      leds[index] = CHSV(hue, saturation, brightness);
      // to mirror to the bottom half of the strip
      leds[HALF_POS - (index - HALF_POS)] = CHSV(hue, saturation, brightness);
      cnt++;
    }
    cnt = 0;
    ledindex += half_MAPPED_AMPLITUDE;
  }
  // ehhh, blur it. why not.
  blur2d(leds, kMatrixWidth, kMatrixHeight, 100);
  FastLED.show();
}
