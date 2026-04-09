#ifndef SSD1306_GFX_H
#define SSD1306_GFX_H

#include <stdint.h>
#include <stddef.h>

// -------------------------
// STRUCTURES GFX STANDARD
// -------------------------

typedef struct {
    uint16_t bitmapOffset;  // Offset dans le bloc bitmap
    uint8_t  width;         // Largeur du glyphe
    uint8_t  height;        // Hauteur du glyphe
    uint8_t  xAdvance;      // Avancement horizontal
    int8_t   xOffset;       // Décalage X
    int8_t   yOffset;       // Décalage Y
} GFXglyph;

typedef struct {
    const uint8_t  *bitmap;   // Données bitmap
    const GFXglyph *glyph;    // Jump table
    uint8_t first;            // Premier caractère
    uint8_t last;             // Dernier caractère
    uint8_t yAdvance;         // Hauteur de ligne
} GFXfont;

// -------------------------
// PROTOTYPES DES FONCTIONS
// -------------------------

void SSD1306_DrawChar_gfx(int16_t x, int16_t y, char c, const GFXfont *font, uint8_t color);
void SSD1306_DrawString_GFX(int16_t x, int16_t y, const char *str, const GFXfont *font, uint8_t color);

// -------------------------
// POLICES EXTERNES
// -------------------------

const GFXfont FreeMono9pt7b;
const GFXfont Picopixel;
const GFXfont FreeSansBoldOblique12pt7b;

#endif

