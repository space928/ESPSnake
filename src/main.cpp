#include <Arduino.h>

#include "Display.h"
#include "EncoderKeysIO.h"
#include "Snake.h"
#include "debug.h"
#include "Globals.h"

U8G2 display;

void setup() {
    setupDebug();
    dbg_log_info("Starting...");
    setupIO();
    setupDisplay();
    initSnake();

    innactivity_timeout_time_ms = 20000;
}

void loop() {
    tickIO();
    drawSnake();
    /*display.clearBuffer();
    display.setDrawColor(1);
    display.drawLine(0, 0, 8, 8);*/
    if (encoder_pressed)
        scrollText("Oi! Who poked me?");
    display.sendBuffer();
}
