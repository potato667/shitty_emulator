#include  "Chip8.h"

void INIT(CHIP_8 *C8){
     C8 = malloc(sizeof(CHIP_8));

     C8->PC =          512;
     C8->I  =            0;
     C8->SP = C8->STACK[0];
     memset(C8->MEMORY,  0,  4096);
     memset(C8->STACK,   0,    16);
     memset(C8->DISPLAY, 0, 64*32);
     memset(C8->V,       0,    16);
     memset(FONT,        0,    80);

     for(uint8_t i = 0; i < 80; i++)C8->MEMORY[i] = FONT[i];

     C8->DT        = 0;
     C8->KEY_PRESS = 0;

     FILE         *file;
     uint16_t file_size;
     char     input[64];

     puts("FILE NAME:");
     gets(input);
     file = fopen(input, "rb");
     fseek(file, 0, SEEK_END);
     file_size = ftell(file);
     rewind(file);

     fread(&C8->MEMORY[C8->PC], file_size, 1, file);
}

void CYCLE(CHIP_8  *C8){

     uint8_t ands[8] = {128, 64, 32, 16, 8, 4, 2, 1};

     if(C8->DT > 0)--(C8->DT);

     uint8_t  HI  = C8->MEMORY[C8->PC++];
     uint8_t  LO  = C8->MEMORY[C8->PC++];
     uint16_t OP  = HI << 8 | LO;
     uint8_t  P   = HI >>    0x4;
     uint8_t  X   = HI &     0xF;
     uint8_t  Y   = LO >>    0x4;
     uint8_t  N   = LO &     0xF;
     uint8_t  NN  =           LO;
     uint16_t NNN = OP &   0xFFF;

     switch(OP & 0xF000){
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
          case 0x8000:
          switch(OP & 0x000F){
               case 0x0: C8->V[Y] = C8->V[X]; break;
               case 0x1: C8->V[X] = C8->V[X] | C8->V[Y]; break;
               case 0x2: C8->V[X] = C8->V[X] & C8->V[Y]; break;
               case 0x3: C8->V[X] = C8->V[X] ^ C8->V[Y]; break;
               case 0x4: if(C8->V[X] + C8->V[Y] > 255)C8->V[0xF] = 1;
                         else C8->V[0xF] = 0;
                         C8->V[X] += C8->V[Y];
                         break;
               case 0x5: if(C8->V[X] < C8->V[Y])C8->V[0xF] = 0;
                         else C8->V[0xF] = 1;
                         C8->V[X] -= C8->V[Y];
                         break;
               case 0x6: if(C8->V[X] % 2 == 0)C8->V[0xF] = 0;
                         else C8->V[0xF] = 1;
                         C8->V[X] = C8->V[Y] >> 1;
                         break;
               case 0x7: if(C8->V[Y] < C8->V[X])C8->V[0xF] = 0;
                         else C8->V[0xF] = 1;
                         C8->V[X] = C8->V[Y] - C8->V[X];
                         break;
               case 0xE: if(C8->V[X] < 128)C8->V[0xF] = 0;
                         else C8->V[0xF] = 1;
                         C8->V[X] = C8->V[Y] << 1;
                         break;
               default:  puts("INVALID"); break;
          }
          case 0x9000: if(C8->V[X] != C8->V[Y])C8->PC += 2;
                       break;
          case 0xA000: C8->I = NNN; break;
          case 0xB000: C8->PC = NNN + C8->V[0]; break;
          case 0xC000: C8->V[X] = (rand() % 255) & NN;
                       break;
          case 0xD000: {
                       int xAxis = C8->V[X];
                       int yAxis = C8->V[Y];
                       C8->V[0xF] = 0;
                       for(uint8_t i = 0; i < N; i++){
                            for(uint8_t j = 0; j < 8; j++){
                                 if(C8->DISPLAY[xAxis + j][yAxis + i] == 1 && ((C8->MEMORY[C8->I + i] & ands[j]) >> (8 - j - 1)) == 1)C8->V[0xF] = 1;
                                 C8->DISPLAY[xAxis + j][yAxis + i] = C8->DISPLAY[xAxis + j][yAxis + i] ^ ((C8->MEMORY[C8->I + 1] & ands[j]) >> (8 - j - 1));
                           }
                           xAxis = C8->V[X];
                           yAxis = C8->V[Y];
                       }
                       break;
                       }
          case 0xE09E: if(C8->KEY_PRESS == C8->V[X])C8->PC += 2;
                       break;
          case 0xE0A1: if(C8->KEY_PRESS != C8->V[X])C8->PC += 2;
                       break;
          case 0xF000:
               switch(OP & 0x00FF){
                    case 0x07: C8->V[X] = C8->DT; break;
                    case 0x0A: C8->V[X] = C8->KEY_PRESS; break;
                    case 0x15: C8->DT = C8->V[X]; break;
                    case 0x18: break; // NO SOUND
                    case 0x1E: C8->I += C8->V[X]; break;
                    case 0x29: C8->I = C8->V[X] * 5; break;
                    case 0x33: C8->MEMORY[C8->I] = C8->V[X] / 100;
                               C8->MEMORY[C8->I + 1] = (C8->V[X] / 10) % 10;
                               C8->MEMORY[C8->I + 2] = C8->V[X] % 10;
                               break;
                    case 0x55: for(uint8_t i = 0; i < X + 1; i++)C8->MEMORY[C8->I + i] = C8->V[i];
                               C8->I = C8->I + X + 1;
                               break;
                    case 0x65: for(uint8_t i = 0; i < X + 1; i++)C8->V[i] += C8->MEMORY[C8->I + i];
                               C8->I = C8->I + X + 1;
                               break;
                    default:   puts("INVALID"); break;
                  }
          default: puts("INVALID"); break;
     }
}
void KEYBOARD(CHIP_8 *C8, uint8_t KEY){
     switch(KEY){
          case 'x' : C8->KEY_PRESS = 0x0;
          case '1' : C8->KEY_PRESS = 0x1;
          case '2' : C8->KEY_PRESS = 0x2;
          case '3' : C8->KEY_PRESS = 0x3;
          case 'q' : C8->KEY_PRESS = 0x4;
          case 'w' : C8->KEY_PRESS = 0x5;
          case 'e' : C8->KEY_PRESS = 0x6;
          case 'a' : C8->KEY_PRESS = 0x7;
          case 's' : C8->KEY_PRESS = 0x8;
          case 'd' : C8->KEY_PRESS = 0x9;
          case 'z' : C8->KEY_PRESS = 0xA;
          case 'c' : C8->KEY_PRESS = 0xB;
          case '4' : C8->KEY_PRESS = 0xC;
          case 'r' : C8->KEY_PRESS = 0xD;
          case 'f' : C8->KEY_PRESS = 0xE;
          case 'v' : C8->KEY_PRESS = 0xF;
          case 27  : exit(0);
     }
}
void KEY_RELEASE(CHIP_8 *C8){
     C8->KEY_PRESS = 0xFF;
}
int main(CHIP_8 *C8){
     INIT(C8);
     while(1){
          CYCLE(C8);
          for(uint8_t i = 0; i < 64; i++)for(uint8_t j = 0; j < 32; j++)if(C8->DISPLAY[i][j] == 1)putchar('#');
          KEYBOARD(C8, C8->KEY_PRESS);
          KEY_RELEASE(C8);
     }
}
