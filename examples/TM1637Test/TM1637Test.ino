#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

TM1637Display display(CLK, DIO);

void setup()
{
}

void loop()
{
  int k;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  display.setBrightness(0x0f);
  
  // All segments on
  display.setSegments(data);
  delay(TEST_DELAY);
  
  // Selectively set different digits
  data[0] = 0b01001001;
  data[1] = display.encodeDigit(1);
  data[2] = display.encodeDigit(2);
  data[3] = display.encodeDigit(3);
  
  for(k = 3; k >= 0; k--) {
	display.setSegments(data, 1, k);
	delay(TEST_DELAY);
	}
	
  display.setSegments(data+2, 2, 2);
  delay(TEST_DELAY);
  
  display.setSegments(data+2, 2, 1);
  delay(TEST_DELAY);
  
  display.setSegments(data+1, 3, 1);
  delay(TEST_DELAY);
 
  
  // Convert decimal to segments
  // Without leading zero
  bool lz = false;
  for (uint8_t z = 0; z < 2; z++) {
	for(k = 0; k < 10000; k += k*4 + 7) {
		display.showNumberDec(k, lz);
		delay(TEST_DELAY);
	}
	lz = true;
  }
  while(1);
}
