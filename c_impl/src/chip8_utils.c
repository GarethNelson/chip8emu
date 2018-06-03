#include <chip8_utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* bindump_byte(uint8_t b) {
      static char retval[9];
      retval[0] = '\0';
      int z;
      for(z = 128; z>0; z >>= 1) {
           strcat(retval, ((b & z) == z) ? "1" : "0");
      }
      return retval;
}

const char* bindump_word(uint16_t w) {
      static char retval[18];
      retval[0] = '\0';
      int z;
      for(z = 32768; z>0; z>>= 1) {
           strcat(retval, ((w & z) == z) ? "1" : "0");
      }

      return retval;
}
