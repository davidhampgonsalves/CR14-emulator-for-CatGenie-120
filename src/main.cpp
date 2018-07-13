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
}

byte BLOCK_5[4] = { 0x00, 0x00, 0x00, 0x00 };
byte BLOCK_6[4] = { 0x00, 0x00, 0x00, 0x00 };
byte BLOCK_8[4] = { 0x00, 0x3C, 0x00, 0x01 };
byte BLOCK_9[4] = { 0x00, 0x3C, 0x00, 0x01 };
byte BLOCK_A[4] = { 0x00, 0x3C, 0x00, 0x01 };
byte BLOCK_B[4] = { 0x00, 0x00, 0x21, 0x08 };
byte BLOCK_C[4] = { 0x00, 0x00, 0x21, 0x08 };
byte BLOCK_D[4] = { 0xCB, 0xD8, 0x2A, 0x30 };
byte BLOCK_F[4] = { 0xDE, 0xE0, 0x21, 0x08 };

byte READ_SELECT_COMMAND[] = { 0x01, 0x02, 0x06, 0x00 };
byte READ_NODE_ID[] = { 0x01, 0x02, 0x0E, 0x45 };
byte READ_UID[] = { 0x01, 0x01, 0x0B };

byte NODE_ID_RESPONSE[] = { 0x01, 0x45 };
byte UID_RESPONSE[] = { 0xD0, 0x02, 0x0D, 0xE4, 0x3F, 0x56, 0x69, 0x67 };


// This has to be really fast or we will miss data. Serial prints are too slow
void requestEvent() {
  byte* response;
  int size = 4;

  if(eq(readRegister, READ_SELECT_COMMAND, sizeof(READ_SELECT_COMMAND)) || eq(readRegister, READ_NODE_ID, sizeof(READ_NODE_ID))) {
    response = NODE_ID_RESPONSE;
    size = sizeof(NODE_ID_RESPONSE);
  } else if(eq(readRegister, READ_UID, sizeof(READ_UID))) {
    response = UID_RESPONSE;
    size = sizeof(UID_RESPONSE);
  } else {
    switch(readRegister[3]) {
      case 0x05: response = BLOCK_5;
      case 0x06: response = BLOCK_6;
      case 0x08: response = BLOCK_8;
      case 0x09: response = BLOCK_9;
      case 0x0A: response = BLOCK_A;
      case 0x0B: response = BLOCK_B;
      case 0x0C: response = BLOCK_C;
      case 0x0D: response = BLOCK_D;
      case 0x0F: response = BLOCK_F;
      default: return;
    }
  }

  Wire.write(response, size);
}

void setup() {
  Wire.begin(I2C_ADDRESS);

  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop() {
}
