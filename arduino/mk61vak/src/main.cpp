#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#include <pico/multicore.h>
#else
#include <fakeserial.h>
#endif

#include "mk61vak/calc.h"
#include "compat.h"

#include <ilc2128l.h>
#include "termvfd.h"


/* 
SH1122 7pin SPI 256x64 grayscale OLED connections for Pi Pico

NOTE: this display is power hungry. Powering it from PiPico 3V3 rail 
may render rp2040 unstable. Prefer using external 3.3V LDO from VIN.

Display	|		PiPico Pin No. and name
  GND		|		GND
  VCC		|		VCC 	(3.3V, use separate LDO)
  SCL		|		18		PIN_SPI_SCK 
  SDA		|		19		PIN_SPI_MOSI
  RST		|		6	    PIN_RST
  DC		|		7		  PIN_DC
  CS		|		17 		PIN_SPI_SS
*/

#define PIN_DC 7
#define PIN_RST 6

// declare ILC2-12/8L instance on hardware SPI1
ILC2128L ilc(PIN_SPI_SS, PIN_DC, PIN_RST);

// serial console display
TermVFD termvfd;

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
      static int keycode = 0;
      static int hold = 0;

      if (Serial.available()) {
        int c = Serial.read();
        //printf("\nc=%d %x\n", c, c);
        keycode = ascii_to_mk(c);
        hold = 128; // make sure the key is held down long enough to be noticed
        //Serial.print("keycode="); Serial.println(keycode);
      }
      if (hold) {
        if (--hold == 0) 
          keycode = 0;
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

  /*
   * Show the next display symbol.
   * Index counter is in range 0..11.
   */

  // the regular cadence seems to be -1,-1,0,1,2,3,4,5,6,7,8,9,10,11, -1,-1, ....
  void calc_display_vfd(int i, int digit, int dot)
  {
      if (i == 0) {
          ilc.flip_buffers();
          multicore_fifo_push_blocking(1);
      }
      if (digit == -1) digit = 0x0f;
      if (dot == -1) dot = 0;
      if (i >= 0 && i <= 11) {
          ilc.set_digit(11 - i, digit, dot);
      }
  }  

  // used for displaying the indicator in terminal 
  void calc_display_term(int i, int digit, int dot)
  {
      if (i == 0) {
          termvfd.flip_buffers();
          termvfd.print_display();
      }
      if (digit == -1) digit = 0x0f;
      if (dot == -1) dot = 0;
      if (i >= 0 && i <= 11) {
          termvfd.set_digit(11 - i, digit, dot);
      }
  }

  void calc_display(int i, int digit, int dot)
  {
      calc_display_vfd(i, digit, dot);
  }

}  // extern "C"

void clear_display()
{
  for (int i = 0; i < 12; ++i) {
    termvfd.set_digit(i, ' ', 0);
    ilc.set_digit(i, ' ', 0);
  }
}


// just a heartbeat counter
int core1_counter = 0;

void loop_core1()
{
  for(;;) {
    ++core1_counter;
    uint32_t out;
    multicore_fifo_pop_timeout_us(10000, &out);
    ilc.refresh();
  }
}


void setup() {
  Serial.begin(115200);
  sleep_ms(250);
  Serial.println("mk-61 by sergev/fixelsan");

  // initialise fake VFD display
  ilc.begin(); 

  Serial.println("launching core1...");
  multicore_launch_core1(loop_core1);

  for (int i = 0; i < 12; ++i) {
    ilc.set_digit(i, i, 0);
  }
  ilc.flip_buffers();
  multicore_fifo_push_blocking(1);

  // initialise the calculator
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

void loop() {
  // put your main code here, to run repeatedly:
  calc_step();

  //Serial.println(core1_counter);
}

#ifndef ARDUINO
int main()
{
  setup();
  for(;;) loop(); 

  return 0;
}
#endif
