#pragma once

#include <U8g2lib.h>

#include "Config.h"
#include "Globals.h"

// ===================================
// METHODS
inline void setupDisplay() {
#if defined(DISPLAY_SSD1309)
    U8G2_SSD1309_128X64_NONAME0_F_HW_I2C display_(
        U8G2_R2, PIN_DISPLAY_RST, PIN_DISPLAY_SCL, PIN_DISPLAY_SDA);
#elif defined(DISPLAY_SSD1306)
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display_(
        U8G2_R0, PIN_DISPLAY_RST, PIN_DISPLAY_SCL, PIN_DISPLAY_SDA);
#elif defined(DISPLAY_8X8_MAX7219)
    //U8G2_MAX7219_8X8_F_4W_HW_SPI display_(U8G2_R2, PIN_SPI_CS, 255);
    U8G2_MAX7219_8X8_F_4W_SW_SPI display_(U8G2_R2, PIN_SPI_CLK, PIN_SPI_MOSI, PIN_SPI_CS, 255);
#else
#error No valid display defined!
#endif

#ifdef WIFIKI32
    pinMode(21, OUTPUT);
    digitalWrite(21, LOW);
#endif

    display = display_;
    display.begin();
}

inline void scrollText(const char* str) {
    display.clear();
    display.setFont(u8g2_font_5x8_tr);
    const int len = display.getStrWidth(str) + 2*DISPLAY_WIDTH;
    for (int i = -DISPLAY_WIDTH; i < len; i++) {
        display.clearBuffer();
        display.drawStr(-i, DISPLAY_HEIGHT-1, str);
        display.sendBuffer();
        delay(50);
    }
}