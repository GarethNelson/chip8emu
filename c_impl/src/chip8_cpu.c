#include <chip8_cpu.h>
#include <chip8_font.h>

#include <stdlib.h>
#include <string.h>

chip8_cpu_t* chip8_new_cpu() {
	chip8_cpu_t* retval = (chip8_cpu_t*)calloc(sizeof(chip8_cpu_t),1);
	// yes, calloc is meant to zero stuff out, some platforms don't and i want this code portable, for reasons
	memset(retval->registers.V,'\0',16);

	memset(retval->stack,'\0',16);
	memset(retval->ram,  '\0',4096);
	retval->registers.I  = 0;
	retval->registers.PC = 0x200; // where programs start by default
	retval->registers.DT = 0;
	retval->registers.ST = 0;
	retval->registers.SP = 0;

	memcpy(retval->ram, chip8_rom_font, 80);
	return retval;
}
