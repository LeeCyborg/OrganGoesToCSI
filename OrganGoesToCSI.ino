#include <Adafruit_NeoPixel.h>
#ifdef _AVR_
#include <avr/power.h>
#endif

#define PIN 2
#define NUM_KEYS 43
#define maxBrightness 255 // zero to two fifty five
int modifier = 0;
int Size = 10;
float timer;    
int timeout = 4000; // how long before fadeout
boolean flop = false;

int keys[NUM_KEYS];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
#if defined (_AVR_ATtiny85_)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  setupKeys();
  for (int i = 0; i < NUM_KEYS + 1; i++) { // uno doesn't have all the pins

    pinMode(keys[i], INPUT_PULLUP);

  }
  pinMode(13, INPUT_PULLUP);
  colorWipe(strip.Color(0, 0, 0), 50); // Red
}

void loop() {
  allKeys();
  //eachKey();
  if (timer + timeout < millis()) {
    if (flop) {
      darken(strip.getPixelColor(10));
    }
  }
}
void allKeys() {
  for (int i = 0; i < NUM_KEYS; i++) {
    if (!digitalRead(keys[i])) {
      strip.setBrightness(maxBrightness);
      strip.show();
      Serial.write(i);
      timer = millis();
      flop = true;
      for (int j = 0; j < 60; j++) {
        int num = map(i, 0, 43, 0, 255);
        strip.setPixelColor(j, Wheel(num));
        strip.show();
      }
    }
  }
}

void eachKey() {
  for (int i = 0; i < NUM_KEYS; i++) {
    if (!digitalRead(keys[i])) {
      strip.setPixelColor(i, Wheel(modifier));
      strip.show();
      modifier = modifier + Size;
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
    }
  }
}

void darken(uint32_t color) {
  for (int j = maxBrightness; j > 0; j--) {
    strip.setBrightness(j);
    strip.show();
    delay(5);
  }
  flop = false;
}

uint8_t getRedValueFromColor(uint32_t c) { // ged red value from color
  return c >> 16;
}

uint8_t getGreenValueFromColor(uint32_t c) {
  return c >> 8;
}

uint8_t getBlueValueFromColor(uint32_t c) {
  return c;
}

void setupKeys() {
  // Setup first row of keys
  int firstKeyA = 0;
  int lastKeyA  = 9;
  int firstPinA = 4;

  for (int i = firstKeyA; i <= lastKeyA; i++) {
    keys[i] = firstPinA + i;
  }

  // Setup second row of pins (big block)
  int firstKeyB = 10;
  int lastKeyB  = 41;
  int firstPinB = 22;

  for (int j = firstKeyB; j <= lastKeyB; j++) {
    keys[j] = firstPinB + (j - firstKeyB);
  }

  // Setup error
  keys[42] = 3;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
