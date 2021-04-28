#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3

TM1637Display display(CLK, DIO);

unsigned long lastUpdatePoint{0};

void setup()
{
    display.setBrightness(0x0f);
}

void loop()
{
    unsigned int hour{16};
    unsigned int minute{45};
    display.showNumberDec(hour, false, 2, 0);
    display.showNumberDec(minute, true, 2, 2);
    displayColon(1000); // Triggered every seconds
}

void displayColon(const uint16_t &interval) {
    if (millis() > (lastUpdatePoint + interval)) {
        display.setColon(!display.getColon()); // Toggle the current state of the colon
        lastUpdatePoint = millis();
    }
}