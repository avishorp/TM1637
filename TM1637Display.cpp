
//  Author: avishorp@gmail.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include <TM1637Display.h>
#include <Arduino.h>

#define TM1637_I2C_COMM1    0x40
#define TM1637_I2C_COMM2    0xC0
#define TM1637_I2C_COMM3    0x80

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D
const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
  };

const uint8_t letterToSegment[] = {
 // XGFEDCBA
  0b01110111,    // A
  0b01111111,    // B
  0b00111001,    // C
  0b00011111,    // D
  0b01111001,    // E
  0b01110001,    // F
  0b00111101,    // G
  0b01110110,    // H
  0b00110000,    // I
  0b00001110,    // J
  0b01111010,    // K
  0b00111000,    // L
  0b00010101,    // M
  0b01110110,    // N
  0b00111111,    // O
  0b01110011,    // P
  0b00111011,    // Q
  0b01111011,    // R
  0b01101101,    // S
  0b00110001,    // T
  0b00111110,    // U
  0b00101110,    // V
  0b01101010,    // W
  0b01001001,    // X
  0b01101110,    // Y
  0b01011011,    // Z
  0b01001100,    // a
  0b01111100,    // b
  0b01011000,    // c
  0b01011110,    // d
  0b00011000,    // e
  0b01110001,    // f
  0b01011001,    // g
  0b00110100,    // h
  0b00010000,    // i
  0b00001110,    // j
  0b01101001,    // k
  0b00110000,    // l
  0b00010101,    // m
  0b01010100,    // n
  0b01011100,    // o
  0b01110011,    // p
  0b01100111,    // q
  0b01010000,    // r
  0b01001000,    // s
  0b01111000,    // t
  0b00011100,    // u
  0b00001100,    // v
  0b00101010,    // w
  0b01001000,    // x
  0b01101110,    // y
  0b01001000     // z
};

static const uint8_t minusSegments = 0b01000000;

TM1637Display::TM1637Display(uint8_t pinClk, uint8_t pinDIO, unsigned int bitDelay)
{
	// Copy the pin numbers
	m_pinClk = pinClk;
	m_pinDIO = pinDIO;
	m_bitDelay = bitDelay;

	// Set the pin direction and default value.
	// Both pins are set as inputs, allowing the pull-up resistors to pull them up
  pinMode(m_pinClk, INPUT);
  pinMode(m_pinDIO,INPUT);
	digitalWrite(m_pinClk, LOW);
	digitalWrite(m_pinDIO, LOW);
}

void TM1637Display::setBrightness(uint8_t brightness, bool on)
{
	m_brightness = (brightness & 0x7) | (on? 0x08 : 0x00);
}

void TM1637Display::setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
    // Write COMM1
	start();
	writeByte(TM1637_I2C_COMM1);
	stop();

	// Write COMM2 + first digit address
	start();
	writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

	// Write the data bytes
	for (uint8_t k=0; k < length; k++)
	  writeByte(segments[k]);

	stop();

	// Write COMM3 + brightness
	start();
	writeByte(TM1637_I2C_COMM3 + (m_brightness & 0x0f));
	stop();
}

void TM1637Display::clear()
{
  uint8_t data[] = { 0, 0, 0, 0 };
	setSegments(data);
}

void TM1637Display::showNumberDec(int num, bool leading_zero, uint8_t length, uint8_t pos)
{
  showNumberDecEx(num, 0, leading_zero, length, pos);
}

void TM1637Display::showNumberDecEx(int num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
  showNumberBaseEx(num < 0? -10 : 10, num < 0? -num : num, dots, leading_zero, length, pos);
}

void TM1637Display::showNumberHexEx(uint16_t num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
  showNumberBaseEx(16, num, dots, leading_zero, length, pos);
}

void TM1637Display::showNumberBaseEx(int8_t base, uint16_t num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
    bool negative = false;
	if (base < 0) {
	    base = -base;
		negative = true;
	}


    uint8_t digits[4];

	if (num == 0 && !leading_zero) {
		// Singular case - take care separately
		for(uint8_t i = 0; i < (length-1); i++)
			digits[i] = 0;
		digits[length-1] = encodeDigit(0);
	}
	else {
		//uint8_t i = length-1;
		//if (negative) {
		//	// Negative number, show the minus sign
		//    digits[i] = minusSegments;
		//	i--;
		//}
		
		for (int i = length-1; i >= 0; --i)
		{
		    uint8_t digit = num % base;
			
			if (digit == 0 && num == 0 && leading_zero == false)
			    // Leading zero is blank
				digits[i] = 0;
			else
			    digits[i] = encodeDigit(digit);
				
			if (digit == 0 && num == 0 && negative) {
			    digits[i] = minusSegments;
				negative = false;
			}

			num /= base;
		}
    }
	
	if (dots != 0)
	{
		showDots(dots, digits);
	}
    
    setSegments(digits, length, pos);
}

void TM1637Display::showLetter(char c, uint8_t pos)
{
  uint8_t seg[1] = {encodeLetter(c)};
  setSegments(seg, 1, pos);
}

void TM1637Display::showMessage(char msg[], uint8_t sleep)
{
  uint8_t seg[4] = {0, 0, 0, 0};
  for (uint8_t i = 0; i <= strlen(msg) + 3; i++)
  {
    if (i < strlen(msg))
      seg[3] = encodeLetter(msg[i]);
    else
      seg[3] = 0;

    setSegments(seg);
    delay(sleep);

    for (uint8_t j = 0; j < 3; j++)
      seg[j] = seg[j + 1];
  }
}

void TM1637Display::bitDelay()
{
	delayMicroseconds(m_bitDelay);
}

void TM1637Display::start()
{
  pinMode(m_pinDIO, OUTPUT);
  bitDelay();
}

void TM1637Display::stop()
{
	pinMode(m_pinDIO, OUTPUT);
	bitDelay();
	pinMode(m_pinClk, INPUT);
	bitDelay();
	pinMode(m_pinDIO, INPUT);
	bitDelay();
}

bool TM1637Display::writeByte(uint8_t b)
{
  uint8_t data = b;

  // 8 Data Bits
  for (uint8_t i = 0; i < 8; i++) {
    // CLK low
    pinMode(m_pinClk, OUTPUT);
    bitDelay();

	// Set data bit
    if (data & 0x01)
      pinMode(m_pinDIO, INPUT);
    else
      pinMode(m_pinDIO, OUTPUT);

    bitDelay();

	// CLK high
    pinMode(m_pinClk, INPUT);
    bitDelay();
    data = data >> 1;
  }

  // Wait for acknowledge
  // CLK to zero
  pinMode(m_pinClk, OUTPUT);
  pinMode(m_pinDIO, INPUT);
  bitDelay();

  // CLK to high
  pinMode(m_pinClk, INPUT);
  bitDelay();
  uint8_t ack = digitalRead(m_pinDIO);
  if (ack == 0)
    pinMode(m_pinDIO, OUTPUT);


  bitDelay();
  pinMode(m_pinClk, OUTPUT);
  bitDelay();

  return ack;
}

void TM1637Display::showDots(uint8_t dots, uint8_t* digits)
{
    for (int i = 0; i < 4; ++i)
    {
        digits[i] |= (dots & 0x80);
        dots <<= 1;
    }
}

uint8_t TM1637Display::encodeDigit(uint8_t digit)
{
  return digitToSegment[digit & 0x0f];
}

uint8_t TM1637Display::encodeLetter(char c)
{
  if (c >= 65 && c <= 90)
    return letterToSegment[c - 65];
  else if (c >= 97 && c <= 122)
    return letterToSegment[c - 71];
  else
    return 0;
}
