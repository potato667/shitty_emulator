#include  "Chip8.h"

void INIT(CHIP_8 *C8){
     printf("\e[48;5;235m");
     printf("\e[38;5;251m");

     system("chcp 65001");

     HANDLE INPUT;
     DWORD PREVIOUS;

     SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){128, 97}); // CHANGE BUFFER SIZE
     SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), 1, &(SMALL_RECT){0, 0, 128, 97}); // CHANGE CONSOLE SIZE
     EnableScrollBar(GetConsoleWindow(), SB_BOTH, ESB_DISABLE_BOTH); // DISABLE SCROLLBAR
     ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0); // HIDE SCROLLBARS
     SetWindowLong(GetConsoleWindow(), GWL_STYLE, GetWindowLong(GetConsoleWindow(), GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX & ~DS_MODALFRAME & ~WS_MINIMIZEBOX);  // DISABLE RESIZING AND REMOVE FULLSCREEN AND MINIMIZE BUTTONS
     GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &PREVIOUS);
     SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | (PREVIOUS & ~ENABLE_QUICK_EDIT_MODE)); // DISABLE MOUSE INPUT
     SetConsoleTitle("░░░░░░░░░░░░░▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓█████║║SHIT_8║║█████▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░"); // CONSOLE TITLE

}

void LOAD_ROM(CHIP_8 *C8){
     C8->PC = 512;
     C8->I  =   0;
     C8->SP =   0;
     C8->DT =   0;

     for(uint8_t i = 0; i < 80; i++)C8->MEMORY[i] = FONT[i];

     FILE         *ROM;
     uint16_t ROM_SIZE;
     char ROM_NAME[64];

     printf("\e[?25h"); // ENABLE TEXT CURSOR

     printf("ROM: ");
     gets(ROM_NAME);
     ROM = fopen(ROM_NAME, "rb");
     if(ROM == NULL){
          printf("UNABLE TO LOAD ROM\n");
          _getch();
          exit(-1);
     }
     fseek(ROM, 0, SEEK_END);
     ROM_SIZE = ftell(ROM);
     rewind(ROM);
     fread(&C8->MEMORY[C8->PC], ROM_SIZE, 1, ROM);

     printf("\e[?25l"); // DISABLE TEXT CURSOR
}

void CYCLE(CHIP_8 *C8, uint8_t IPF){
     for(uint8_t i = 0; i < IPF; i++){
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
          uint8_t yAxis;
          uint8_t tmp;

          /*
          switch(OP & 0xF000){
               case 0x0000:{
                    switch (OP & 0xFFF){
                         case 0x00E0: break;
                         case 0x00EE: break;
                         default: printf("NO INSTRUCTION %04x", OP); getch(); break;
                    }
               break;
               }
               case 0x1000: break;
               case 0x2000: break;
               case 0x3000: break;
               case 0x4000: break;
               case 0x5000: break;
               case 0x6000: break;
               case 0x7000: break;
               case 0x8000:{
                    switch(OP & 0x000F){
                         case 0x0: break;
                         case 0x1: break;
                         case 0x2: break;
                         case 0x3: break;
                         case 0x4: break;
                         case 0x5: break;
                         case 0x6: break;
                         case 0x7: break;
                         case 0xE: break;
                         default: printf("NO INSTRUCTION %04x", OP); getch(); break;
                    }
               break;
               }
               case 0x9000: break;
               case 0xA000: break;
               case 0xB000: break;
               case 0xC000: break;
               case 0xD000: break;
               case 0xE09E: break;
               case 0xE0A1: break;
               case 0xF000:{
                    switch(OP & 0x00FF){
                         case 0x07: break;
                         case 0x0A: break;
                         case 0x15: break;
                         case 0x18: break;
                         case 0x1E: break;
                         case 0x29: break;
                         case 0x33: break;
                         case 0x55: break;
                         case 0x65: break;
                         default: printf("NO INSTRUCTION %04x", OP); getch(); break;
                    }
               break;
               }
               default: printf("NO INSTRUCTION %04x", OP); getch(); break;
          }
          */
          switch(OP & 0xF000){
               case 0x0000:
                    switch (OP & 0xFFF){
                         case 0x00E0: memset(C8->DISPLAY, 0, 64*32);
                                      break;
                         case 0x00EE: C8->PC = C8->STACK[--C8->SP]; break;
                         default:     printf("IN CASE 0x0000 | NO INSTRUCTION %04x", OP); getch(); break;
                    }
                    break;
               case 0x1000: /*if(C8->PC - 2 == NNN)break;//exit(0);
                            else */C8->PC = NNN;
                            break;
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
                    case 0x0: C8->V[X] = C8->V[Y]; break;
                    case 0x1: C8->V[X] = C8->V[X] | C8->V[Y]; break;
                    case 0x2: C8->V[X] = C8->V[X] & C8->V[Y]; break;
                    case 0x3: C8->V[X] = C8->V[X] ^ C8->V[Y]; break;
                    case 0x4: if(C8->V[X] + C8->V[Y] > 255)tmp = 1;
                              else tmp = 0;
                              C8->V[X] += C8->V[Y];
                              C8->V[0xF] = tmp;
                              break;
                    case 0x5: if(C8->V[X] < C8->V[Y])tmp = 0;
                              else tmp = 1;
                              C8->V[X] -= C8->V[Y];
                              C8->V[0xF] = tmp;
                              break;
                    case 0x6: if(C8->V[X] % 2 == 0)tmp = 0;
                              else tmp = 1;
                              C8->V[X] = C8->V[Y] >> 1;
                              C8->V[0xF] = tmp;
                              break;
                    case 0x7: if(C8->V[Y] < C8->V[X])tmp = 0;
                              else tmp = 1;
                              C8->V[X] = C8->V[Y] - C8->V[X];
                              C8->V[0xF] = tmp;
                              break;
                    case 0xE: if(C8->V[X] < 128)tmp = 0;
                              else tmp = 1;
                              C8->V[X] = C8->V[Y] << 1;
                              C8->V[0xF] = tmp;
                              break;
                    default:  printf("IN CASE 0x8000 | NO INSTRUCTION %04x", OP); getch(); break;
               }
               break;
               case 0x9000: if(C8->V[X] != C8->V[Y])C8->PC += 2;
                            break;
               case 0xA000: C8->I = NNN; break;
               case 0xB000: C8->PC = NNN + C8->V[0]; break;
               case 0xC000: C8->V[X] = (rand() % 255) & NN;
                            break;
               case 0xD000: tmp = 0;
                            for(uint8_t i = 0; i < N; i++){
                                 for(uint8_t j = 0; j < 8; j++){
                                      if(C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] == 1 && ((C8->MEMORY[C8->I + i] & ands[j]) >> (8 - j - 1)) == 1)tmp = 1;
                                      C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] = C8->DISPLAY[C8->V[X] + j][C8->V[Y] + i] ^ ((C8->MEMORY[C8->I + i] & ands[j]) >> (8 - j - 1));
                                 }
                            }
                            C8->V[0xF] = tmp;
                            break;
               case 0xE09E: if(C8->KEY_PRESS[C8->V[X] & 0xF] == C8->V[X] & 0xF)C8->PC += 2;
                            break;
               case 0xE0A1: if(C8->KEY_PRESS[C8->V[X] & 0xF] != C8->V[X] & 0xF)C8->PC += 2;
                            break;
               case 0xF000:
                    switch(OP & 0x00FF){
                         case 0x07: C8->V[X] = C8->DT; break;
                         case 0x0A: if(C8->KEY_PRESS[C8->V[X] & 0xF] == 1)break;
                                    else C8->PC -= 2;
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
                         case 0x65: for(uint8_t i = 0; i < X + 1; i++)C8->V[i] = C8->MEMORY[C8->I + i];
                                    C8->I = C8->I + X + 1;
                                    break;
                         default:   printf("IN CASE 0xF000 | NO INSTRUCTION %04x", OP); getch(); break;
                       }
                       break;
               default: printf("NO INSTRUCTION %04x", OP); getch(); break;
          }
     }
}

void RENDER(CHIP_8 *C8, char *FG, char *BG){
     CONSOLE_SCREEN_BUFFER_INFO CONSOLE_INFO;
     CONSOLE_INFO.dwCursorPosition.X = 0;
     CONSOLE_INFO.dwCursorPosition.Y = 0;
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_INFO.dwCursorPosition);
     for(uint8_t i = 0; i < 32; i++)
         for(uint8_t j = 0; j < 64; j++)printf("%s", C8->DISPLAY[j][i] ? FG : BG);
}

void KEYBOARD(CHIP_8 *C8){
     if(_kbhit()){
          switch(_getch()){
               case 'x': C8->KEY_PRESS[0x0] = 1; break;
               case '1': C8->KEY_PRESS[0x1] = 1; break;
               case '2': C8->KEY_PRESS[0x2] = 1; break;
               case '3': C8->KEY_PRESS[0x3] = 1; break;
               case 'q': C8->KEY_PRESS[0x4] = 1; break;
               case 'w': C8->KEY_PRESS[0x5] = 1; break;
               case 'e': C8->KEY_PRESS[0x6] = 1; break;
               case 'a': C8->KEY_PRESS[0x7] = 1; break;
               case 's': C8->KEY_PRESS[0x8] = 1; break;
               case 'd': C8->KEY_PRESS[0x9] = 1; break;
               case 'z': C8->KEY_PRESS[0xA] = 1; break;
               case 'c': C8->KEY_PRESS[0xB] = 1; break;
               case '4': C8->KEY_PRESS[0xC] = 1; break;
               case 'r': C8->KEY_PRESS[0xD] = 1; break;
               case 'f': C8->KEY_PRESS[0xE] = 1; break;
               case 'v': C8->KEY_PRESS[0xF] = 1; break;
               default: break;
          }
     }
}

int main(){
     CHIP_8 C8;
     INIT(&C8);
     LOAD_ROM(&C8);
     while(1){
          KEYBOARD(&C8);
          CYCLE(&C8, 10);
          Sleep(16);
          RENDER(&C8, "██", "  ");
     }
}
