/*
 * Simulator of MK-54 programmable soviet calculator.
 * Based on sources of emu145 project: https://code.google.com/p/emu145/
 *
 * Copyright (C) 2013 Serge Vakulenko, <serge@vak.ru>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

#pragma once

#include <stdint.h>

#ifdef MK_54
#define CODE_NBYTES 98                  // Number of instructions in code memory
#define DATA_NREGS  14                  // Number of numeric registers
#else
#define CODE_NBYTES 105                 // Number of instructions in code memory
#define DATA_NREGS  15                  // Number of numeric registers
#endif

//
// Specialized PLM chips К145ИК130x.
//
#define REG_NWORDS  42                  // Number of words in data register

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t input;                     // Input word
    uint8_t output;                    // Output word
    uint8_t R [REG_NWORDS];             // R register
    uint8_t M [REG_NWORDS];             // M register
    uint8_t ST [REG_NWORDS];            // ST register
    uint8_t S;
    uint8_t Q;
    uint8_t carry;
    uint8_t keypad_event;
    uint32_t opcode;
    unsigned keyb_x;
    unsigned keyb_y;
    unsigned dot;
    uint32_t command;
    uint8_t enable_display;
    uint8_t show_dot [14];
    const uint32_t *inst_rom;           // Micro-instructions
    const uint32_t *cmd_rom;            // Instructions
    const uint8_t *prog_rom;            // Program
} plm_t;

//
// Initialize the PLM data structure.
//
void plm_init (plm_t *t, const uint32_t inst_rom[],
    const uint32_t cmd_rom[], const uint8_t prog_rom[]);

//
// Simulate one cycle of the PLM chip.
//
void plm_step (plm_t *t, unsigned cycle);


plm_t * get_ik1302();
uint32_t plm_get_cmd_rom(plm_t *t, uint16_t pc);

//
// FIFO serial memory chip К145ИР2.
//
#define FIFO_NWORDS (6*REG_NWORDS)      // Number of words in FIFO chip

typedef struct {
    uint8_t input;                     // Input word
    uint8_t output;                    // Output word
    uint16_t cycle;                     // Cycle counter
    uint8_t data [FIFO_NWORDS];   // FIFO memory
} fifo_t;

//
// Initialize the FIFO register data structure.
//
void fifo_init (fifo_t *t);

//
// Simulate one cycle of the FIFO chip.
//
void fifo_step (fifo_t *t);

//
// Initialize the calculator.
//
void calc_init (void);

//
// Simulate one cycle of the calculator.
// Return 0 when stopped, or 1 when running a user program.
// Call calc_keypad(), calc_rgd() and calc_display functions,
// supplied by user.
//
int calc_step (void);

//
// User function: show one digit on the LED display.
//
extern void calc_display (int i, int digit, int dot);

//
// User function: poll the radians/grads/degrees switch.
//
extern int calc_rgd(void);

#define MODE_RADIANS    10
#define MODE_DEGREES    11
#define MODE_GRADS      12

//
// User function: poll the keypad.
// Return the keycode value.
//
extern int calc_keypad(void);
                            //  Key Function
#define KEY_0       0x21    //  0   10^x    НОП
#define KEY_1       0x31    //  1   e^x
#define KEY_2       0x41    //  2   lg
#define KEY_3       0x51    //  3   ln
#define KEY_4       0x61    //  4   sin-1
#define KEY_5       0x71    //  5   cos-1
#define KEY_6       0x81    //  6   tg-1
#define KEY_7       0x91    //  7   sin
#define KEY_8       0xa1    //  8   cos
#define KEY_9       0xb1    //  9   tg
#define KEY_ADD     0x28    //  +   pi
#define KEY_SUB     0x38    //  -   sqrt
#define KEY_MUL     0x48    //  *   x^2
#define KEY_DIV     0x58    //  /   1/x
#define KEY_XY      0x68    //  xy  x^y
#define KEY_DOT     0x78    //  ,   O       a
#define KEY_NEG     0x88    //  /-/ АВТ     b
#define KEY_EXP     0x98    //  ВП  ПРГ     c
#define KEY_CLEAR   0xa8    //  Cx  CF      d
#define KEY_ENTER   0xb8    //  B^  Bx
#define KEY_STOPGO  0x29    //  C/П x!=0
#define KEY_GOTO    0x39    //  БП  L2
#define KEY_RET     0x49    //  B/O x>=0
#define KEY_CALL    0x59    //  ПП  L3
#define KEY_STORE   0x69    //  П   L1
#define KEY_NEXT    0x79    //  ШГ> x<0
#define KEY_LOAD    0x89    //  ИП  L0
#define KEY_PREV    0x99    //  <ШГ x=0
#define KEY_K       0xa9    //  K
#define KEY_F       0xb9    //  F

//
// Poll the USB port.
//
void calc_poll(void);

//
// Read the stack: X, Y, Z, T and X1 values.
// Each value contains 12 bcd digits stored as six bytes.
//
void calc_get_stack (unsigned char stack[][6]);

//
// Read the memory registers 0-9, A-D.
// Each value contains 12 bcd digits stored as six bytes.
//
void calc_get_regs (unsigned char reg[][6]);

//
// Read the program code.
//
void calc_get_code (unsigned char code[]);

//
// Update the program code.
//
void calc_write_code (unsigned char code[]);

//
// Microinstructions
//
#define UCMD_ALPHA_R        0x0000001UL   // alpha |= Ri
#define UCMD_ALPHA_M        0x0000002UL   // alpha |= Mi
#define UCMD_ALPHA_ST       0x0000004UL   // alpha |= STi
#define UCMD_ALPHA_NR       0x0000008UL   // alpha |= ~Ri
#define UCMD_ALPHA_C10      0x0000010UL   // if (carry == 0) alpha |= 10
#define UCMD_ALPHA_S        0x0000020UL   // alpha |= S
#define UCMD_ALPHA_4        0x0000040UL   // alpha |= 4

#define UCMD_BETA_S         0x0000080UL   // beta |= S
#define UCMD_BETA_NS        0x0000100UL   // beta |= ~S
#define UCMD_BETA_Q         0x0000200UL   // beta |= Q
#define UCMD_BETA_6         0x0000400UL   // beta |= 6
#define UCMD_BETA_1         0x0000800UL   // beta |= 1

#define UCMD_GAMMA_CARRY    0x0001000UL   // gamma |= carry
#define UCMD_GAMMA_NCARRY   0x0002000UL   // gamma |= !carry
#define UCMD_GAMMA_NKEY     0x0004000UL   // gamma |= !keypad_event

#define UCMD_R_MASK         0x0038000UL   // mask for R operation
#define UCMD_R_R3           0x0008000UL   // Ri := R[i+3]
#define UCMD_R_SUM          0x0010000UL   // Ri := sum
#define UCMD_R_S            0x0018000UL   // Ri := S
#define UCMD_R_RSSUM        0x0020000UL   // Ri |= S | sum
#define UCMD_R_SSUM         0x0028000UL   // Ri := S | sum
#define UCMD_R_RS           0x0030000UL   // Ri |= S
#define UCMD_R_RSUM         0x0038000UL   // Ri |= sum
#define UCMD_R1_SUM         0x0040000UL   // R[i-1] := sum
#define UCMD_R2_SUM         0x0080000UL   // R[i-2] := sum

#define UCMD_M_S            0x0100000UL   // Mi := S

#define UCMD_CARRY_SUM      0x0200000UL   // carry := carry bit of sum

#define UCMD_S_MASK         0x0c00000UL   // mask for S operation
#define UCMD_S_Q            0x0400000UL   // S := Q
#define UCMD_S_SUM          0x0800000UL   // S := sum
#define UCMD_S_QSUM         0x0c00000UL   // S := Q | sum

#define UCMD_Q_SUM          0x1000000UL   // Q := sum
#define UCMD_KEYPAD         0x2000000UL   // Q := poll_keypad()

#define UCMD_ST_SUM         0x4000000UL   // ST[i,+1,+2] := sum, STi, ST[i+1]
#define UCMD_ST_ROT         0x8000000UL   // ST[i,+1,+2] := ST[i+1], ST[i+2], STi

#ifdef __cplusplus
}
#endif