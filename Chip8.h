#define _WIN32_WINNT 0x0500
#include  <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include  <conio.h>
#include   <time.h>
#include<windows.h>

typedef struct{
     //uint8_t RESERVED_MEMORY[512];
     //uint8_t USABLE_MEMORY[4096 - 512];
     //uint8_t ETI_660_USABLE_MEMORY[4095 - 1536];
     uint8_t  MEMORY[ 4096 ];
     uint8_t           V[16]; // REGISTERS, V0 TO VF, VF IS A FLAG REGISTER
     uint16_t              I; // STORE MEMORY ADDRESS
     uint16_t             PC;
     uint8_t DISPLAY[64][32];
     uint8_t              DT; // DELAY TIMER
     uint16_t      STACK[16];
     uint16_t             SP;
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

void INIT(CHIP_8 *C8);
void CYCLE(CHIP_8 *C8);
void RENDER(CHIP_8 *C8, char *FG, char *BG);
void KEYBOARD(CHIP_8 *C8, uint8_t KEY, uint8_t PRESS_OR_RELEASE);
void DXYN(CHIP_8 *C8, uint8_t N, uint8_t X, uint8_t Y);
void DRAWB(CHIP_8 *C8, uint8_t L, uint8_t R, uint8_t Y, uint8_t SHL, uint8_t SHR, uint16_t *IDX, uint8_t ON);
