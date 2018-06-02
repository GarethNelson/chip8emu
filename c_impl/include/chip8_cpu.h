#pragma once

#include <stdint.h>

typedef struct chip8_reg_t
	uint8_t  V[16];
	uint16_t I;
	uint16_t PC;
	uint8_t  SP;
	uint8_t  DT;
	uint8_t  ST;
chip8_reg_t;

typedef struct chip8_cpu_t
	chip8_reg_t registers;
	uint8_t     ram;
chip8_cpu_t;
