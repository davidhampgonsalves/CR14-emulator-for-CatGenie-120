#include "Arduino.h"
#include "Wire.h"

/*
 * Pins - Arduino Uno - A4 (SDA), A5 (SCL) other boards see https://www.arduino.cc/en/Reference/Wire
 *
 * CatGenie 120 wire colors. This is just a starting point as they like to swap the colors. If this doesn't work you will need to probe them on your device.
 *
 * White - SDA - pin A4
 * Black - SDL - pin A5
 * Red - Power(5v)
 * Blue - Ground
 *
 */

byte I2C_ADDRESS = 80; // 80(7 bits) with a R/W bit appended

byte* readRegister = new byte[4];
int readRegisterSize;
byte remaining = 0x78;

bool areBytesEqual(byte d1[], const byte d2[], int size) {
  for (int i = 0 ; i < size ; i++) {
    if(d1[i] != d2[i])
      return false;
  }
  return true;
}

void receiveEvent(int length) {
  // skip single byte write as its setting up the next read
  // and reading the ack breaks the following read
  if(length == 1) {
    // consume data but ignore
    Wire.read();
    return;
  }

  // 2 byte instructions are reset commands and can be ignored
  if(length < 2) {
    while(Wire.available()) {
      Wire.read();
    }
    return;
  }

  // 6 byte instructions contain the remaining usage count in byte 4
  if(length > 6) {
    for(int i = 0 ; Wire.available() ; i++) {
      if(i == 4)
        remaining = Wire.read();
      else
        Wire.read();
    }
    return;
  }

  // all other instructions set the read register for a subsiquent block read (handled by requestEvent)
  for(int i = 0 ; Wire.available() ; i++) {
    readRegister[i] = Wire.read();
  }
  readRegisterSize = length;
}

// Data that will be "read" from RFID tag by block id
const byte BLOCK_D[] = { 0x04, 0x1F, 0x7E, 0x30, 0x2A };
const byte BLOCK_5[] = { 0x04, 0x78, 0x00, 0x00, 0x00 };
const byte BLOCK_6[] = { 0x04, 0x78, 0x00, 0x00, 0x00 };
const byte BLOCK_0[] = { 0x04, 0x00, 0x00, 0x00, 0x00 };
const byte BLOCK_8[] = { 0x04, 0x01, 0x00, 0x78, 0x00 };
const byte BLOCK_9[] = { 0x04, 0x01, 0x00, 0x78, 0x00 };
const byte BLOCK_A[] = { 0x04, 0x01, 0x00, 0x78, 0x00 };
const byte BLOCK_B[] = { 0x04, 0x04, 0x0F, 0x00, 0x00 };
const byte BLOCK_C[] = { 0x04, 0x04, 0x0F, 0x00, 0x00 };
const byte BLOCK_F[] = { 0x04, 0x04, 0x0F, 0x2B, 0xFC };

const byte READ_SELECT_COMMAND[] = { 0x01, 0x02, 0x06, 0x00 };
const byte READ_NODE_ID[] = { 0x01, 0x02, 0x0E, 0x3C };
const byte READ_UID[] = { 0x01, 0x01, 0x0B };

byte NODE_ID_RESPONSE[] = { 0x01, 0x3C };
byte UID_RESPONSE[] = { 0x08, 0xA3, 0x6E, 0x6A, 0x73, 0x09, 0x33, 0x02, 0xD0 };

// WARNING - This needs to be pretty fast or we will miss data. Serial prints are too slow.
void requestEvent() {
  byte* response;
  int size = 4;
  bool isReadCommand = areBytesEqual(readRegister, READ_SELECT_COMMAND, readRegisterSize) 
  || areBytesEqual(readRegister, READ_NODE_ID, readRegisterSize);
  bool isUIDRequest = areBytesEqual(readRegister, READ_UID, readRegisterSize);
  if(isReadCommand) {
    response = NODE_ID_RESPONSE;
    size = 2;
  } 
  else if(isUIDRequest) {
    response = UID_RESPONSE;
    size = 9;
  } 
  else {
    // Return block requested by previous write to read register
    switch(readRegister[3]) {
    case 0x0D:
      response = const_cast<byte*>(BLOCK_D);
      break;
    case 0x05:
      // block 5/6 hold the remaining usage count
      response = const_cast<byte*>(BLOCK_5);
      response[1] = remaining;
    case 0x06:
      response = const_cast<byte*>(BLOCK_6);
      response[1] = remaining;
      break;
    case 0x00:
      response = const_cast<byte*>(BLOCK_0);
      break;
    case 0x08:
      response = const_cast<byte*>(BLOCK_8);
      break;
    case 0x09:
      response = const_cast<byte*>(BLOCK_9);
      break;
    case 0x0A:
      response = const_cast<byte*>(BLOCK_A);
      break;
    case 0x0B:
      response = const_cast<byte*>(BLOCK_B);
      break;
    case 0x0C:
      response = const_cast<byte*>(BLOCK_C);
      break;
    case 0x0F:
      response = const_cast<byte*>(BLOCK_F);
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
}

void loop() {
  delay(100);
}
