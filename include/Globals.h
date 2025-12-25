/*
 * Project wide globals
 */

#pragma once

#include <U8g2lib.h>

extern U8G2 display;

extern int8_t encoder_dir;
extern bool encoder_pressed;
extern bool encoder_down;

extern ulong innactivity_timeout_time_ms;
