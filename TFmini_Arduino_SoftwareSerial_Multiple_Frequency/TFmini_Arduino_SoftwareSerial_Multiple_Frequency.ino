#include <SoftwareSerial.h>  

typedef struct {
  int distance;
  int strength;
  boolean receiveComplete;
}TFmini;

TFmini TFminiOne = {0, 0, false};
TFmini TFminiTwo = {0, 0, false};

//SoftwareSerial port(TX, RX);
SoftwareSerial portOne(2, 2);
SoftwareSerial portTwo(3, 3); 

void getTFminiData(SoftwareSerial* port, TFmini* tfmini) {
  port->listen();
  while(tfmini->receiveComplete == false) 
  {   
    static char i = 0;
    char j = 0;
    int checksum = 0; 
    static int rx[9];
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
          tfmini->distance = rx[2] + rx[3] * 256;
          tfmini->strength = rx[4] + rx[5] * 256;
          tfmini->receiveComplete = true;
        }
        i = 0;
      } else {
        i++;
      } 
    } 
  } 
}

void setup() {
  Serial.begin(115200);
  portOne.begin(115200);
  portTwo.begin(115200);
}

void loop() {
    static unsigned long lastTime = millis();
    static unsigned int count = 0;
    static unsigned int frequency = 0;
    
    getTFminiData(&portOne, &TFminiOne);
    getTFminiData(&portTwo, &TFminiTwo);
    
    if(TFminiOne.receiveComplete == true && TFminiTwo.receiveComplete == true) {
      ++count;

      if(millis() - lastTime > 999) {
        lastTime = millis();
        frequency = count;
        count = 0;
      }   
    
      Serial.print(TFminiOne.distance);
      Serial.print("cm\t");
      Serial.print("strength1: ");
      Serial.print(TFminiOne.strength);
      Serial.print("\t");
    
      Serial.print(TFminiTwo.distance);
      Serial.print("cm\t");
      Serial.print("strength2: ");
      Serial.print(TFminiTwo.strength);
      Serial.print("\t");

      Serial.print(frequency);  //40~70Hz, It maybe higher if we don't print other thing. 
      Serial.println("Hz");
      
      TFminiOne.receiveComplete = false;
      TFminiTwo.receiveComplete = false;
    }

 }

