#include  <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct{
     //uint8_t RESERVED_MEMORY[512];
     //uint8_t USABLE_MEMORY[4096 - 512];
     //uint8_t ETI_660_USABLE_MEMORY[4095 - 1536];
     uint8_t  MEMORY[ 4096 ];
     uint8_t DISPLAY[64][32];
     uint16_t      STACK[16];
     uint8_t           V[16]; // REGISTERS, V0 TO VF, VF IS A FLAG REGISTER
     uint16_t              I; // STORE MEMORY ADDRESS
     uint16_t             PC;
     uint16_t             SP;
     uint8_t       KEY_PRESS;
     // SOUND NOT INCLUDED
} CHIP_8;

uint8_t FONT[80] = {
    0b00111100,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00111100,

    0b00001000,
    0b00011000,
    0b00001000,
    0b00001000,
    0b00011100,

    0b00011000,
    0b00100100,
    0b00001000,
    0b00010000,
    0b00111100,

    0b00011000,
    0b00100100,
    0b00001000,
    0b00100100,
    0b00011000,

    0b00001000,
    0b00011000,
    0b00111100,
    0b00001000,
    0b00001000,

    0b00111100,
    0b00101000,
    0b00110100,
    0b00000100,
    0b00111000,

    0b00001000,
    0b00010000,
    0b00111000,
    0b01000100,
    0b00111000,

    0b00111100,
    0b00000100,
    0b00011000,
    0b00001000,
    0b00001000,

    0b00011000,
    0b00100100,
    0b00011000,
    0b00100100,
    0b00011000,

    0b00111000,
    0b01000100,
    0b00111000,
    0b00010000,
    0b00100000,

    0b00011000,
    0b00100100,
    0b00111100,
    0b00100100,
    0b00100100,

    0b00111000,
    0b00100100,
    0b00111000,
    0b00100100,
    0b00111000,

    0b00011000,
    0b00100100,
    0b00100000,
    0b00100100,
    0b00011000,

    0b00111000,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00111000,

    0b00111100,
    0b00100000,
    0b00111100,
    0b00100000,
    0b00111100,

    0b00111100,
    0b00100000,
    0b00111000,
    0b00100000,
    0b00100000
};

int main(){
     CHIP_8  *C8;

     C8 = malloc(sizeof(CHIP_8));

     memset(C8->MEMORY,  0,  4096);
     memset(C8->DISPLAY, 0, 64*32);
     memset(FONT,        0,    80);

     for(uint8_t C = 0; C < 80; C++)C8->MEMORY[C] = FONT[C];

     uint16_t OP  = C8->MEMORY[C8->PC];
     uint8_t  P   = (OP >> 12) &   0xF;
     uint8_t  X   = (OP >> 8)  &   0xF;
     uint8_t  Y   = (OP >> 4)  &   0xF;
     uint8_t  N   =  OP        &   0xF;
     uint8_t  NN  =  OP        &  0xFF;
     uint8_t  KK  = (OP >> 8)  &  0xFF;
     uint16_t NNN =  OP        & 0xFFF;

     FILE         *file;
     uint16_t file_size;
     char           chr;
     uint8_t        rnd;

     C8->PC = 512;
     C8->SP =   0;
     C8->I  =   0;

     memset(C8->MEMORY,  0,  4096);
     memset(C8->DISPLAY, 0, 64*32);
     memset(FONT,        0,    80);

     char input[12];

     puts("FILE NAME:");
     gets(input);
     file = fopen(input, "rb");
     fseek(file, 0, SEEK_END);
     file_size = ftell(file);
     fclose(file);
     file = fopen(input, "rb");

     fread(C8->MEMORY + 512, 1, file_size, file);

     switch(OP){
          case 0x0000:
               if(C8->STACK[C8->SP] != 0)++(C8->SP);
               C8->STACK[C8->SP] = C8->PC;
               C8->PC = NNN;
               break;
          case 0x00E0:
               for(uint8_t xAxis = 0; xAxis < 64; xAxis++){
                    for(uint8_t yAxis = 0; yAxis < 32; yAxis++)puts(" ");
                    puts("\n");
               }
               C8->PC += 2;
          break;
          case 0x00EE:
               C8->PC = C8->STACK[C8->SP];
               C8->STACK[C8->SP] = 0;
               if(C8->STACK[C8->SP] > 0)--(C8->SP);
               break;
          case 0x1000: C8->PC = NNN; break;
          case 0x2000:
               if(C8->STACK[C8->SP] != 0)++(C8->SP);
               C8->STACK[C8->SP] = C8->PC;
               C8->PC = NNN;
               break;
          case 0x3000: if(C8->V[X] == NN)break;
          case 0x4000: if(C8->V[X] != NN)break;
          case 0x5000: if(C8->V[X] == C8->V[Y])break;
          case 0x6000: C8->V[X] = NN; break;
          case 0x7000: C8->V[X] += NN; break;
          case 0x8000: C8->V[Y] = C8->V[X]; break;
          case 0x8001: break;
          case 0x8002: break;
          case 0x8003: break;
          case 0x8004: break;
          case 0x8005: break;
          case 0x8006: break;
          case 0x8007: break;
          case 0x800E: break;
          case 0x9000: if(C8->V[X] == C8->V[Y])break;
          case 0xA000: C8->I = NNN; break;
          case 0xB000: C8->PC = NNN + C8->V[0]; break;
          case 0xC000:
               rnd = rand() % 0xFF;
               C8->V[X] = (rnd >> 8) & 0xF;
               break;
          case 0xD000: break;
          case 0xE09E: if(chr == C8->V[X])break;
          case 0xE0A1: if(chr != C8->V[X])break;
          case 0xF007: break; // NO DELAY
          case 0xF00A:
               chr = getchar();
               C8->V[X] = chr;
               break;
          case 0xF015: break; // NO DELAY
          case 0xF018: break; // NO SOUND
          case 0xF01E: C8->I += C8->V[X]; break;
          case 0xF029: break;
          case 0xF033: break;
          case 0xF055:
               for(uint8_t i = 0; i == 16; i++){
                    C8->V[i] = C8->I;
                    C8->I += X + 1;
               }
               break;
          case 0xF065:
               for(uint8_t i = 0; i == 16; i++){
                    C8->V[i] += C8->I;
                    C8->I += X + 1;
               }
               break;
          default: puts("INVALID"); break;
     }
}
