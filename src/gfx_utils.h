#ifndef GFX_UTILS
#define GFX_UTILS

#include <cstdint>

#include <Adafruit_GFX.h>

class GFXcanvas1Read: public GFXcanvas1
{
    public:
        GFXcanvas1Read(uint16_t w, uint16_t h): GFXcanvas1(w,h) {}
        bool getPixel(int16_t x, int16_t y);
};

void copyBitmap(GFXcanvas1Read& src, int16_t sx, int16_t sy,
                Adafruit_GFX& dst, int16_t dx, int16_t dy,
                uint16_t w, uint16_t h);


#endif //GFX_UTILS