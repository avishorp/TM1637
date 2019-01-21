#include <Arduino.h>
#include "TM1637Display.h"

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
#define NUMBEROFDIGITS 6
// if you just have a booring 4 digit display w/o keypad,
// set NUMBEROFDIGITS 4 and uncomment the NOKEYPAD-define below 

// Uncomment the following, if you just want to test your Keypad
//#define JUSTTESTKEYPAD

// Uncomment the following, if you don't have any Keypad
//#define NOKEYPAD

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   500

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
  0,0
};

TM1637Display display(CLK, DIO, 2, NUMBEROFDIGITS);

void setup()
{
}

void loop()
{
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  uint8_t all[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  display.setBrightness(0x03);

  display.setSegments(blank);
#ifndef NOKEYPAD
  while (!display.lastKeyCode())
  {
    delay(100);
    display.getKeyCode();
  }
  display.showNumberDec(display.lastKeyCode(), false);
//  display.showNumberHexEx(display.lastKeyCode(), false);
  delay(500);
  display.clear();
#endif
#ifndef JUSTTESTKEYPAD
  int k;
  long i;
  uint8_t data[6];
  data[0] = display.encodeDigit(0);
  data[1] = display.encodeDigit(1);
  data[2] = display.encodeDigit(2);
  data[3] = display.encodeDigit(3);
  data[4] = display.encodeDigit(4);
  data[5] = display.encodeDigit(5);

#ifndef NOKEYPAD
  if (display.lastKeyCode()==1)
#endif
  {
   
    // All segments on
    display.setSegments(all);
    delay(TEST_DELAY);
    display.clear();
  }
  
  // Selectively set different digits

#ifndef NOKEYPAD
  if (display.lastKeyCode()==2)
#endif
  {
    display.setSegments(data);
    delay(TEST_DELAY);
    display.clear();
  }

#ifndef NOKEYPAD
  if (display.lastKeyCode()==3)
#endif
  {
    for(k = NUMBEROFDIGITS-1; k >= 0; k--) {
      display.setSegments(data, 1, k);
      delay(TEST_DELAY);
    }
    display.clear();
  }
  
#ifndef NOKEYPAD
  if (display.lastKeyCode()==4)
#endif
  {
    display.setSegments(data+2, 2, 2);
    delay(TEST_DELAY);
    display.clear();
  }
  
#ifndef NOKEYPAD
  if (display.lastKeyCode()==5)
#endif
  {
    display.setSegments(data+2, 2, 1);
    delay(TEST_DELAY);
    display.clear();
  }
  
#ifndef NOKEYPAD
  if (display.lastKeyCode()==6)
#endif
  {
    display.setSegments(data+1, 3, 1);
    delay(TEST_DELAY);
    display.clear();
  }

  // Show decimal numbers with/without leading zeros
#ifndef NOKEYPAD
  if (display.lastKeyCode()==7)
#endif
  {
    display.showNumberDec(0, false); // Expect: ___0
    delay(TEST_DELAY);
    display.showNumberDec(0, true);  // Expect: 0000
    delay(TEST_DELAY);
    display.clear();
  }
#ifndef NOKEYPAD
  if (display.lastKeyCode()==8)
#endif
  {
    display.showNumberDec(1, false); // Expect: ___1
    delay(TEST_DELAY);
    display.showNumberDec(1, true);  // Expect: 0001
    delay(TEST_DELAY);
    display.clear();
  }
#ifndef NOKEYPAD
  if (display.lastKeyCode()==9)
#endif
  {
    display.showNumberDec(301, false); // Expect: _301
    delay(TEST_DELAY);
    display.showNumberDec(301, true); // Expect: 0301
    delay(TEST_DELAY);
    display.clear();
  }
#ifndef NOKEYPAD
  if (display.lastKeyCode()==10)
#endif
  {
    display.showNumberDec(14, false, 2, 1); // Expect: _14_
    delay(TEST_DELAY);
    display.clear();
    display.showNumberDec(4, true, 2, 2);  // Expect: 04__
    delay(TEST_DELAY);
    display.clear();
  }
#ifndef NOKEYPAD
  if (display.lastKeyCode()==11)
#endif
  {
    display.showNumberDec(-1, false);  // Expect: __-1
    delay(TEST_DELAY);
    display.showNumberDec(-12);        // Expect: _-12
    delay(TEST_DELAY);
    display.showNumberDec(-999);       // Expect: -999
    delay(TEST_DELAY);
    display.clear();
    display.showNumberDec(-5, false, 3, 0); // Expect: _-5_
    delay(TEST_DELAY);
    display.clear();
  }
#ifndef NOKEYPAD
  if (display.lastKeyCode()==12)
#endif
  {
    display.showNumberHexEx(0xf1af);        // Expect: f1Af
    delay(TEST_DELAY);
    display.showNumberHexEx(0x2c);          // Expect: __2C
    delay(TEST_DELAY);
    display.showNumberHexEx(0xd1, 0, true); // Expect: 00d1
    delay(TEST_DELAY);
    display.clear();
    display.showNumberHexEx(0xd1, 0, true, 2); // Expect: d1__
    delay(TEST_DELAY);
    display.clear();
  }

#ifndef NOKEYPAD
  if (display.lastKeyCode()==13)
#endif
  {
    // Run through all the dots
    for(k=0; k <= NUMBEROFDIGITS; k++) {
      display.showNumberDecEx(0, (0x80 >> k), true);
      delay(TEST_DELAY);
    }
    display.clear();
  }

#ifndef NOKEYPAD
  if (display.lastKeyCode()==14)
#endif
  {
    // Brightness Test
//    for(k = 0; k < 8; k++) {
    for(k = 0; k < 4; k++) { // low power led just need up to 3
      display.setBrightness(k);
      display.setSegments(all);
      delay(TEST_DELAY);
    }
    display.clear();
  }

#ifndef NOKEYPAD
  if (display.lastKeyCode()==15)
#endif
  {
    // On/Off test
    for(k = 0; k < NUMBEROFDIGITS; k++) {
//      display.setBrightness(7, false);
      display.setBrightness(3, false);  // Turn off
      display.setSegments(all);
      delay(TEST_DELAY);
//      display.setBrightness(7, true);
      display.setBrightness(3, true); // Turn on
      display.setSegments(all);
      delay(TEST_DELAY);
    }
    display.clear();
  }

#ifndef NOKEYPAD
  if (display.lastKeyCode()==16)
#endif
  {
    long startno=999999;
    long stopno=-100000;
    int k = 6;

    while ((k--) - NUMBEROFDIGITS > 0)
    {
      startno /= 10;
      stopno /= 10;
    }
    
    for(i=startno;(i>stopno) 
#ifndef NOKEYPAD
      && (i > startno-1000 || display.lastKeyCode()==0)
#endif
      ;i--) {
      display.showNumberDec(i, false);
      // delay(1);
    }
    display.clear();

    // Done!
    display.setSegments(SEG_DONE);
    delay(2000);
    display.clear();
  }
  
  delay(100);
#endif
}
