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

	memset(retval->ram,  '\0',4096);
	retval->registers.I  = 0;
	retval->registers.PC = 0x200; // where programs start by default
	retval->registers.DT = 0;
	retval->registers.ST = 0;
	retval->registers.SP = CPU8_STACK_START;

	memcpy(retval->ram, chip8_rom_font, 80);
	retval->vram = malloc(64*32);
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
		 case 0x00: {
		       snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "NOP");
		       break;
		 }
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
	case 0x03: {
	     snprintf(retval,32,"%04x %02x %02x %-10s V%01x %02x", offset, code[0], code[1], "SKIP.EQ",lower_nibble,code[1]);
	     break;
	}
	case 0x04: {
	     snprintf(retval,32,"%04x %02x %02x %-10s V%01x %02x", offset, code[0], code[1], "SKIP.NE",lower_nibble,code[1]);
	     break;
	}

   	case 0x05: {
	     snprintf(retval,32,"%04x %02x %02x %-10s V%01x V%01x", offset, code[0], code[1], "SKIP.EQ",lower_nibble,UPPER_NIBBLE_U8(code[1]));
	     break;
	}
	case 0x09: {
	     snprintf(retval,32,"%04x %02x %02x %-10s V%01x V%01x", offset, code[0], code[1], "SKIP.NE",lower_nibble,UPPER_NIBBLE_U8(code[1]));
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
	case 0x08: {
	     switch(LOWER_NIBBLE_U8(code[1])) {
		case 0x00: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x", offset, code[0], code[1], "MOV", lower_nibble, UPPER_NIBBLE_U8(code[1]));
		     break;
		}
		case 0x01: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x", offset, code[0], code[1], "OR", lower_nibble, UPPER_NIBBLE_U8(code[1]));
		     break;
		}
		case 0x02: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x", offset, code[0], code[1], "AND", lower_nibble, UPPER_NIBBLE_U8(code[1]));
		     break;
		}
		case 0x03: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x", offset, code[0], code[1], "XOR", lower_nibble, UPPER_NIBBLE_U8(code[1]));
		     break;
		}
		case 0x04: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x", offset, code[0], code[1], "ADD.", lower_nibble, UPPER_NIBBLE_U8(code[1]));
		     break;
		}
		case 0x05: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x", offset, code[0], code[1], "SUB.", lower_nibble, UPPER_NIBBLE_U8(code[1]));
		     break;
		}
		case 0x06: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x", offset, code[0], code[1], "SHR.", lower_nibble);
		     break;
		}
		case 0x07: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x", offset, code[0], code[1], "SUBB.", lower_nibble, UPPER_NIBBLE_U8(code[1]));
		     break;
		}
		case 0x0E: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x", offset, code[0], code[1], "SHL.", lower_nibble);
		     break;
		}
	        default: {
		   snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "UNKNOWN");
		   break;
	 	}
	     }
	     break;
	}
	case 0x0A: {
             snprintf(retval,32,"%04x %02x %02x %-10s I,#$%01x%02x", offset, code[0], code[1], "MOV IMM", code[0] & 0x0f, code[1]);
	     break;
	}
	case 0x0D: {
	     snprintf(retval,32,"%04x %02x %02x %-10s V%01x,V%01x #$%01x", offset, code[0], code[1], "SPRITE", lower_nibble, UPPER_NIBBLE_U8(code[1]),LOWER_NIBBLE_U8(code[1]));	     
	     break;
	}
	case 0xE: {
	     switch(code[1]) {
		case 0x9E:{
		     snprintf(retval,32,"%04x %02x %02x %-10s",offset, code[0], code[1], "SKIP.KEY");
	             break;
	        }

		case 0xA1:{
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x",offset, code[0], code[1], "SKIP.NOKEY",lower_nibble);
	             break;
	        }
                default: {
		   snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "UNKNOWN");
		   break;
	 	}
	     }
	     break;
	}
	case 0xF:{
	     switch(code[1]) {
		case 0x07: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x DELAY", offset, code[0], code[1], "MOV", lower_nibble);
	             break;
		}
		case 0x0A: {
		     snprintf(retval,32,"%04x %02x %02x %-10s V%01x",offset, code[0], code[1], "WAITKEY",lower_nibble);
	             break;
	        }
		case 0x15: {
		     snprintf(retval,32,"%04x %02x %02x %-10s DELAY V%01x", offset, code[0], code[1], "MOV", lower_nibble);
	             break;
 		}
		case 0x18: {
		     snprintf(retval,32,"%04x %02x %02x %-10s SOUND V%01x", offset, code[0], code[1], "MOV", lower_nibble);
	             break;
 		}
		case 0x1E: {
                     snprintf(retval,32,"%04x %02x %02x %-10s I V%01x", offset, code[0], code[1], "ADD", lower_nibble);
	             break;
	        }
		case 0x29: {
                     snprintf(retval,32,"%04x %02x %02x %-10s V%01x", offset, code[0], code[1], "SPRITECHAR", lower_nibble);
	             break;
 		}
		case 0x33: {
                     snprintf(retval,32,"%04x %02x %02x %-10s V%01x", offset, code[0], code[1], "MOVBCD", lower_nibble);
	             break;
	        }
		case 0x55: {
                     snprintf(retval,32,"%04x %02x %02x %-10s (I), V0-V%01x", offset, code[0], code[1], "MOVM", lower_nibble);
	             break;
	        }
		case 0x65: {
                     snprintf(retval,32,"%04x %02x %02x %-10s V0-V%01x, (I)", offset, code[0], code[1], "MOVM", lower_nibble);
	             break;
	        }
		default: {
		   snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "UNKNOWN");
		   break;
	 	}
	     }
	     break;
	}


	default: {
             snprintf(retval,32,"%04x %02x %02x %-10s", offset, code[0], code[1], "UNKNOWN");
	}
     }


     return retval;
}

void UnimplementedInstruction(chip8_cpu_t* cpu, uint8_t* code) {
     chip8_dump_status(cpu);
     printf("UNIMPLEMENTED INSTRUCTION: %s\n\n", chip8_disasm(cpu, cpu->registers.PC));
     cpu->registers.PC += 2;
}

void Op0(chip8_cpu_t* cpu, uint8_t* code) {
     switch(code[1]) {
         case 0xE0: { // CLS
              memset(cpu->vram, 0, 64*32);
	      cpu->registers.PC += 2;
	      break;
	 }
	 case 0xEE: { // RTS
	      cpu->registers.PC  = *((uint16_t*)&(cpu->ram[cpu->registers.SP]));
	      cpu->registers.SP += 2;
	      break;
	 }
	 default: UnimplementedInstruction(cpu, code); break;
     }
}

void Op1(chip8_cpu_t* cpu, uint8_t* code) {
     // JUMP $NNN
     cpu->registers.PC = ((code[0]&0xf)<<8) | code[1];
}

void Op2(chip8_cpu_t* cpu, uint8_t* code) {
     UnimplementedInstruction(cpu,code);
}

void Op3(chip8_cpu_t* cpu, uint8_t* code) {
     uint8_t reg = code[0] & 0xf;    
     if(cpu->registers.V[reg] == code[1]) cpu->registers.PC += 2;
     cpu->registers.PC += 2;  
}

void Op4(chip8_cpu_t* cpu, uint8_t* code) {
     chip8_dump_status(cpu);
     uint8_t reg_a = LOWER_NIBBLE_U8(code[0]);
     if(cpu->registers.V[reg_a] != code[1]) cpu->registers.PC += 2;
     cpu->registers.PC += 2;
}

void Op5(chip8_cpu_t* cpu, uint8_t* code) {
     uint8_t reg_a = LOWER_NIBBLE_U8(code[0]);
     uint8_t reg_b = UPPER_NIBBLE_U8(code[1]);
     if(cpu->registers.V[reg_a] == cpu->registers.V[reg_b]) cpu->registers.PC += 2;
     cpu->registers.PC += 2;
}

void Op6(chip8_cpu_t* cpu, uint8_t* code) {
     uint8_t reg = code[0] & 0xf;    
     cpu->registers.V[reg] = code[1];
     cpu->registers.PC+=2;   
}

void OpA(chip8_cpu_t* cpu, uint8_t* code) {
     uint16_t* op = (uint16_t*)code;
     cpu->registers.I = (*op) & 0x0FFF;
     cpu->registers.PC+=2;
}

void OpD(chip8_cpu_t* cpu, uint8_t* code) {
     uint8_t x = cpu->registers.V[UPPER_NIBBLE_U8(code[0])];
     uint8_t y = cpu->registers.V[UPPER_NIBBLE_U8(code[1])];
     uint8_t n = LOWER_NIBBLE_U8(code[1]);
     uint8_t pixel;
     int i=0;
     int j=0;     
     for(i=0; i<n; i++) {
         pixel = cpu->ram[cpu->registers.I + i];
         for(int j=0; j < 8; j++) {
 	     if((pixel & (0x80 >> j)) != 0) {
                cpu->vram[(x+j*64)+y+i] = 1;						
             }
	 }
     }
     cpu->registers.PC+=2;
}

void OpF(chip8_cpu_t* cpu, uint8_t* code) {
     switch(code[1]) { 
	  case 0x29: {
               cpu->registers.I = (cpu->registers.V[LOWER_NIBBLE_U8(code[0])] * 5);
	       cpu->registers.PC += 2;
	       break;
	   	     }
          default: UnimplementedInstruction(cpu,code); break;
     }
}

void chip8_iter(chip8_cpu_t* cpu) {
     uint8_t* code = &(cpu->ram[cpu->registers.PC]);
     uint8_t upper_nibble = UPPER_NIBBLE_U8(code[0]);
     uint8_t lower_nibble = LOWER_NIBBLE_U8(code[0]);     
     switch(upper_nibble) {
        case 0x00: Op0(cpu, code); break;
        case 0x01: Op1(cpu, code); break;
        case 0x02: Op2(cpu, code); break;
        case 0x03: Op3(cpu, code); break;
        case 0x04: Op4(cpu, code); break;
        case 0x05: Op5(cpu, code); break;
        case 0x06: Op6(cpu, code); break;
/*        case 0x07: Op7(cpu, code); break;
        case 0x08: Op8(cpu, code); break;
        case 0x09: Op9(cpu, code); break;*/
        case 0x0a: OpA(cpu, code); break;
/*        case 0x0b: OpB(cpu, code); break;
        case 0x0c: OpC(cpu, code); break;*/
        case 0x0d: OpD(cpu, code); break;
/*        case 0x0e: OpE(cpu, code); break;*/
        case 0x0f: OpF(cpu, code); break;
        default: UnimplementedInstruction(cpu, code); break;
     }
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
