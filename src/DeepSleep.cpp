#include "DeepSleep.h"

#include <Arduino.h>

#include "Globals.h"
#include "Config.h"
#include "debug.h"
#include "driver/rtc_io.h"

#define PIN_WAKEUP PIN_ENCODER_SW

void deepSleep() {
    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    rtc_gpio_deinit((gpio_num_t)PIN_WAKEUP);  // ensure RTC can control it
    rtc_gpio_pulldown_dis((gpio_num_t)PIN_WAKEUP);
    rtc_gpio_pullup_en((gpio_num_t)PIN_WAKEUP);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, 0);

    display.clear();
    // These serial commands are needed for the esp to enter sleep correctly,
    // for some reason that bewilders me.
    dbg_log_info("Night night...");
    Serial.flush();
    esp_deep_sleep_start();
}
