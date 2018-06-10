#include <stdio.h>
#include <stdlib.h>

#include <chip8_cpu.h>
#include <chip8_input.h>

#include <SDL.h>

#define scale 10

void draw_disp(uint8_t* vram, SDL_Surface* screen) {
     SDL_Rect rect;
     int x;
     int y;
     rect.x = 0;
     rect.y = 0;
     rect.w = scale;
     rect.h = scale;
     for(x=0; x<64; x++) {
         for(y=0; y<32; y++) {
	     rect.x = x*scale;
	     rect.y = y*scale;
	     rect.w = scale;
	     rect.h = scale;
             if(vram[(x*64)+y]==1) {
                SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 255, 255));
	     } else {
                SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
	     }
	 }
     }
}

void test_font(chip8_cpu_t* CPU) {
     int i=0;
     int j=0;
     uint8_t pixel;
     for(i=0; i< 5; i++) {
	 pixel = CPU->ram[i];
         for(j=0; j<8; j++) {
             if((pixel & (0x80 >> j)) != 0) {
		CPU->vram[(i*64)+j] = 1;
	     }
    	 }
     }
}

int main(int argc, char** argv) {
    SDL_Window*   window = NULL;
    SDL_Surface*  screen = NULL;
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { 
       fprintf(stderr,"Error setting up SDL: %s\n", SDL_GetError());
       return 1;
    }

    window = SDL_CreateWindow("CHIP8 Emulator",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64*scale, 32*scale, 0 );
    screen = SDL_GetWindowSurface(window);

    if(window==NULL) {
       fprintf(stderr,"Could not create window: %s\n", SDL_GetError());
    }

    chip8_cpu_t* CPU = chip8_new_cpu();
    chip8_load_rom(CPU, argv[1]);
    for(;;) {
        chip8_iter(CPU);
//	chip8_dump_status(CPU);
//	test_font(CPU);
	draw_disp(CPU->vram,screen);
	SDL_UpdateWindowSurface(window);
    }


}
