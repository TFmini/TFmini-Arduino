#include <SoftwareSerial.h>  

//SoftwareSerial port(TX, RX);
SoftwareSerial portOne(2, 2);
SoftwareSerial portTwo(3, 3); 

void getTFminiData(SoftwareSerial* port, int* distance, int* strength, boolean* complete) {
  static char i = 0;
  char j = 0;
  int checksum = 0; 
  static int rx[9];

  port->listen();
  if(port->available()) {  
    rx[i] = port->read();
    if(rx[0] != 0x59) {
      i = 0;
    } else if(i == 1 && rx[1] != 0x59) {
      i = 0;
    } else if(i == 8) {
      for(j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if(rx[8] == (checksum % 256)) {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
        *complete = true;
      }
      i = 0;
    } else {
      i++;
    } 
  }  
}

void setup() {
  Serial.begin(115200);
  portOne.begin(115200);
  portTwo.begin(115200);
}

void loop() {
  int distance1 = 0;
  int strength1 = 0;
  boolean receiveComplete1 = false;

  int distance2 = 0;
  int strength2 = 0;
  boolean receiveComplete2 = false;

  while(!receiveComplete1) {
    getTFminiData(&portOne, &distance1, &strength1, &receiveComplete1);
    if(receiveComplete1) {
      Serial.print(distance1);
      Serial.print("cm\t");
      Serial.print("strength1: ");
      Serial.print(strength1);
      Serial.print("\t");
    }
  }
  receiveComplete1 = false;

  while(!receiveComplete2) {
    getTFminiData(&portTwo, &distance2, &strength2, &receiveComplete2);
    if(receiveComplete2) {
      Serial.print(distance2);
      Serial.print("cm\t");
      Serial.print("strength2: ");
      Serial.println(strength2);
    }
  }
  receiveComplete2 = false;
}
