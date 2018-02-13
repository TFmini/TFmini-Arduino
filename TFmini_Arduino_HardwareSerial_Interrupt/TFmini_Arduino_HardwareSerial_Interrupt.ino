int distance = 0;
int strength = 0;
boolean receiveComplete = false;

void getTFminiData(int* distance, int* strength, boolean* complete) {
  static char i = 0;
  char j = 0;
  int checksum = 0; 
  static int rx[9];
  if(Serial.available()) {  
    rx[i] = Serial.read();
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
}

void loop() {
  if(receiveComplete) {
    receiveComplete = false;
    Serial.print(distance);
    Serial.print("cm\t");
    Serial.print("strength: ");
    Serial.println(strength);
  }
}

void serialEvent() {
  getTFminiData(&distance, &strength, &receiveComplete);
}

