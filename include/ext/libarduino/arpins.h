#pragma once
#ifndef ARPINS_H
#define ARPINS_H

#include <cstdint>

#define not_a_pin 0

#define __LPM_enhanced__(addr) \
(__extension__({ \
    uint16_t __addr16 = (uint16_t)(addr); \
    uint8_t __result; \
    __asm__ volatile ( \
        "lpm %0, Z" "\n\t" \
        : "=r" (__result) \
        : "z" (__addr16) \
    ); \
    __result; \
}))

#define PROGMEM __attribute__((__progmem__))
#define _BV(bit) (1 << (bit))

#define __LPM(addr) __LPM_enhanced__(addr)
#define pgm_read_byte_near(address_short) __LPM((uint16_t)(address_short))
#define pgm_read_byte(address_short) pgm_read_byte_near(address_short)

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
    _BV(6),
    _BV(7),
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
};    

// digitalPinToBitMask
#define digitalPinToBitMask(pin) ( pgm_read_byte(digital_pin_to_bit_mask_PGM + (pin)) )

#endif