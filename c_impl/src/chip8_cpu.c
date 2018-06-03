#include <chip8_cpu.h>
#include <chip8_font.h>
#include <chip8_utils.h>

#include <stdio.h>
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

void chip8_load_rom(chip8_cpu_t* cpu, char* filename) {
     FILE* fd = fopen(filename,"rb");
     fseek(fd, 0, SEEK_END);
     size_t fd_size = ftell(fd);
     rewind(fd);
     // yeah, this is stupid
     fread(cpu->ram + 0x200, fd_size, 1, fd);
     fclose(fd);
}

void chip8_dump_status(chip8_cpu_t* cpu) {
     printf("V:  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", 
            cpu->registers.V[0],
            cpu->registers.V[1],
            cpu->registers.V[2],
            cpu->registers.V[3],
            cpu->registers.V[4],
            cpu->registers.V[5],
            cpu->registers.V[6],
            cpu->registers.V[7],
            cpu->registers.V[8],
            cpu->registers.V[9],
            cpu->registers.V[10],
            cpu->registers.V[11],
            cpu->registers.V[12],
            cpu->registers.V[13],
            cpu->registers.V[14],
            cpu->registers.V[15]
	   );
     printf("PC: 0x%04x I: 0x%04x SP: 0x%02x DT: 0x%02x ST: 0x%02x\n",
            cpu->registers.PC,
	    cpu->registers.I,
	    cpu->registers.SP,
	    cpu->registers.DT,
	    cpu->registers.ST);
     printf("OP: 0x%02x%02x\n", cpu->ram[cpu->registers.PC],cpu->ram[cpu->registers.PC+1] );
}
