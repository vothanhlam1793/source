/*
 *    LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 * 
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 * 
 *    This permission notice shall be included in all copies or 
 *    substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */


#include "LedControl.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15


const byte words[][32] = {
  /*
  {0x00,0x00,0x00,0x00,0x38,0x00,0x44,0x00,0x44,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x44,0x00,0x44,0x00,0x38,0x00,0x00,0x00,0x00,0x00 }, // 0
  {0x00,0x00,0x00,0x00,0x10,0x00,0x30,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x7c,0x00,0x00,0x00,0x00,0x00 }, // 1
  {0x00,0x00,0x00,0x00,0x78,0x00,0x84,0x00,0x82,0x00,0x02,0x00,0x02,0x00,0x04,0x00,0x08,0x00,0x10,0x00,0x20,0x00,0x40,0x00,0x82,0x00,0xfe,0x00,0x00,0x00,0x00,0x00 }, // 2
  {0x00,0x00,0x00,0x00,0x38,0x00,0x44,0x00,0x04,0x00,0x04,0x00,0x08,0x00,0x38,0x00,0x04,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x84,0x00,0x78,0x00,0x00,0x00,0x00,0x00 }, // 3
  {0x00,0x00,0x00,0x00,0x08,0x00,0x18,0x00,0x18,0x00,0x28,0x00,0x28,0x00,0x48,0x00,0x48,0x00,0x88,0x00,0xfe,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x00,0x00,0x00,0x00 }, // 4
  {0x00,0x00,0x00,0x00,0x7e,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x78,0x00,0x04,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x84,0x00,0x78,0x00,0x00,0x00,0x00,0x00 }, // 5
  {0x00,0x00,0x00,0x00,0x1c,0x00,0x20,0x00,0x40,0x00,0x80,0x00,0xb8,0x00,0xc4,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x44,0x00,0x38,0x00,0x00,0x00,0x00,0x00 }, // 6
  {0x00,0x00,0x00,0x00,0xfe,0x00,0x82,0x00,0x82,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x00,0x00,0x00,0x00 }, // 7
  {0x00,0x00,0x00,0x00,0x38,0x00,0x44,0x00,0x82,0x00,0x82,0x00,0x44,0x00,0x38,0x00,0x44,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x44,0x00,0x38,0x00,0x00,0x00,0x00,0x00 }, // 8
  {0x00,0x00,0x00,0x00,0x38,0x00,0x44,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x46,0x00,0x3a,0x00,0x02,0x00,0x04,0x00,0x08,0x00,0x70,0x00,0x00,0x00,0x00,0x00 }, // 9

  {0x00,0x00,0x0c,0x00,0x1e,0x00,0x0c,0x00,0x06,0x00,0x07,0x80,0x07,0x40,0x0b,0x20,0x13,0x20,0x03,0x80,0x02,0x80,0x04,0x70,0x04,0x08,0x04,0x10,0x18,0x00,0x00,0x00}, // Icon 1 walk
  {0x00,0x00,0x0c,0x00,0x1e,0x00,0x0c,0x00,0x06,0x00,0x07,0x00,0x06,0x80,0x0f,0x40,0x13,0x40,0x03,0x80,0x02,0x80,0x04,0x60,0x08,0x20,0x06,0x60,0x02,0x00,0x0e,0x00}, // Icon 2
  {0x00,0x00,0x0c,0x00,0x1e,0x00,0x0c,0x00,0x06,0x00,0x07,0x00,0x07,0x80,0x07,0x40,0x03,0x40,0x03,0x80,0x02,0x80,0x04,0x40,0x04,0x20,0x04,0x10,0x1c,0x0c,0x00,0x04}, // Icon 3
  {0x00,0x00,0x0c,0x00,0x1e,0x00,0x0c,0x00,0x06,0x00,0x07,0x80,0x07,0x40,0x0b,0x40,0x13,0x00,0x03,0x80,0x02,0x80,0x04,0x60,0x04,0x18,0x04,0x08,0x18,0x10,0x00,0x00}, // Icon 4
  {0x00,0x00,0x0c,0x00,0x1e,0x00,0x0c,0x00,0x06,0x00,0x07,0x00,0x07,0x00,0x07,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x03,0x00}, // Icon 5
  {0x00,0x00,0x0c,0x00,0x1e,0x00,0x0c,0x00,0x06,0x00,0x07,0x00,0x06,0x80,0x0f,0x40,0x13,0x40,0x03,0x80,0x02,0x80,0x04,0x40,0x08,0x20,0x06,0x10,0x02,0x30,0x06,0x00}, // Icon 6
  {0x00,0x00,0x0c,0x00,0x1e,0x00,0x0c,0x00,0x06,0x00,0x07,0x80,0x07,0x40,0x0b,0x20,0x13,0x20,0x03,0x80,0x02,0x80,0x04,0x70,0x04,0x08,0x04,0x10,0x18,0x00,0x00,0x00}, // Icon 7
  {0x03,0x80,0x03,0x80,0x03,0x80,0x02,0x80,0x07,0xc0,0x0f,0xe0,0x1f,0xf0,0x1b,0xb0,0x1b,0xb0,0x1b,0xb0,0x17,0xd0,0x06,0xc0,0x06,0xc0,0x06,0xc0,0x0e,0xe0,0x1e,0xf0},  // Icon 8 stop
  */
 
  // 歡迎光臨點矩陣字幕器展示測試！
  {0x00,0x00,0x0a,0x18,0x7b,0xd0,0x0a,0x10,0x3b,0xbf,0x2a,0xa1,0x3b,0xca,0x14,0x08,0x1f,0x88,0x32,0x08,0x5f,0x88,0x12,0x14,0x1f,0x94,0x12,0x22,0x1f,0xa2,0x10,0x41 },
  {0x00,0x00,0x40,0x52,0x31,0x9f,0x11,0x12,0x01,0x12,0x79,0x12,0x09,0x12,0x11,0x12,0x21,0x12,0x79,0x52,0x09,0x9e,0x09,0x14,0x10,0x10,0x18,0x10,0x66,0x03,0x41,0xfc },
  {0x00,0x00,0x00,0xc0,0x00,0x80,0x20,0x8c,0x18,0x98,0x0c,0x90,0x04,0xa0,0x00,0x82,0x7f,0xff,0x04,0x40,0x04,0x40,0x04,0x40,0x08,0x42,0x08,0x42,0x10,0x43,0x60,0x3e },
  {0x00,0x00,0x48,0x60,0x7e,0x42,0x50,0xff,0x50,0x80,0x55,0x7c,0x7e,0x44,0x44,0x44,0x44,0x7c,0x7c,0x00,0x55,0xef,0x51,0x29,0x51,0x29,0x7f,0x29,0x41,0xef,0x01,0x29 }
  /*{0x00,0x00,0x41,0x18,0x7f,0x90,0x69,0x12,0x5b,0x1f,0x4d,0x10,0x7f,0x10,0x08,0x10,0x7f,0x52,0x08,0x7f,0x08,0x42,0x7f,0x42,0x00,0x42,0x2a,0x42,0x55,0x7e,0x55,0x42 },
  {0x00,0x00,0x30,0x02,0x21,0xff,0x24,0x80,0x3e,0x80,0x48,0x84,0x48,0xfe,0x0a,0x84,0x7f,0x84,0x08,0x84,0x14,0xfc,0x14,0x84,0x22,0x80,0x22,0x80,0x40,0x82,0x01,0xff },
  {0x00,0x00,0x48,0x60,0x7c,0x44,0x4b,0xfe,0x48,0x40,0x53,0xfc,0x52,0x44,0x4b,0xfc,0x4a,0x44,0x4a,0x44,0x4b,0xfc,0x7a,0x44,0x50,0x42,0x47,0xff,0x40,0x40,0x40,0x40 },
  {0x00,0x00,0x01,0x00,0x20,0x82,0x3f,0xff,0x20,0x02,0x40,0x24,0x0f,0xf0,0x00,0x20,0x00,0xc0,0x00,0x82,0x7f,0xff,0x00,0x80,0x00,0x80,0x04,0x80,0x03,0x80,0x01,0x00 },
  {0x00,0x00,0x08,0x0a,0x7f,0x7f,0x04,0x10,0x0f,0xf8,0x08,0x08,0x0f,0xf8,0x08,0x08,0x0f,0xf8,0x02,0x02,0x7f,0xff,0x08,0x88,0x1f,0xfc,0x68,0x8b,0x08,0x98,0x00,0x80 },
  {0x00,0x00,0x22,0x44,0x3f,0x7e,0x22,0x44,0x3e,0x7c,0x22,0x44,0x01,0x90,0x01,0x0a,0x7f,0xff,0x04,0x10,0x18,0x0c,0x7e,0x7f,0x22,0x44,0x22,0x44,0x3e,0x7c,0x22,0x44 },
  {0x00,0x00,0x20,0x04,0x3f,0xfe,0x20,0x04,0x3f,0xfc,0x20,0x04,0x22,0x10,0x2f,0xfc,0x22,0x10,0x22,0x12,0x3f,0xff,0x24,0x00,0x24,0x2c,0x45,0x90,0x47,0x0c,0x44,0x03 },
  {0x00,0x00,0x00,0x08,0x1f,0xfc,0x00,0x00,0x00,0x00,0x00,0x02,0x7f,0xff,0x00,0x80,0x00,0x80,0x18,0x90,0x10,0x88,0x20,0x84,0x20,0x86,0x40,0x82,0x00,0x80,0x00,0x80 },
  {0x00,0x00,0x24,0x43,0x17,0xe2,0x14,0x5a,0x04,0x52,0x47,0xd2,0x24,0x52,0x24,0x52,0x07,0xd2,0x04,0x52,0x14,0x52,0x17,0xd2,0x24,0x42,0x22,0x82,0x44,0x6e,0x58,0x24 },
  {0x00,0x00,0x20,0x1a,0x18,0x11,0x08,0x11,0x7f,0x10,0x00,0xff,0x3e,0x08,0x00,0x08,0x3e,0xe8,0x00,0x48,0x22,0x49,0x3f,0x49,0x22,0x45,0x22,0xf5,0x3e,0x43,0x22,0x01 },
  {0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00 }
  */
};

LedControl::LedControl(int dataPin, int clkPin, int csPin, int numDevices) {
    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    if(numDevices<=0 || numDevices>8 )
        numDevices=8;
    maxDevices=numDevices;
    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
    pinMode(SPI_CS,OUTPUT);
    digitalWrite(SPI_CS,HIGH);
    SPI_MOSI=dataPin;
    for(int i=0;i<64;i++) 
        status[i]=0x00;
    for(int i=0;i<maxDevices;i++) {
        spiTransfer(i,OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        setScanLimit(i,7);
        //decode is done in source
        spiTransfer(i,OP_DECODEMODE,0);
        clearDisplay(i);
        //we go into shutdown-mode on startup
        shutdown(i,true);
    }
}

const byte* LedControl::GetCharArray(char ch)
{
  if(ch >= ' ' && ch <= '~')
    return words[(int)(ch - ' ')];

  return words[31]; //unknown?
}

const byte* LedControl::GetArrayByIndex(int index)
{
  return words[index];
}

int LedControl::getDeviceCount() {
    return maxDevices;
}

void LedControl::shutdown(int addr, bool b) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(b)
        spiTransfer(addr, OP_SHUTDOWN,0);
    else
        spiTransfer(addr, OP_SHUTDOWN,1);
}

void LedControl::setScanLimit(int addr, int limit) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(limit>=0 && limit<8)
        spiTransfer(addr, OP_SCANLIMIT,limit);
}

void LedControl::setIntensity(int addr, int intensity) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(intensity>=0 && intensity<16)	
        spiTransfer(addr, OP_INTENSITY,intensity);
}

void LedControl::clearDisplay(int addr) {
    int offset;

    if(addr<0 || addr>=maxDevices)
        return;
    offset=addr*8;
    for(int i=0;i<8;i++) {
        status[offset+i]=0;
        spiTransfer(addr, i+1,status[offset+i]);
    }
}

void LedControl::setLed(int addr, int row, int column, boolean state) {
    int offset;
    byte val=0x00;

    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7 || column<0 || column>7)
        return;
    offset=addr*8;
    val=B10000000 >> column;
    if(state)
        status[offset+row]=status[offset+row]|val;
    else {
        val=~val;
        status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1,status[offset+row]);
}

void LedControl::setRow(int addr, int row, byte value) {
    int offset;
    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;
    offset=addr*8;
    status[offset+row]=value;
    spiTransferAll(addr, row+1,status[offset+row]);
}

void LedControl::resetStatus() {
    int maxbytes=maxDevices*2;
    for(int i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;
}

void LedControl::action() {
     //enable the line 
    digitalWrite(SPI_CS,LOW);
    //Now shift out the data 
    int maxbytes=maxDevices*2;
    for(int i=maxbytes;i>0;i--)
        shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
    //latch the data onto the display
    digitalWrite(SPI_CS,HIGH);
}

void LedControl::setColumn(int addr, int col, byte value) {
    byte val;

    if(addr<0 || addr>=maxDevices)
        return;
    if(col<0 || col>7) 
        return;
    for(int row=0;row<8;row++) {
        val=value >> (7-row);
        val=val & 0x01;
        setLed(addr,row,col,val);
    }
}
/*
void LedControl::setDigit(int addr, int digit, byte value, boolean dp) {
    int offset;
    byte v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7 || value>15)
        return;
    offset=addr*8;
    v=pgm_read_byte_near(charTable + value); 
    if(dp)
        v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}

void LedControl::setChar(int addr, int digit, char value, boolean dp) {
    int offset;
    byte index,v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7)
        return;
    offset=addr*8;
    index=(byte)value;
    if(index >127) {
        //no defined beyond index 127, so we use the space char
        index=32;
    }
    v=pgm_read_byte_near(charTable + index); 
    if(dp)
        v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}
*/
void LedControl::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
    //Create an array with the data to shift out
    int offset=addr*2;
    int maxbytes=maxDevices*2;

    for(int i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
    //enable the line 
    digitalWrite(SPI_CS,LOW);
    //Now shift out the data 
    for(int i=maxbytes;i>0;i--)
        shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
    //latch the data onto the display
    digitalWrite(SPI_CS,HIGH);
}

void LedControl::spiTransferAll(int addr, volatile byte opcode, volatile byte data) {
    //Create an array with the data to shift out
    int offset=addr*2;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
}  


