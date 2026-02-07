#ifdef ENABLE_DISPLAY_RGBLED
#include <Adafruit_NeoPixel.h>

#include "display.h"

namespace Display {

Adafruit_NeoPixel pixel(1, PIN_WS2812, NEO_GRB);

bool init() {
    return pixel.begin();
    pixel.clear();
    pixel.show();
}

void showIdleScreen() {
}

void showConnectingScreen(int offset) {
    pixel.setPixelColor(0, pixel.Color(0, 128, 0));
    pixel.show();
}

void showProgressScreen(size_t bytes, size_t totalBytes, size_t bytesPerImage, const char* label) {
    float pct = (float)bytes / totalBytes;
    pixel.setPixelColor(0, pixel.ColorHSV(110.0f * pct));
    pixel.show();
}

void showMountedScreen() {
    pixel.setPixelColor(0, pixel.Color(0, 0, 1));
    pixel.show();
}

}  // namespace Display

#endif