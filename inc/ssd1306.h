#ifndef SSD1306_H
#define SSD1306_H
#include "fonts_gfx.h"
#include <stdint.h>
#include "BlueNRG1_conf.h"   // pour ErrorStatus


/* Adresse 7-bit par défaut (modifiable si nécessaire) */
#define SSD1306_ADDR_7BIT  0x3C

extern uint8_t SSD1306_Buffer[1024];

/* Taille écran supportée (128x64) */
#define SSD1306_WIDTH      128
#define SSD1306_HEIGHT     64
#define SSD1306_PAGES      (SSD1306_HEIGHT / 8)

/* Chunk max pour l'envoi de données (ajustable selon FIFO) */
#define SSD1306_MAX_CHUNK  8

/* API publique */
void SSD1306_Init(void);
void SSD1306_FillDisplay(uint8_t pattern);
void SSD1306_ClearDisplay(void);
void SSD1306_UpdateScreen(void);
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void SSD1306_DrawHLine(uint8_t x, uint8_t y, uint8_t length, uint8_t color);
void SSD1306_DrawVLine(uint8_t x, uint8_t y, uint8_t length, uint8_t color);
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void SSD1306_DrawString(int16_t x, int16_t y, const char *str,
		const GFXfont *font, uint8_t color);
void SSD1306_DrawString_GFX(int16_t x, int16_t baseline, const char *str,
                            const GFXfont *font, uint8_t color);

void SSD1306_DrawMultiline(int16_t x, int16_t y, const char *text,
                           const GFXfont *font, uint8_t color);
void SSD1306_DrawChar(int16_t x, int16_t y, char c, const GFXfont *font,
		uint8_t color);
void SSD1306_DrawStringAtTop(int16_t x, int16_t y_top, const char *str,
		const GFXfont *font, uint8_t color);

ErrorStatus SSD1306_WriteCommand(uint8_t cmd);
ErrorStatus SSD1306_WriteCommands(uint8_t *cmds, uint8_t len);
ErrorStatus SSD1306_WriteData(uint8_t *data, uint16_t len);

/* Optionnel : changer l'adresse si votre module diffère */
void SSD1306_SetAddress(uint8_t addr7);

#endif /* SSD1306_H */
