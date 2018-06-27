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

bool writePending = false;
byte recieved[10];
byte* readRegister;

bool eq(byte d1[], byte d2[]) {
  if(sizeof(d1) != sizeof(d2))
    return false;

  for (int i = 0 ; i < sizeof(d1) ; i++) {
    if(d1[i] != d2[i])
      return false;
  }

  return true;
}

void p(byte data[]) {
  Serial.print("[");
  for (int i = 0 ; i < sizeof(data) ; i++) {
    if(i > 0)
      Serial.print(", ");
    Serial.print(data[i], HEX);
  }
  Serial.print("]");
  Serial.print("(");
  Serial.print(sizeof(data));
  Serial.println(")");
}

void receiveEvent(int length) {
  digitalWrite(LED_BUILTIN, HIGH);

  byte data[length];
  Wire.readBytes(data, length);

  // THIS BREAKS THINGS
  Serial.print("RAW - recieved");
  Serial.print(data[0], HEX);
  Serial.println(data[1], HEX);
  //p(data);
  if(writePending) {
    writePending = false;
    memcpy(readRegister, data, length);

    Serial.print("setting read address to: ");
    p(data);
  } else if(data[0] == 0x01 && length > 1) {
    Serial.print("Write Pending, ");
    writePending = true;
  } else if(eq(CLEAR_PARAMS, data)) {
    writePending = false;
    Serial.println("Clear Params");
  } else if(eq(ENABLE_CARRIER, data))
    Serial.println("Carrier Enabled");
  else {
    Serial.print("UNHANDLED write: ");
    p(data);
  }
}

byte BLOCK_5[10] = { 0x00, 0x00, 0x00, 0x00 };
byte BLOCK_6[10] = { 0x00, 0x00, 0x00, 0x00 };
byte BLOCK_8[10] = { 0x00, 0x3C, 0x00, 0x01 };
byte BLOCK_9[10] = { 0x00, 0x3C, 0x00, 0x01 };
byte BLOCK_A[10] = { 0x00, 0x3C, 0x00, 0x01 };
byte BLOCK_B[10] = { 0x00, 0x00, 0x21, 0x08 };
byte BLOCK_C[10] = { 0x00, 0x00, 0x21, 0x08 };
byte BLOCK_D[10] = { 0xCB, 0xD8, 0x2A, 0x30 };
byte BLOCK_F[10] = { 0xDE, 0xE0, 0x21, 0x08 };

byte* getBlockByRegister(byte reg) {
  switch(reg) {
    case 0x05: return BLOCK_5;
    case 0x06: return BLOCK_6;
    case 0x08: return BLOCK_8;
    case 0x09: return BLOCK_9;
    case 0x0A: return BLOCK_A;
    case 0x0B: return BLOCK_B;
    case 0x0C: return BLOCK_C;
    case 0x0D: return BLOCK_D;
    case 0x0F: return BLOCK_F;
  }
  Serial.print("ERROR, block not found for ");
  Serial.print(reg);

  return BLOCK_5;
}

byte NODE_ID_RESPONSE[10] = { 0x91 };
byte UNHANDLED_RESPONSE[10] = { 0x00 };
byte UID_RESPONSE[10] = { 0xD0, 0x02, 0x0D, 0xE4, 0x3F, 0x56, 0x69, 0x67 };

byte READ_SELECT_COMMAND[] = { 0x06, 0x00 };
byte READ_NODE_ID[] = { 0x0E, 0x91 };
byte READ_UID[] = { 0x0B };

void requestEvent() {
  digitalWrite(LED_BUILTIN, HIGH);

  byte *response;
  if(eq(READ_SELECT_COMMAND, readRegister))
    response = NODE_ID_RESPONSE;
  if(eq(READ_NODE_ID, readRegister))
    response = NODE_ID_RESPONSE;
  if(eq(READ_UID, readRegister))
    response = UID_RESPONSE;
  if(readRegister[0] == 0x08) // read block
    response = getBlockByRegister(readRegister[1]);
  else {
    Serial.println(" UHANDED NACK");
    return;
  }

  Serial.print(" response: ");
  p(response);

  Wire.write(response, sizeof(response));
}

void setup() {

  Wire.begin(I2C_ADDRESS);

  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);
  Serial.println("Starting");

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  delay(100);
  //Serial.println("Waiting ..");
}


