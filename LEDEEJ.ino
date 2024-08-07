#include <FastLED.h>


#define LED_PIN     5
#define NUM_LEDS    10
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds0[NUM_LEDS];
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];

const int numMappings = 10;
const int valueMapping[numMappings] = { 0, 30, 150, 312, 485, 670, 840, 980, 1005,  1016};
const int ledMapping[numMappings] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int previousLitLeds = -1;
float previousFraction = -1;

const int margin = 10;  // Margin around the target value to prevent flicker

const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4};

int analogSliderValues[NUM_SLIDERS];

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }

  FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(leds0, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(leds1, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(leds3, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(leds4, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  Serial.begin(9600);
}

void loop() {
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)

  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);
    updateLedColor(i, (int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}

void updateLedColor(int sliderIdx, int sliderValue) {
  sliderValue = map(sliderValue, 0, 1023, 1023, 0);
  int litLeds = 0;
  float fraction = 0;

  // Find the appropriate number of lit LEDs based on the slider value
  for (int i = 0; i < numMappings - 1; i++) {
    if (sliderValue >= valueMapping[i] && sliderValue < valueMapping[i + 1]) {
      fraction = (float)(sliderValue - valueMapping[i]) / (valueMapping[i + 1] - valueMapping[i]);
      litLeds = ledMapping[i] + fraction * (ledMapping[i + 1] - ledMapping[i]);
      break;
    }
  }

  if (sliderValue >= valueMapping[numMappings - 1]) {
    litLeds = ledMapping[numMappings - 1];
    fraction = 1.0;
  }

  // Implement flicker prevention margin
  static int lastSliderValue = -1;
  if (abs(sliderValue - lastSliderValue) > margin || previousLitLeds != litLeds || previousFraction != fraction) {
    // Define the start and end colors for the gradient
    CRGB startColor = CRGB::Red;
    CRGB endColor = CRGB::Green;

    // Update the color of each LED based on the number of lit LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i < litLeds - 1) {
        // Fully lit LEDs
        if (sliderIdx == 0) {
          leds0[i] = blend(startColor, endColor, (255 * i) / (NUM_LEDS - 1));
        } else if (sliderIdx == 1) {
          leds1[i] = blend(startColor, endColor, (255 * i) / (NUM_LEDS - 1));
        } else if (sliderIdx == 2) {
          leds2[i] = blend(startColor, endColor, (255 * i) / (NUM_LEDS - 1));
        } else if (sliderIdx == 3) {
          leds3[i] = blend(startColor, endColor, (255 * i) / (NUM_LEDS - 1));
        } else if (sliderIdx == 4) {
          leds4[i] = blend(startColor, endColor, (255 * i) / (NUM_LEDS - 1));
        }
      } else if (i == litLeds - 1 && fraction > 0) {
        // Partially lit LED
        CRGB color = blend(startColor, endColor, (255 * i) / (NUM_LEDS - 1));
        if (sliderIdx == 0) {
          leds0[i] = color;
          // Adjust brightness manually
          leds0[i].nscale8((uint8_t)(fraction * 255));  // Adjust brightness
        } else if (sliderIdx == 1) {
          leds1[i] = color;
          // Adjust brightness manually
          leds1[i].nscale8((uint8_t)(fraction * 255));  // Adjust brightness
        } else if (sliderIdx == 2) {
          leds2[i] = color;
          // Adjust brightness manually
          leds2[i].nscale8((uint8_t)(fraction * 255));  // Adjust brightness
        } else if (sliderIdx == 3) {
          leds3[i] = color;
          // Adjust brightness manually
          leds3[i].nscale8((uint8_t)(fraction * 255));  // Adjust brightness
        } else if (sliderIdx == 4) {
          leds4[i] = color;
          // Adjust brightness manually
          leds4[i].nscale8((uint8_t)(fraction * 255));  // Adjust brightness
        }
      } else {
        if (sliderIdx == 0) {
          // Unlit LEDs
          leds0[i] = CRGB::Black;
        } else if (sliderIdx == 1) {
          leds1[i] = CRGB::Black;
        } else if (sliderIdx == 2) {
          leds2[i] = CRGB::Black;
        } else if (sliderIdx == 3) {
          leds3[i] = CRGB::Black;
        } else if (sliderIdx == 4) {
          leds4[i] = CRGB::Black;
        }
      }
    }

    // Show the updated LED colors
    FastLED.show();

    // Store the current state
    previousLitLeds = litLeds;
    previousFraction = fraction;
    lastSliderValue = sliderValue;
  }
}