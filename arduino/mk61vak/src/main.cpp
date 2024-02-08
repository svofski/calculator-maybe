#ifdef ARDUINO
#include <Arduino.h>
#else
#include <fakeserial.h>
#endif

#include "mk61vak/calc.h"
#include "compat.h"

uint8_t display[12];
uint8_t dots[12];

void print_display(void);
void clear_display(void);

int ascii_to_mk(int c)
{
    switch (c) {
        case '7': return KEY_7;       //  7   sin
        case '4': return KEY_4;       //  4   sin-1
        case '1': return KEY_1;       //  1   e^x
        case '0': return KEY_0;       //  0   10^x
        case '.': return KEY_DOT;     //  ,   O
        case '~': return KEY_NEG;     //  /-/ АВТ
        case '^': return KEY_EXP;     //  ВП  ПРГ
        case 8:    return KEY_CLEAR;  //  Cx  CF
        case 'k':  return KEY_K;      //  K
        case 'i':  return KEY_LOAD;   //  ИП  L0
        case '8':  return KEY_8;      //  8   cos
        case '5':  return KEY_5;      //  5   cos-1
        case '2':  return KEY_2;      //  2   lg
        case '3':  return KEY_3;      //  3   ln
        case 's':  return KEY_XY;     //  xy  x^y
        case 13:   return KEY_ENTER;  //  B^  Bx
        case 'f':  return KEY_F;      //  F
        case 67:  
        case 75:   return KEY_NEXT;   //  ШГ> x<0      // arrow right (prefixed 91)
        case 68:
        case 77:   return KEY_PREV;   //  <ШГ x=0      // arrow left (prefixed 91)
        case 'p':  return KEY_STORE;  //  П   L1
        case '9':  return KEY_9;      //  9   tg
        case '6':  return KEY_6;      //  6   tg-1
        case '+':  return KEY_ADD;    //  +   pi
        case '*':  return KEY_MUL;    //  *   x^2
        case 72:
        case 71:   return KEY_RET;    //  B/O x>=0     // home (prefixed 91)
        case 'b':  return KEY_GOTO;   //  БП  L2
        case '-':  return KEY_SUB;    //  -   sqrt
        case '/':  return KEY_DIV;    //  /   1/x
        case 'c':  return KEY_CALL;   //  ПП  L3
        case ' ':  return KEY_STOPGO; //  C/П x!=0
    }
    return 0;
}

extern "C" {
  int calc_keypad(void) 
  {
      int keycode = 0;
      if (Serial.available()) {
        int c = Serial.read();
        //printf("\nc=%d %x\n", c, c);
        //Serial.println(); Serial.print('#'); Serial.println(c);
        keycode = ascii_to_mk(c);
        //Serial.print("keycode="); Serial.println(keycode);
      }
      return keycode;
  }

  void calc_poll(void)
  {
  }

  int calc_rgd(void)
  {
      //#define MODE_RADIANS    10
      //#define MODE_DEGREES    11
      //#define MODE_GRADS      12
      return MODE_RADIANS;
  }

  void clear_segments()
  {
      //for (int i = 0; i < 12; ++i) {
      //    display[i] = ' ';
      //    dots[i] = 0;
      //}
  }

  static const char segments[16] PROGMEM = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '-', 'L', 'C', 'r', 'E', ' '};

  /*
   * Show the next display symbol.
   * Index counter is in range 0..11.
   */

  void calc_display (int i, int digit, int dot)
  {
      static bool changed = false;

      if (digit == -1 && dot == -1) {
        //Serial.println("calc_display: clear");
        clear_display();
        print_display();
        changed = true;
      }
      //Serial.print("calc_display:"); Serial.print(i); Serial.print(' '); Serial.print(digit); Serial.print(' '); Serial.println(dot);
      //clear_segments();
      if (i >= 0) {
          uint8_t prev, fresh;
          if (digit >= 0) {
              prev = display[11 - i];
              fresh = pgm_read_byte(&segments[digit]);
              display[11 - i] = fresh;
              changed |= prev != fresh;
          }

          prev = dots[11 - i];
          fresh = dot ? ',' : ' ';
          dots[11 - i] = fresh;
          changed |= prev != fresh;
      }

      //if (i == -1 && changed) {
      if (i == 11 && changed) {
        print_display();
        changed = false;
      }
  }

}

void clear_display()
{
  for (int i = 0; i < 12; ++i) {
    display[i] = ' ';
    dots[i] = ' ';
  }
}


void print_display()
{
    Serial.print('[');
    for (int i = 0; i < 12; ++i) {
        Serial.print((char)display[i]);
        Serial.print((char)dots[i]);
    }
    Serial.print("]  ");

    Serial.print('\r');
}

void setup() {
  Serial.begin(115200);
  Serial.println("mk-61 by sergev/fixelsan");
  // put your setup code here, to run once:
  calc_init();

  #if 0
  Serial.println("ik1302 cmd_rom:");
  plm_t * ik1302 = get_ik1302();
  for (unsigned pc = 0; pc < 64; ++pc) {
    Serial.print(plm_get_cmd_rom(ik1302, pc), 16); Serial.print(',');
  }
  Serial.println();
  #endif
}

uint32_t frame = 0;

void loop() {
  // put your main code here, to run repeatedly:
  int running = calc_step();

  #if 0
  Serial.print("step: ");
  Serial.print(frame); 
  Serial.print(" running: ");
  Serial.println(running);
  #endif

  ++frame;
}

#ifndef ARDUINO
int main()
{
  setup();
  for(;;) loop(); 

  return 0;
}
#endif