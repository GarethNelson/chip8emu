#pragma once

#include <stdint.h>

typedef struct chip8_reg_t {
	uint8_t  V[16];
	uint16_t I;
	uint16_t PC;
	uint16_t SP;
	uint8_t  DT;
	uint8_t  ST;
} chip8_reg_t;

typedef struct chip8_cpu_t {
	chip8_reg_t registers;
	uint8_t     ram[4096];
	uint16_t    stack[16];
} chip8_cpu_t;

chip8_cpu_t* chip8_new_cpu();
void chip8_load_rom(chip8_cpu_t* cpu, char* filename);
void chip8_dump_status(chip8_cpu_t* cpu);
const char* chip8_disasm(chip8_cpu_t* cpu, uint16_t offset);
