#include <stdio.h>
#include <stdlib.h>

#include <chip8_cpu.h>
#include <chip8_input.h>

#include <SDL.h>

#define scale 10

int main(int argc, char** argv) {
    SDL_Window*   window = NULL;
    SDL_Surface*  screen = NULL;
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { 
       fprintf(stderr,"Error setting up SDL: %s\n", SDL_GetError());
       return 1;
    }

    window = SDL_CreateWindow("CHIP8 Emulator",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64*scale, 32*scale, 0 );

    if(window==NULL) {
       fprintf(stderr,"Could not create window: %s\n", SDL_GetError());
    }

    chip8_cpu_t* CPU = chip8_new_cpu();
    chip8_load_rom(CPU, argv[1]);
    for(;CPU->registers.PC < 4096; CPU->registers.PC++) {
        printf("%s\n", chip8_disasm(CPU,CPU->registers.PC));
    }

}
