#pragma once

/// @brief Configures and imediately enters deep sleep. When the devices wakes
/// up, any data no stored in RTC memory will be cleared.
void deepSleep() __attribute__((noreturn));
