#pragma once

#include <stdint.h>

#define UPPER_NIBBLE_U8(x) ((x >> 4) & 0x0F)
#define LOWER_NIBBLE_U8(x) (x & 0x0F)

const char* bindump_byte(uint8_t b); // dumps a byte to binary
const char* bindump_word(uint16_t w); // dumps a 2-byte word to binary
