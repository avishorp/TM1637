#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

TM1637Display display(CLK, DIO);

void setup()
{
  pinMode(13,OUTPUT);
}

void loop()
{
  uint8_t data[] = { 0xff, 0x06, 0x00, 0xff };
  
  display.setBrightness(0x0f);
  
  display.setSegments(data);
  while(1);
}
