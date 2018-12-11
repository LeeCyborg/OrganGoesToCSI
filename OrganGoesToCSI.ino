#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2
#define NUM_KEYS 43
int modifier = 0;
int Size = 10;

int keys[NUM_KEYS];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
    setupKeys();
  for (int i = 0; i < NUM_KEYS+1; i++) { // uno doesn't have all the pins
    
    pinMode(keys[i], INPUT_PULLUP);
    
  }
  pinMode(13, INPUT_PULLUP);
  colorWipe(strip.Color(0, 0, 0), 50); // Red
}

void loop() {
readKeys();
}
void readKeys() {
  for (int i = 0; i < NUM_KEYS; i++) {
    if (!digitalRead(keys[i])) {
      strip.setPixelColor(i, Wheel(modifier));
      strip.show();
      modifier = modifier + Size;
      if(i%7==0){
        i++; 
        strip.setPixelColor(i, Wheel(modifier));
      strip.show();
        };
      Serial.println(i);
    } else {
      //strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
  }
}
void setupKeys() {
  // Setup first row of keys
  int firstKeyA = 0;
  int lastKeyA  = 9;
  int firstPinA = 4;

  for(int i = firstKeyA; i <= lastKeyA; i++) {
    keys[i] = firstPinA + i;
  }
  Serial.println(keys[9]);

  // Setup second row of pins (big block)
  int firstKeyB = 10;
  int lastKeyB  = 41;
  int firstPinB = 22;

  for(int j = firstKeyB; j <= lastKeyB; j++) {
    keys[j] = firstPinB + (j - firstKeyB);
  }

  // Setup error
  keys[42] = 3;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
