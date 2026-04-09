#include "fonts_gfx.h"
#include "ssd1306.h"
#include "BlueNRG1_i2c.h"
#include "SDK_EVAL_Config.h" /* pour SDK_EVAL_I2C */
#include <string.h>
#include "stdio.h"

/* Adresse utilisÃ©e (modifiable via SSD1306_SetAddress) */
static uint8_t ssd1306_addr = SSD1306_ADDR_7BIT;

uint8_t SSD1306_Buffer[1024];


/* Ecrit une commande unique */
ErrorStatus SSD1306_WriteCommand(uint8_t cmd)
{
    I2C_TransactionType t;

    t.Operation = I2C_Operation_Write;
    t.Address = ssd1306_addr;
    t.StartByte = I2C_StartByte_Disable;
    t.AddressType = I2C_AddressType_7Bit;
    t.StopCondition = I2C_StopCondition_Enable;   // âš ï¸� IMPORTANT
    t.Length = 2; // control + command

    I2C_FlushTx((I2C_Type*)SDK_EVAL_I2C);
    while (I2C_WaitFlushTx((I2C_Type*)SDK_EVAL_I2C) == I2C_OP_ONGOING);

    I2C_BeginTransaction((I2C_Type*)SDK_EVAL_I2C, &t);

    // control byte = 0x00 (command)
    I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, 0x00);
    I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, cmd);

    // wait end
    while (I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_MTD) == RESET)
    {
        if (I2C_OP_ABORTED == I2C_GetStatus((I2C_Type*)SDK_EVAL_I2C))
        {
            printf("SSD1306 command: no ACK\r\n");
            return ERROR;
        }
    }

    I2C_ClearITPendingBit((I2C_Type*)SDK_EVAL_I2C, I2C_IT_MTD);
    return SUCCESS;
}
ErrorStatus SSD1306_WriteCommands(uint8_t *cmds, uint8_t len)
{
    I2C_TransactionType t;

    t.Operation = I2C_Operation_Write;
    t.Address = ssd1306_addr;
    t.StartByte = I2C_StartByte_Disable;
    t.AddressType = I2C_AddressType_7Bit;
    t.StopCondition = I2C_StopCondition_Enable;
    t.Length = len + 1; // control byte + commands

    I2C_FlushTx((I2C_Type*)SDK_EVAL_I2C);
    while (I2C_WaitFlushTx((I2C_Type*)SDK_EVAL_I2C) == I2C_OP_ONGOING);

    I2C_BeginTransaction((I2C_Type*)SDK_EVAL_I2C, &t);

    I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, 0x00); // control byte = command

    for (uint8_t i = 0; i < len; i++)
        I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, cmds[i]);

    while (I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_MTD) == RESET);

    I2C_ClearITPendingBit((I2C_Type*)SDK_EVAL_I2C, I2C_IT_MTD);

    return SUCCESS;
}


/* Ecrit un buffer de donnÃ©es en chunks */
ErrorStatus SSD1306_WriteData(uint8_t *buf, uint16_t len)
{
  I2C_TransactionType t;
  uint16_t i = 0;
  uint8_t ctrl = 0x40; /* control byte = data */

  while (i < len) {
    uint16_t chunk = (len - i) > SSD1306_MAX_CHUNK ? SSD1306_MAX_CHUNK : (len - i);

    t.Operation = I2C_Operation_Write;
    t.Address = ssd1306_addr;
    t.StartByte = I2C_StartByte_Disable;
    t.AddressType = I2C_AddressType_7Bit;
    t.StopCondition = I2C_StopCondition_Enable;
    t.Length = chunk + 1; /* control + chunk data */

    I2C_FlushTx((I2C_Type*)SDK_EVAL_I2C);
    while (I2C_WaitFlushTx((I2C_Type*)SDK_EVAL_I2C) == I2C_OP_ONGOING);

    I2C_BeginTransaction((I2C_Type*)SDK_EVAL_I2C, &t);
    I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, ctrl);
    for (uint16_t j = 0; j < chunk; ++j) {
      I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, buf[i + j]);
    }

    /* attendre fin ou abort */
    do {
      if (I2C_OP_ABORTED == I2C_GetStatus((I2C_Type*)SDK_EVAL_I2C)) {
        printf("SSD1306 data chunk: no ACK\r\n");
        return ERROR;
      }
    } while (I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_MTD) == RESET);

    I2C_ClearITPendingBit((I2C_Type*)SDK_EVAL_I2C, I2C_IT_MTD);
    i += chunk;

  }
  return SUCCESS;
}

void SSD1306_DrawString(int16_t x, int16_t y, const char *str, const GFXfont *font, uint8_t color)
{
    while (*str) {
        char c = *str++;

        if (c < font->first || c > font->last)
            continue;

        const GFXglyph *glyph = &font->glyph[c - font->first];

        SSD1306_DrawChar(x, y, c, font, color);

        x += glyph->xAdvance;
    }
}
void SSD1306_DrawMultiline(int16_t x, int16_t y, const char *text,
                           const GFXfont *font, uint8_t color)
{
    char buffer[128];
    strncpy(buffer, text, sizeof(buffer));
    buffer[sizeof(buffer)-1] = 0;

    int16_t maxY = 64 - font->yAdvance;  // derni�re ligne affichable

    char *line = strtok(buffer, "\n");

    while (line != NULL && y <= maxY) {
        SSD1306_DrawString(x, y, line, font, color);
        y += font->yAdvance;
        line = strtok(NULL, "\n");
    }
}





void SSD1306_DrawString_GFX(int16_t x, int16_t baseline, const char *str,
                            const GFXfont *font, uint8_t color)
{
    int16_t cursor_x = x;
    int16_t cursor_y = baseline;

    while (*str) {
        char c = *str++;

        if (c == '\n') {
            cursor_x = x;
            cursor_y += font->yAdvance;
            continue;
        }

        if (c < font->first || c > font->last)
            continue;

        const GFXglyph *glyph = &font->glyph[c - font->first];

        SSD1306_DrawChar(cursor_x, cursor_y, c, font, color);

        cursor_x += glyph->xAdvance;
    }

}
void SSD1306_DrawStringAtTop(int16_t x, int16_t y_top,
                             const char *str,
                             const GFXfont *font,
                             uint8_t color)
{
    // On prend un caractÃ¨re reprÃ©sentatif (ex: 'A')
    char ref = 'A';
    if (ref < font->first || ref > font->last)
        ref = font->first;

    const GFXglyph *g = &font->glyph[ref - font->first];

    // La baseline est y_top - yOffset
    int16_t baseline = y_top - g->yOffset;

    SSD1306_DrawString_GFX(x, baseline, str, font, color);
}



void SSD1306_DrawChar(int16_t x, int16_t y, char c,
                          const GFXfont *font, uint8_t color)
{
    if (!font)
        return;

    if (c < font->first || c > font->last)
        return;

    const GFXglyph *glyph = &font->glyph[c - font->first];
    uint8_t  w  = glyph->width;
    uint8_t  h  = glyph->height;
    int8_t   xo = glyph->xOffset;
    int8_t   yo = glyph->yOffset;
    uint16_t bo = glyph->bitmapOffset;

    const uint8_t *bitmap = font->bitmap;

    uint8_t bits = 0;
    uint8_t bit  = 0;

    for (uint8_t yy = 0; yy < h; yy++) {
        for (uint8_t xx = 0; xx < w; xx++) {

            if (!(bit++ & 7)) {              // tous les 8 pixels
                bits = bitmap[bo++];         // on lit un nouvel octet
            }

            if (bits & 0x80) {               // bit de poids fort
                SSD1306_DrawPixel(x + xo + xx, y + yo + yy, color);
            }

            bits <<= 1;                      // on dÃ©cale vers la gauche
        }
    }
}
/* Initialisation minimale pour 128x64 (sÃ©quence courante) */
void SSD1306_Init(void)
{
    // Display OFF
    uint8_t cmd1[] = {0xAE};
    SSD1306_WriteCommands(cmd1, sizeof(cmd1));

    // Clock divide
    uint8_t cmd2[] = {0xD5, 0x80};
    SSD1306_WriteCommands(cmd2, sizeof(cmd2));

    // Multiplex ratio
    uint8_t cmd3[] = {0xA8, 0x3F};
    SSD1306_WriteCommands(cmd3, sizeof(cmd3));

    // Display offset
    uint8_t cmd4[] = {0xD3, 0x00};
    SSD1306_WriteCommands(cmd4, sizeof(cmd4));

    // Start line = 0
    uint8_t cmd5[] = {0x40};
    SSD1306_WriteCommands(cmd5, sizeof(cmd5));

    // Charge pump ON
    uint8_t cmd6[] = {0x8D, 0x14};
    SSD1306_WriteCommands(cmd6, sizeof(cmd6));

    // Memory mode = horizontal
    uint8_t cmd7[] = {0x20, 0x00};
    SSD1306_WriteCommands(cmd7, sizeof(cmd7));

    // Segment remap
    uint8_t cmd8[] = {0xA1};
    SSD1306_WriteCommands(cmd8, sizeof(cmd8));

    // COM scan direction
    uint8_t cmd9[] = {0xC8};
    SSD1306_WriteCommands(cmd9, sizeof(cmd9));

    // COM pins config
    uint8_t cmd10[] = {0xDA, 0x12};
    SSD1306_WriteCommands(cmd10, sizeof(cmd10));

    // Contrast
    uint8_t cmd11[] = {0x81, 0xCF};
    SSD1306_WriteCommands(cmd11, sizeof(cmd11));

    // Pre-charge
    uint8_t cmd12[] = {0xD9, 0xF1};
    SSD1306_WriteCommands(cmd12, sizeof(cmd12));

    // VCOM detect
    uint8_t cmd13[] = {0xDB, 0x40};
    SSD1306_WriteCommands(cmd13, sizeof(cmd13));

    // Resume RAM
    uint8_t cmd14[] = {0xA4};
    SSD1306_WriteCommands(cmd14, sizeof(cmd14));

    // Normal display
    uint8_t cmd15[] = {0xA6};
    SSD1306_WriteCommands(cmd15, sizeof(cmd15));

    // Display ON
    uint8_t cmd16[] = {0xAF};
    SSD1306_WriteCommands(cmd16, sizeof(cmd16));
}


void SSD1306_UpdateScreen(void)
{
    uint8_t col[] = {0x21, 0x00, 0x7F};
    SSD1306_WriteCommands(col, sizeof(col));

    uint8_t page[] = {0x22, 0x00, 0x07};
    SSD1306_WriteCommands(page, sizeof(page));

    SSD1306_WriteData(SSD1306_Buffer, 1024);
}


/* Remplit l'Ã©cran page par page avec un pattern (0x00 = clear, 0xFF = full) */
void SSD1306_FillDisplay(uint8_t color)
{
    memset(SSD1306_Buffer, color, 1024);
    SSD1306_UpdateScreen();
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= 128 || y >= 64)
        return;

    if (color)
        SSD1306_Buffer[x + (y / 8) * 128] |=  (1 << (y % 8));
    else
        SSD1306_Buffer[x + (y / 8) * 128] &= ~(1 << (y % 8));
}

void SSD1306_DrawHLine(uint8_t x, uint8_t y, uint8_t length, uint8_t color)
{
    for (uint8_t i = 0; i < length; i++)
        SSD1306_DrawPixel(x + i, y, color);
}
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    for (uint8_t i = 0; i < h; i++)
        for (uint8_t j = 0; j < w; j++)
            SSD1306_DrawPixel(x + j, y + i, color);
}









void SSD1306_DrawVLine(uint8_t x, uint8_t y, uint8_t length, uint8_t color)
{
    for (uint8_t i = 0; i < length; i++)
        SSD1306_DrawPixel(x, y + i, color);
}


/* Clear helper */
void SSD1306_ClearDisplay(void)
{
    memset(SSD1306_Buffer, 0x00, 1024);
    SSD1306_UpdateScreen();
}


/* Permet de changer l'adresse si nÃ©cessaire */
void SSD1306_SetAddress(uint8_t addr7)
{
  ssd1306_addr = addr7;
}
