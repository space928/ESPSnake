#include <Arduino.h>
#include <Wire.h>

#include "Config.h"
#include "DeepSleep.h"
#include "Globals.h"
#include "debug.h"
#include "driver/gpio.h"
#include "soc/gpio_struct.h"

// ===================================
// GLOBALS
int8_t encoder_dir;
bool encoder_pressed;
bool encoder_down;

ulong innactivity_timeout_time_ms;

// ===================================
// LOCALS
volatile int8_t encoder_dir_internal;
volatile int8_t encoder_dir_internal_rem;
int8_t encoder_old_state;
ulong last_input_time;

// Rotary encoder state machine from:
// http://www.mathertel.de/Arduino/RotaryEncoderLibrary.aspx The array holds the
// values -1 for the entries where a position was decremented, a 1 for the
// entries where the position was incremented and 0 in all the other (no change
// or not valid) cases.
const int8_t KNOB_DIR[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

// ===================================
// METHODS

static inline uint8_t readEncoderAB() {
    uint32_t in = GPIO.in;
    uint8_t a = (in >> PIN_ENCODER_A) & 1;
    uint8_t b = (in >> PIN_ENCODER_B) & 1;
    return (a << 1) | b;
}

void IRAM_ATTR encoderISR() {
    uint8_t s = readEncoderAB();
    encoder_dir_internal -= KNOB_DIR[(encoder_old_state << 2) | s];
    encoder_old_state = s;
}

void setupIO() {
    // set rotary encoder inputs and interrupts
    pinMode(PIN_ENCODER_A, INPUT_PULLUP);
    pinMode(PIN_ENCODER_B, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), encoderISR, CHANGE);

    innactivity_timeout_time_ms = -1;
}

#define GET_RISING_EDGE(edge, state, expr) \
    {                                      \
        bool _b = expr;                    \
        edge = _b && !state;               \
        state = _b;                        \
    }

void tickIO() {
    // Check encoder press
    GET_RISING_EDGE(encoder_pressed, encoder_down,
                    !digitalRead(PIN_ENCODER_SW));

    noInterrupts();
    encoder_dir_internal_rem = encoder_dir_internal & 3;
    encoder_dir = encoder_dir_internal >> 2;
    encoder_dir_internal = encoder_dir_internal_rem;
    interrupts();

    // encoder_dir = encoder_dir < 0 ? -encoder_dir * encoder_dir
    //                               : encoder_dir * encoder_dir;  //
    //                               Acceleration

    // if (encoder_pressed) dbg_log_info("[IO] Encoder pressed!");
    // if (encoder_dir != 0) dbg_log_info("[IO] Encoder dir: %d", encoder_dir);

    // Innactivity timeout
    if (encoder_dir || encoder_pressed) last_input_time = millis();
    if (innactivity_timeout_time_ms != -1 &&
        millis() - last_input_time > innactivity_timeout_time_ms) {
        dbg_log_info("User has been innactive for >%d ms, shutting down...",
                     innactivity_timeout_time_ms);
        deepSleep();
    }
}
