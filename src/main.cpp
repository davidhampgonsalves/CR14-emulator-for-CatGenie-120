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
byte* readRegister = new byte[4];

bool eq(byte d1[], byte d2[]) {
  if(sizeof(d1) != sizeof(d2))
    return false;

  for (unsigned int i = 0 ; i < sizeof(d1) ; i++) {
    if(d1[i] != d2[i])
      return false;
  }

  return true;
}

void p(byte data[]) {
  Serial.print("[");
  for (unsigned int i = 0 ; i < 4 ; i++) {
    if(i > 0)
      Serial.print(", ");
    Serial.print(data[i], HEX);
  }
  Serial.print("]");
  Serial.print("(");
  Serial.print(sizeof(data));
  Serial.println(")");
}

void print() {
  Serial.print("IGNOING [");
  while(Wire.available()) {
    Serial.print(Wire.read(), HEX);
    Serial.print(", ");
  }
  Serial.println("]");
}

void receiveEvent(int length) {
  print();
  return;

  if(length < 3) {
    //Serial.println("IGNOING [");
    //while(Wire.available()) {
      //Serial.print(Wire.read(), HEX);
      //Serial.print(", ");
    //}
    //Serial.println("]");
    return;
  }

  //Serial.print("Filling read register [");
  for(int i = 0 ; Wire.available() ; i++) {
    readRegister[i] = Wire.read();
    //Serial.print("filling read register");
    //Serial.print(i);
    //Serial.print(" with ");
    //Serial.println(Wire.read(), HEX);
  }
  Serial.print("wrote read register: ");
  p(readRegister);
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
  Serial.println(reg, HEX);

  return BLOCK_5;
}

byte UNHANDLED_RESPONSE[10] = { 0x00 };

byte READ_SELECT_COMMAND[] = { 0x01, 0x02, 0x06, 0x00 };
byte READ_NODE_ID[] = { 0x01, 0x02, 0x0E, 0x91 };
byte NODE_ID_RESPONSE[10] = { 0x01, 0x91 };

byte READ_UID[] = { 0x01, 0x01, 0x0B };
byte UID_RESPONSE[10] = { 0xD0, 0x02, 0x0D, 0xE4, 0x3F, 0x56, 0x69, 0x67 };


void requestEvent() {
  //Serial.println("READ!!!!!");
  byte* response;
  if(eq(readRegister, READ_SELECT_COMMAND))
      response = NODE_ID_RESPONSE;
  if(eq(readRegister, READ_NODE_ID))
    response = NODE_ID_RESPONSE;
  if(eq(readRegister, READ_UID))
    response = UID_RESPONSE;
  else
    response = getBlockByRegister(readRegister[3]);

  Serial.print("READ at: ");
  p(readRegister);

  Wire.write(response, sizeof(response));

  Serial.print(" responding with: ");
  p(response);
}

void setup() {
  Wire.begin(I2C_ADDRESS);

  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  Serial.begin(9600);
  Serial.println("Starting");

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  delay(100);
  //Serial.println("Waiting ..");
}


