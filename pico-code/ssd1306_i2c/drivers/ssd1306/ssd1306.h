/*
This file defines all needed APIs to drive
SSD1306 OLED Display through I2C
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "ssd1306_def.h"

typedef struct{
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;

    int buflen;
} render_area;

void calc_render_area_buflen( render_area *area);

void SSD1306_send_cmd(uint8_t cmd);

void SSD1306_send_cmd_list(uint8_t *buf, int num);

void SSD1306_send_buf(uint8_t buf[], int buflen);

void SSD1306_init();

void SSD1306_scroll(bool on);

void render(uint8_t *buf,  render_area *area);

void SetPixel(uint8_t *buf, int x,int y, bool on);

// Basic Bresenhams.
 void DrawLine(uint8_t *buf, int x0, int y0, int x1, int y1, bool on);

  int GetFontIndex(uint8_t ch);

 void WriteChar(uint8_t *buf, int16_t x, int16_t y, uint8_t ch);

 void WriteString(uint8_t *buf, int16_t x, int16_t y, char *str);