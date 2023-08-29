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
     uint8_t              DT; // DELAY TIMER
     uint8_t       KEY_PRESS;
     // SOUND NOT INCLUDED
} CHIP_8;

uint8_t FONT[80] = {
     0x60, 0xA0, 0xA0, 0xA0, 0xC0, // 0
     0x40, 0xC0, 0x40, 0x40, 0xE0, // 1
     0xC0, 0x20, 0x40, 0x80, 0xE0, // 2
     0xC0, 0x20, 0x40, 0x20, 0xC0, // 3
     0x20, 0xA0, 0xE0, 0x20, 0x20, // 4
     0xE0, 0x80, 0xC0, 0x20, 0xC0, // 5
     0x40, 0x80, 0xC0, 0xA0, 0x40, // 6
     0xE0, 0x20, 0x60, 0x40, 0x40, // 7
     0x40, 0xA0, 0x40, 0xA0, 0x40, // 8
     0x40, 0xA0, 0x60, 0x20, 0x40, // 9
     0x40, 0xA0, 0xE0, 0xA0, 0xA0, // A
     0xC0, 0xA0, 0xC0, 0xA0, 0xC0, // B
     0x60, 0x80, 0x80, 0x80, 0x60, // C
     0xC0, 0xA0, 0xA0, 0xA0, 0xC0, // D
     0xE0, 0x80, 0xC0, 0x80, 0xE0, // E
     0xE0, 0x80, 0xC0, 0x80, 0x80  // F
};

int main(){
     CHIP_8  *C8;

     C8 = malloc(sizeof(CHIP_8));

     C8->PC = 512;
     C8->I  =   0;
     C8->SP =   0;

     memset(C8->MEMORY,  0,  4096);
     memset(C8->STACK,   0,    16);
     memset(C8->DISPLAY, 0, 64*32);
     memset(C8->V,       0,    16);
     memset(FONT,        0,    80);

     for(uint8_t i = 0; i < 80; i++)C8->MEMORY[i] = FONT[i];

     C8->DT        = 0;
     C8->KEY_PRESS = 0;

     uint16_t OP  = C8->MEMORY[C8->PC] | C8->MEMORY[C8->PC + 1];
     uint8_t  HI  = C8->MEMORY[C8->PC++];
     uint8_t  LO  = C8->MEMORY[C8->PC++];
     uint8_t  P   = HI >>    0x4;
     uint8_t  X   = HI &     0xF;
     uint8_t  Y   = LO >>    0x4;
     uint8_t  N   = LO &     0xF;
     uint8_t  NN  = OP &    0xFF;
     uint16_t NNN = OP &   0xFFF;

     FILE         *file;
     uint16_t file_size;
     uint8_t        rnd;
     char     input[64];
     uint8_t    ands[8] = {128, 64, 32, 16, 8, 4, 2, 1};

     puts("FILE NAME:");
     gets(input);
     file = fopen(input, "rb");
     fseek(file, 0, SEEK_END);
     file_size = ftell(file);
     rewind(file);

     fread(C8->MEMORY + 512, 1, file_size, file);

     switch(OP){
          case 0x0000: if(C8->STACK[C8->SP] != 0)++(C8->SP);
                       C8->STACK[C8->SP] = C8->PC;
                       C8->PC = NNN;
                       break;
          case 0x00E0: for(uint8_t xAxis = 0; xAxis < 64; xAxis++){
                            for(uint8_t yAxis = 0; yAxis < 32; yAxis++)puts(" ");
                            puts("\n");
                       }
                       break;
          case 0x00EE: C8->PC = C8->STACK[C8->SP];
                       C8->STACK[C8->SP] = 0;
                       if(C8->STACK[C8->SP] > 0)--(C8->SP);
                       break;
          case 0x1000: C8->PC = NNN; break;
          case 0x2000: if(C8->STACK[C8->SP] != 0)++(C8->SP);
                       C8->STACK[C8->SP] = C8->PC;
                       C8->PC = NNN;
                       break;
          case 0x3000: if(C8->V[X] == NN)C8->PC += 2;
                       break;
          case 0x4000: if(C8->V[X] != NN)C8->PC += 2;
                       break;
          case 0x5000: if(C8->V[X] == C8->V[Y])C8->PC += 2;
                       break;
          case 0x6000: C8->V[X] = NN; break;
          case 0x7000: C8->V[X] += NN; break;
          case 0x8000: C8->V[Y] = C8->V[X]; break;
          case 0x8001: C8->V[X] = C8->V[X] | C8->V[Y]; break;
          case 0x8002: C8->V[X] = C8->V[X] & C8->V[Y]; break;
          case 0x8003: C8->V[X] = C8->V[X] ^ C8->V[Y]; break;
          case 0x8004: if(C8->V[X] + C8->V[Y] > 255)C8->V[0xF] = 1;
                       else C8->V[0xF] = 0;
                       C8->V[X] += C8->V[Y];
                       break;
          case 0x8005: if(C8->V[X] < C8->V[Y])C8->V[0xF] = 0;
                       else C8->V[0xF] = 1;
                       C8->V[X] -= C8->V[Y];
                       break;
          case 0x8006: if(C8->V[X] % 2 == 0)C8->V[0xF] = 0;
                       else C8->V[0xF] = 1;
                       C8->V[X] = C8->V[Y] >> 1;
                       break;
          case 0x8007: if(C8->V[Y] < C8->V[X])C8->V[0xF] = 0;
                       else C8->V[0xF] = 1;
                       C8->V[X] = C8->V[Y] - C8->V[X];
                       break;
          case 0x800E: if(C8->V[X] < 128)C8->V[0xF] = 0;
                       else C8->V[0xF] = 1;
                       C8->V[X] = C8->V[Y] << 1;
                       break;
          case 0x9000: if(C8->V[X] != C8->V[Y])C8->PC += 2;
                       break;
          case 0xA000: C8->I = NNN; break;
          case 0xB000: C8->PC = NNN + C8->V[0]; break;
          case 0xC000: C8->V[X] = (rand() % 255) & NN;
                       break;
          case 0xD000: C8->V[0xF] = 0;
                       for(uint8_t i = 0; i < N; i++){
                            for(uint8_t j = 0; j < 8; j++){
                                 if(C8->V[X] + j == 64)C8->V[X] = -j;
                                 if(C8->V[Y] + i == 64)C8->V[Y] = -i;
                                 if(C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] == 1 && ((C8->MEMORY[C8->I + i] & ands[j]) >> (8 - j - 1)) == 1)C8->V[0xF] = 1;
                                 C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] = C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] ^ ((C8->MEMORY[C8->I + 1] & ands[j]) >> (8 - j - 1));
                            }
                       }
                       break; // SET V[F] TO 01 IF ANY SET PIXELS ARE CHANGED OR UNSET AND 00 OTHERWISE
          case 0xE09E: if(C8->KEY_PRESS == C8->V[X])C8->PC += 2;
                       break;
          case 0xE0A1: if(C8->KEY_PRESS != C8->V[X])C8->PC += 2;
                       break;
          case 0xF007: C8->V[X] = C8->DT; break;
          case 0xF00A: C8->V[X] = C8->KEY_PRESS; break;
          case 0xF015: C8->DT = C8->V[X]; break;
          case 0xF018: break; // NO SOUND
          case 0xF01E: C8->I += C8->V[X]; break;
          case 0xF029: C8->I = C8->V[X] * 5; break;
          case 0xF033: C8->MEMORY[C8->I] = C8->V[X] / 100;
                       C8->MEMORY[C8->I + 1] = (C8->V[X] / 10) % 10;
                       C8->MEMORY[C8->I + 2] = C8->V[X] % 10;
                       break;
          case 0xF055: for(uint8_t i = 0; i < X + 1; i++)C8->MEMORY[C8->I + i] = C8->V[i];
                       C8->I = C8->I + X + 1;
                       break;
          case 0xF065: for(uint8_t i = 0; i < X + 1; i++)C8->V[i] += C8->MEMORY[C8->I + i];
                       C8->I = C8->I + X + 1;
                       break;
          default:     puts("INVALID"); break;
     }
}
