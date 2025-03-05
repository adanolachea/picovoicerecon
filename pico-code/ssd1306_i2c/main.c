/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "drivers/ssd1306/raspberry26x32.h"
#include "drivers/ssd1306/ssd1306_def.h"
#include "drivers/ssd1306/ssd1306.h"

int main() {
    stdio_init_all();

#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c / SSD1306_i2d example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    // useful information for picotool
    bi_decl(bi_2pins_with_func(28, 29, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("SSD1306 OLED driver I2C example for the Raspberry Pi Pico"));

    printf("Hello, SSD1306 OLED display! Look at my raspberries..\n");

    // I2C is "open drain", pull ups to keep signal high when no data is being
    // sent
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(28, GPIO_FUNC_I2C);
    gpio_set_function(29, GPIO_FUNC_I2C);
    gpio_pull_up(28);
    gpio_pull_up(29);

    // run through the complete initialization process
    SSD1306_init();

    // Initialize render area for entire frame (SSD1306_WIDTH pixels by SSD1306_NUM_PAGES pages)
     render_area frame_area = {
        start_col: 0,
        end_col : SSD1306_WIDTH - 1,
        start_page : 0,
        end_page : SSD1306_NUM_PAGES - 1
        };

    calc_render_area_buflen(&frame_area);

    // zero the entire display
    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);

    // render 3 cute little raspberries
    // render_area area = {
    //     start_page : 0,
    //     end_page : (IMG_HEIGHT / SSD1306_PAGE_HEIGHT)  - 1
    // };

restart:

    printf("Here we go!\n");

    // area.start_col = 0;
    // area.end_col = IMG_WIDTH - 1;

    // calc_render_area_buflen(&area);

    // uint8_t offset = 5 + IMG_WIDTH; // 5px padding

    // for (int i = 0; i < 3; i++) {
    //     render(raspberry26x32, &area);
    //     area.start_col += offset;
    //     area.end_col += offset;
    // }

    // SSD1306_scroll(true);
    // sleep_ms(5000);
    // SSD1306_scroll(false);

    char *text[] = {
        "              ",
        " ERA UNA VEZ  ",
        "  UNA PINCHI  ",
        "  VIEJA CARA  ",
        "   DE GATO    ",
        "MIAU MIAU MIAU",
        "              ",
        ""
    };

    int y = 0;
    for (uint i = 0 ;i < count_of(text); i++) {
        WriteString(buf, 5, y, text[i]);
        y+=8;
    }
    render(buf, &frame_area);

    // Test the display invert function
    sleep_ms(1500);
    SSD1306_send_cmd(SSD1306_SET_INV_DISP);
    sleep_ms(1500);
    SSD1306_send_cmd(SSD1306_SET_NORM_DISP);

    // bool pix = true;
    // for (int i = 0; i < 2;i++) {
    //     for (int x = 0;x < SSD1306_WIDTH;x++) {
    //         DrawLine(buf, x, 0,  SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pix);
    //         render(buf, &frame_area);
    //     }

    //     for (int y = SSD1306_HEIGHT-1; y >= 0 ;y--) {
    //         DrawLine(buf, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pix);
    //         render(buf, &frame_area);
    //     }
    //     pix = false;
    // }

    goto restart;

#endif
    return 0;
}
