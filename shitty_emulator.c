#include  "Chip8.h"

void INIT(CHIP_8 *C8){
     C8->PC = 512;
     C8->I  =   0;
     C8->SP =   0;

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
     if(C8->DT > 0)--(C8->DT);
     uint8_t ands[8] = {128, 64, 32, 16, 8, 4, 2, 1};
     uint8_t  HI  = C8->MEMORY[C8->PC++];
     uint8_t  LO  = C8->MEMORY[C8->PC++];
     uint16_t OP  = HI << 8 | LO;
     uint8_t  P   = HI >>    0x4;
     uint8_t  X   = HI &     0xF;
     uint8_t  Y   = LO >>    0x4;
     uint8_t  N   = LO &     0xF;
     uint8_t  NN  =           LO;
     uint16_t NNN = OP &   0xFFF;
     uint8_t xAxis;
     uint8_t yAxis;/*
     printf("OP %04x\n",         OP);
     printf("PC %04x\n", C8->PC - 2);*/
     switch(OP & 0xF000){
          case 0x0000:
               switch (OP & 0xFFF){
                    case 0x00E0: memset(C8->DISPLAY, 0, 64*32);
                                 break;
                    case 0x00EE: C8->PC = C8->STACK[--C8->SP]; break;
                    default:     printf("NO INSTRUCTION %04x", OP); getch(); break;
               }
               break;
          case 0x1000: C8->PC = NNN; break;
          case 0x2000: C8->STACK[C8->SP++] = C8->PC;
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
               default:  printf("NO INSTRUCTION %04x", OP); getch(); break;
          }
          break;
          case 0x9000: if(C8->V[X] != C8->V[Y])C8->PC += 2;
                       break;
          case 0xA000: C8->I = NNN; break;
          case 0xB000: C8->PC = NNN + C8->V[0]; break;
          case 0xC000: C8->V[X] = (rand() % 255) & NN;
                       break;
          case 0xD000: DXYN(C8, N, X, Y);/*C8->V[0xF] = 0;
                       for(uint8_t i = 0; i < N; i++){
                            for(uint8_t j = 0; j < 8; j++){
                                 if(C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] == 1 && ((C8->MEMORY[C8->I + i] & ands[j]) >> (8 - j - 1)) == 1)C8->V[0xF] = 1;
                                 C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] = C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] ^ ((C8->MEMORY[C8->I + i] & ands[j]) >> (8 - j - 1));
                            }
                       }*/
                       break;
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
                    case 0x55: /*for(uint8_t i = 0; i < X + 1; i++)C8->MEMORY[C8->I + i] = C8->V[i];
                               C8->I = C8->I + X + 1;*/
                               break;
                    case 0x65: /*for(uint8_t i = 0; i < X + 1; i++)C8->V[i] += C8->MEMORY[C8->I + i];
                               C8->I = C8->I + X + 1;*/
                               break;
                    default:   printf("NO INSTRUCTION %04x", OP); getch(); break;
                  }
                  break;
          default: printf("NO INSTRUCTION %04x", OP); getch(); break;
     }
}
void RENDER(CHIP_8 *C8, char *FG, char *BG){

     for(uint8_t i = 0; i < 32; i++)
          for(uint8_t j = 0; j < 64; j++)printf("%s", C8->DISPLAY[j][i] ? FG : BG);
}
void KEYBOARD(CHIP_8 *C8, uint8_t KEY, uint8_t PRESS_OR_RELEASE){
     if(PRESS_OR_RELEASE == 1){
          switch(KEY){
               case 'x' : C8->KEY_PRESS = 0x0; break;
               case '1' : C8->KEY_PRESS = 0x1; break;
               case '2' : C8->KEY_PRESS = 0x2; break;
               case '3' : C8->KEY_PRESS = 0x3; break;
               case 'q' : C8->KEY_PRESS = 0x4; break;
               case 'w' : C8->KEY_PRESS = 0x5; break;
               case 'e' : C8->KEY_PRESS = 0x6; break;
               case 'a' : C8->KEY_PRESS = 0x7; break;
               case 's' : C8->KEY_PRESS = 0x8; break;
               case 'd' : C8->KEY_PRESS = 0x9; break;
               case 'z' : C8->KEY_PRESS = 0xA; break;
               case 'c' : C8->KEY_PRESS = 0xB; break;
               case '4' : C8->KEY_PRESS = 0xC; break;
               case 'r' : C8->KEY_PRESS = 0xD; break;
               case 'f' : C8->KEY_PRESS = 0xE; break;
               case 'v' : C8->KEY_PRESS = 0xF; break;
               case 27  : exit(0);
          }
     }

     if(PRESS_OR_RELEASE == 0)C8->KEY_PRESS = 0xFF;
}

void DELAY(int MS){
    clock_t START = clock();
    while(clock()<START + MS);
}
void DXYN(CHIP_8 *C8, uint8_t N, uint8_t X, uint8_t Y){
    uint8_t CLIPSPRITE = 69;
    uint8_t WIDE = N == 0 ? 1 : 0;
    if(WIDE)N = 0x10;
     C8->V[15] = 0;

    uint8_t  VX  = C8->V[X] &  63;
    uint8_t  VY  = C8->V[Y] &  31;
    uint8_t  SHR =       VX &   7;
    uint8_t  SHL =        8 - SHR;
    uint16_t IDX =       C8 ->  I;
    uint8_t  X0  =       VX >>  3;
    uint8_t  X1  =       X0 +   1;
    uint8_t  X2  =       X0 +   2;

    if(!CLIPSPRITE){
        X1 &= 7;
        X2 &= 7;
    }

    for(uint8_t Y = VY; Y < VY + N; ++Y){
        if(!CLIPSPRITE)Y &= 63;
        else if(Y >= 32)break;

        DRAWB(C8, X0, X1, Y, SHL, SHR, &IDX,    1);
        DRAWB(C8, X1, X2, Y, SHL, SHR, &IDX, WIDE);
    }
}

void DRAWB(CHIP_8 *C8, uint8_t L, uint8_t R, uint8_t Y, uint8_t SHL, uint8_t SHR, uint16_t *IDX, uint8_t ON){
    if(!ON)return;
      printf("Xue Hua Piao Piao\nIDX: %4x -> ", *IDX - 512);
    uint8_t DATA = C8->MEMORY[(*IDX)++];
      printf("%4x : %2x (RAM: %2x)", *IDX - 512, DATA, C8->MEMORY[*IDX -1]);
    if(L < 8){
        if(!C8->V[15])C8->V[15] = (C8->DISPLAY[Y][L] & DATA >> SHR) != 0;
        C8->DISPLAY[Y][L]       = (C8->DISPLAY[Y][L] ^ DATA >> SHR);
    }
    if(SHR && R < 8){
        if(!C8->V[15])C8->V[15] = (C8->DISPLAY[Y][R] & DATA << SHL) != 0;
        C8->DISPLAY[Y][L]       = (C8->DISPLAY[Y][R] ^ DATA << SHL);
    }
}
int main(){
     system("chcp 65001");
     CHIP_8 C8;
     INIT(&C8);
     while(1){
          KEYBOARD(&C8, C8.KEY_PRESS, 1);
          for(uint8_t penis_size = 0; penis_size < 10; penis_size++)CYCLE(&C8);
          DELAY(16);
          KEYBOARD(&C8, C8.KEY_PRESS, 0);
          RENDER(&C8, " ", "█");
     }
}


/* MEME ARCHIVES
printf("GETS PC: %d\n", C8->PC);
printf("FOPEN PC: %d\n", C8->PC);
printf("FSEEK PC: %d\n", C8->PC);
printf("REWIND PC: %d\n", C8->PC);
printf(" FREAD PC: %d\n", C8->PC);
printf("FILE SIZE %d MEM %d %d", file_size, C8->MEMORY[C8->PC], C8->MEMORY[C8->PC + 1]); // <- BYTES
getch();
printf("PC %d\n", C8->PC);
printf("PC: %d\n", C8->PC);
printf("HI: %d\n", HI);
printf("LO: %d\n", LO);
getch();
*/
