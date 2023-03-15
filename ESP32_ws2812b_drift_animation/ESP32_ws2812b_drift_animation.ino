#include <FastLED.h>

#define LED_PIN  13
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS 32
#define LED_COLS 8
#define LED_ROWS 8
#define POT_PIN     4

#define NUM_LEDS (LED_COLS * LED_ROWS)

CRGBPalette16 dynamicPalette;
CRGB leds[NUM_LEDS];
uint8_t value;
// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;
const bool    kMatrixVertical = false;

int potValue;
int colorIndex;


//bla bla
void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(115200);

  // инициализация динамической палитры
  dynamicPalette = CRGBPalette16(
                     CRGB::Black,
                     CRGB::Blue,
                     CRGB::DeepSkyBlue,
                     CRGB::Turquoise,
                     CRGB::MintCream,
                     CRGB::PeachPuff,
                     CRGB::OrangeRed,
                     CRGB::Purple,
                     CRGB::Black,
                     CRGB::Blue,
                     CRGB::DeepSkyBlue,
                     CRGB::Turquoise,
                     CRGB::MintCream,
                     CRGB::PeachPuff,
                     CRGB::OrangeRed,
                     CRGB::Purple
                   );
}

void loop() {
  // put your main code here, to run repeatedly:
  draw();
  FastLED.show();
  // чтение значения потенциометра и масштабирование его до диапазона цветов
  potValue = analogRead(POT_PIN);
  colorIndex = map(potValue, 0, 4096, 0, 255);


}







#define CenterX ((LED_COLS / 2) - 0.5)
#define CenterY ((LED_ROWS / 2) - 0.5)
const byte maxDim = max(LED_COLS, LED_ROWS);

void drawPixelXYF(float x, float y, const CRGB & color) {
  // extract the fractional parts and derive their inverses
  uint8_t xx = (x - (int) x) * 255;
  uint8_t yy = (y - (int) y) * 255;
  uint8_t ix = 255 - xx;
  uint8_t iy = 255 - yy;
  // calculate the intensities for each affected pixel
#define WU_WEIGHT(a, b)((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {
    WU_WEIGHT(ix, iy),
    WU_WEIGHT(xx, iy),
    WU_WEIGHT(ix, yy),
    WU_WEIGHT(xx, yy)
  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XY(xn, yn)];
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    leds[XY(xn, yn)] = clr;
  }
#undef WU_WEIGHT
}


uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;

  if ( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * LED_ROWS) + x;
    } else {
      i = LED_COLS * (LED_ROWS - (x + 1)) + y;
    }
  }

  if ( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if ( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (LED_ROWS - 1) - x;
        i = (y * LED_ROWS) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * LED_ROWS) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = LED_COLS * (LED_ROWS - (x + 1)) + y;
      } else {
        i = LED_COLS * (LED_ROWS - x) - (y + 1);
      }
    }
  }

  return i;
}



void draw() {
  FastLED.clear(); //fadeToBlackBy(leds, NUM_LEDS, 16);
  unsigned long t = millis() / 10;
  for (float i = 1; i < maxDim / 2.; i += 0.5) {
    double angle = radians(t * (maxDim / 2 - i));
    drawPixelXYF(CenterX + sin(angle) * i, CenterY + cos(angle) * i, ColorFromPalette(dynamicPalette, colorIndex));
  }

}
