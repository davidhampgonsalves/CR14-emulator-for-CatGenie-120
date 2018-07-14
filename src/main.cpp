#define SIZEOF(x)  (sizeof (x) / sizeof (x[0]))

#include "Arduino.h"
#include "Wire.h"

/*
 *
 * Pins - Arduino Uno A4 (SDA), A5 (SCL) other boards see https://www.arduino.cc/en/Reference/Wire
 *
 * White - SDA - pin A4
 * Blue - SDL - pin A5
 *
 */
byte I2C_ADDRESS = 80; // 80(7 bits) with a R/W bit appended
byte CLEAR_PARAMS[] = { 0x00, 0x00 };
byte ENABLE_CARRIER[] = { 0x00, 0x10 };

byte* readRegister = new byte[4];
int readRegisterSize;

bool eq(byte d1[], byte d2[], int size) {
  for (int i = 0 ; i < size ; i++) {
    if(d1[i] != d2[i])
      return false;
  }
  return true;
}

void receiveEvent(int length) {
  //skip single byte write as its setting up the next read
  //and reading the ack breaks the following read
  if(length == 1) {
    Wire.read();
    return;
  }

  if(length < 2) {
    // consume data but ignore
    while(Wire.available()) {
      Wire.read();
    }
    return;
  }

  for(int i = 0 ; Wire.available() ; i++) {
    readRegister[i] = Wire.read();
  }
  readRegisterSize = length;
}

const byte BLOCK_D[] = { 0x04, 0xF2, 0x81, 0xC7, 0x96 };
const byte BLOCK_5[] = { 0x04, 0x36, 0x00, 0x00, 0x00 };
const byte BLOCK_6[] = { 0x04, 0x36, 0x00, 0x00, 0x00 };
const byte BLOCK_0[] = { 0x04, 0x00, 0x00, 0x00, 0x00 };
const byte BLOCK_8[] = { 0x04, 0x01, 0x00, 0x78, 0x00 };
const byte BLOCK_9[] = { 0x04, 0x01, 0x00, 0x78, 0x00 };
const byte BLOCK_A[] = { 0x04, 0x01, 0x00, 0x78, 0x00 };
const byte BLOCK_B[] = { 0x04, 0x04, 0x0F, 0x00, 0x00 };
const byte BLOCK_C[] = { 0x04, 0x04, 0x0F, 0x00, 0x00 };
const byte BLOCK_F[] = { 0x04, 0x04, 0x0F, 0x7D, 0xF8 };

byte READ_SELECT_COMMAND[] = { 0x01, 0x02, 0x06, 0x00 };
byte READ_NODE_ID[] = { 0x01, 0x02, 0x0E, 0xD9 };
byte READ_UID[] = { 0x01, 0x01, 0x0B };

byte NODE_ID_RESPONSE[] = { 0x01, 0xD9 };
byte UID_RESPONSE[] = { 0x08, 0x61, 0x18, 0x03, 0x24, 0x01, 0x33, 0x02, 0xD0 };


int i = 0;
// This has to be really fast or we will miss data. Serial prints are too slow
void requestEvent() {
  byte* response;
  int size = 4;

  if(eq(readRegister, READ_SELECT_COMMAND, readRegisterSize) || eq(readRegister, READ_NODE_ID, readRegisterSize)) {
    response = NODE_ID_RESPONSE;
    size = SIZEOF(NODE_ID_RESPONSE);
  } else if(eq(readRegister, READ_UID, readRegisterSize)) {
    response = UID_RESPONSE;
    size = SIZEOF(UID_RESPONSE);
  } else {
    switch(readRegister[3]) {
      case 0x0D:
        response = BLOCK_D;
        break;
      case 0x05:
        response = BLOCK_5;
        break;
      case 0x06:
        response = BLOCK_6;
        break;
      case 0x00:
        response = BLOCK_0;
        break;
      case 0x08:
        response = BLOCK_8;
        break;
      case 0x09:
        response = BLOCK_9;
        break;
      case 0x0A:
        response = BLOCK_A;
        break;
      case 0x0B:
        response = BLOCK_B;
        break;
      case 0x0C:
        response = BLOCK_C;
        break;
      case 0x0F:
        response = BLOCK_F;
        break;
    }
    size = 5;
  }

  Wire.write(response, size);
}

void setup() {
  Wire.begin(I2C_ADDRESS);

  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  Serial.begin(9600);
}

void loop() {

}
