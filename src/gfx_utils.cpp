#include <gfx_utils.h>
#include <Adafruit_GFX.h>

void copyBitmap(GFXcanvas1Read& src, int16_t sx, int16_t sy,
                Adafruit_GFX& dst, int16_t dx, int16_t dy,
                uint16_t w, uint16_t h)
{
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            dst.drawPixel(dx+i, dy+j, src.getPixel(sx+i, sy+j));
        }
    }
}


bool GFXcanvas1Read::getPixel(int16_t x, int16_t y)
{
    if(!getBuffer())
        return false;
    
    if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) 
        return false;

    int16_t t;
    switch(rotation) {
        case 1:
            t = x;
            x = WIDTH  - 1 - y;
            y = t;
            break;
        case 2:
            x = WIDTH  - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
    }

    uint8_t b = getBuffer()[(x / 8) + y * ((WIDTH + 7) / 8)];

    return b & (0x80 >> (x & 7));
}