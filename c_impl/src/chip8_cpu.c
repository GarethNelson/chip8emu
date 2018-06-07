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

const char* chip8_disasm(chip8_cpu_t* cpu, uint16_t offset) {
     static char retval[32];
     snprintf(retval,32,"%s","UNKNOWN");
     uint8_t* code = &(cpu->ram[offset]);
     uint8_t upper_nibble = UPPER_NIBBLE_U8(code[0]);
     uint8_t lower_nibble = LOWER_NIBBLE_U8(code[0]);
     switch(upper_nibble) {
	case 0x00: {
	     switch(code[1]) {
		 case 0xE0: {
		       snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "CLS");
		       break;
	         }
	         case 0xEE: {
 		       snprintf(retval,32,"%04x %02x %02x %-10s",offset, code[0], code[1], "RTS");
		       break;

	         }
                 default: {
		   snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "UNKNOWN");
		   break;
	 	 }
	     }
	     break;
	}
	case 0x01: {
	     snprintf(retval,32,"%04x %02x %02x %-10s %02x%02x", offset, code[0], code[1], "JMP",lower_nibble,code[1]);
	     break;
	}
	case 0x02: {
	     snprintf(retval,32,"%04x %02x %02x %-10s %02x%02x", offset, code[0], code[1], "CALL",lower_nibble,code[1]);
	     break;
	}
	case 0x0B: {
	     snprintf(retval,32,"%04x %02x %02x %-10s V0(%02x%02x)", offset, code[0], code[1], "JMP",lower_nibble,code[1]);
	     break;
  	}

	case 0x06: {
             snprintf(retval,32,"%04x %02x %02x %-10s V%01x,#$%02x", offset, code[0], code[1], "MOV IMM", code[0] & 0x0f, code[1]);
	     break;
	}
	case 0x0a: {
             snprintf(retval,32,"%04x %02x %02x %-10s I,#$%01x%02x", offset, code[0], code[1], "MOV IMM", code[0] & 0x0f, code[1]);
	     break;
	}
	default: {
             snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "UNKNOWN");
	}
     }


     return retval;
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
     printf("Disasm: %s\n", chip8_disasm(cpu, cpu->registers.PC));
}
