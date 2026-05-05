#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include "arpins.h"
#include "arport.h"

void __cdecl pinMode(uint8_t pin, uint8_t mode) {
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *reg, *out;

    if (port == not_a_pin) return;
}