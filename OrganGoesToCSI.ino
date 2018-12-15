#include <Adafruit_NeoPixel.h>
#ifdef _AVR_
#include <avr/power.h>
#endif


/*##########
# Definitions
##########*/

#define FIRST_KEY 0
#define NUM_KEYS 43
#define FADE_DELAY 5
#define NEOPIX_PIN 2
#define FIRST_NEOPIX 0
#define NUM_NEOPIX 60
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 255
#define SERIAL_BAUDRATE 9600


/*##########
# Global Variables
##########*/

// Bools
boolean fadeTimerArmed  = false;

// Bool Arrays
boolean keyWasDown[NUM_KEYS];

// Ints

int modifier      = 0;          // Into `wheel` on `eachKey`
int fadeTimeout   = 4000;       // how long before fadeout
int modifierSize  = 10;

// Int arrays
int keys[NUM_KEYS];

// Floats
float fadeTimer   = 0.0; 

//Objects
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIX, NEOPIX_PIN, NEO_GRB + NEO_KHZ800);


/*##########
# Arduino Functions
##########*/

void setup() {
#if defined (_AVR_ATtiny85_)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // Init serial
  Serial.begin(SERIAL_BAUDRATE);

  // Setup organ keys mapping
  setupKeys();
  
  // Init all keys
  for (int i = FIRST_KEY; i < NUM_KEYS; i++) {
    // Set pull-up for each key
    pinMode(keys[i], INPUT_PULLUP);
    
    // Init `keyWasDown` bool array
    keyWasDown[i] = false;
  }
  
  // Init Neopixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // Do a color wipe on the NeoPixles
  colorWipe(strip.Color(0, 0, 0), 50); // Red
}


void loop() {
  // Scan the keys for presses
  allKeys();
  
  /*
  eachKey();
  */
  
  // A timer for fade out light animation (fade out the lights)
  //NOTE: Check `fadeTimerArmed` first, so we skip math and calls to `millis` if not necessary 
  if (fadeTimerArmed && fadeTimer + fadeTimeout < millis()) {
    darken();
  }
}


/*##########
# Functions
##########*/

void setupKeys() {
  // Setup first row of keys
  int firstKeyA = FIRST_KEY;
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


void allKeys() {
  // Scan through all the keys
  for (int i = FIRST_KEY; i < NUM_KEYS; i++) {
    // Check if the key is low (pressed)
    if (!digitalRead(keys[i])) {
      // Set the brightness of the entire strand to max
      strip.setBrightness(MAX_BRIGHTNESS);
      strip.show();
      
      if (false == keyWasDown[i]) {
        // Write the key pressed to Serial
        Serial.write(i + ", on;");
        keyWasDown[i] = true;
      }
      
      // Set and arm the fade timer
      fadeTimer       = millis();
      fadeTimerArmed  = true;
      
      // Map `num` for `i` (Do outside the inner loop because it never changes inside it)
      uint32_t num = Wheel((byte)map(i, FIRST_KEY, NUM_KEYS, MIN_BRIGHTNESS, MAX_BRIGHTNESS));
      
      for (uint16_t j = FIRST_NEOPIX; j < NUM_NEOPIX; j++) {
        strip.setPixelColor(j, num);
        strip.show();
      }
    } 
    // The key was HIGH (not pressed), check if it was previously
    else if (true == keyWasDown[i]) {
      // Write the key pressed to Serial
      Serial.write(i + ", off;");
      
      // Reset the down state
      keyWasDown[i] = false;
    }
  }
}


void eachKey() {
  // Scan through all the keys
  for (int i = FIRST_KEY; i < NUM_KEYS; i++) {
    // Check if the key is LOW (pressed)
    if (!digitalRead(keys[i])) {
      // Set the individual pixel color
      strip.setPixelColor(i, Wheel(modifier));
      strip.show();
      
      // Update the modifier
      //TODO: where is `modifier` reset?
      modifier += modifierSize;
    }
    // The key was HIGH (not pressed)
    else {
      // Set the individual pixel color to NONE
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
    }
  }
}


void darken() {
  // Loop down from MAX_BRIGHTNESS to MIN_BRIGHTNESS
  for (int j = MAX_BRIGHTNESS; j > MIN_BRIGHTNESS; j--) {
    strip.setBrightness(j);
    strip.show();
    
    // Wait 5ms before going to the next (lower) brightness level
    delay(FADE_DELAY);
  }
  
  // Release the fade timer (disarm)
  fadeTimerArmed = false;
}


void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = FIRST_NEOPIX; i < NUM_NEOPIX; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    
    delay(wait);
  }
}


/*##########
# Helper Functions
##########*/

uint8_t getRedValueFromColor(uint32_t c) { // ged red value from color
  return c >> 16;
}


uint8_t getGreenValueFromColor(uint32_t c) {
  return c >> 8;
}


uint8_t getBlueValueFromColor(uint32_t c) {
  return c;
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
