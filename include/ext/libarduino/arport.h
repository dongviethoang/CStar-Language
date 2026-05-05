#ifndef ARPORT_H
#define ARPORT_H

#include <cstdint>
#include "arpins.h"

#define not_a_port 0
#define PD 4
#define PB 2
#define PC 3

#define _SFR_IO8(address) (*(volatile uint8_t *)(address + 0x20))
#define DDRB _SFR_IO8(0x04)
#define DDRC _SFR_IO8(0x07)
#define DDRD _SFR_IO8(0x0A)

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PB,
    PB,
    PB,
    PB,
    PB,
    PC,
    PC,
    PC,
    PC,
    PC,
    PC,
};

#define __LPM_word(addr) ({
    uint16_t __addr16 = (uint16_t)(addr); \
    uint16_t __result; \
    __asm__ volatile ( \
        "lpm %A0, Z+" "\n\t" \
        "lpm %B0, Z" "\n\t" \
        : "=r" (__result) \
        : "z" (__addr16) \
    ); \
    __result; \
})

#define pgm_read_word_near(address_short) __LPM_word((uint16_t)(address_short))
#define pgm_read_word(address_short) pgm_read_word_near(address_short)

const uint16_t PROGMEM port_to_mode_PGM[] = {
    not_a_port,
    not_a_port,
    (uint16_t) &DDRB,
    (uint16_t) &DDRC,
    (uint16_t) &DDRD,
};

#define digitalPinToPort(pin) ( pgm_read_byte(digital_pin_to_port_PGM + (pin)) )
#endif // ARPORT_H