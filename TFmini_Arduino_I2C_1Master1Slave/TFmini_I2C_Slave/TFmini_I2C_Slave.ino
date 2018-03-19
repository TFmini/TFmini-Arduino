#include <Wire.h>

#define I2C_SLAVE_ADDR  0x03 

typedef struct {
	char distanceLow;
	char distanceHigh;
	int distance;
	char strengthLow;
	char strengthHigh;
	int strength;
	boolean receiveComplete;
}TFmini;

TFmini TFminiOne = {0, 0, 0, 0, 0, 0, false};

void getTFminiData(TFmini* tfmini) {
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
				tfmini->distanceLow = rx[2];
				tfmini->distanceHigh = rx[3];
				tfmini->strengthLow = rx[4];
				tfmini->strengthHigh = rx[5];
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


void setup() {
	Wire.begin(I2C_SLAVE_ADDR); // join i2c bus with address #59
	Wire.onRequest(requestEvent); // register event
	Serial.begin(115200);
}

void loop() {
	static unsigned long lastTime = millis();
	static unsigned int count = 0;
	static unsigned int frequency = 0;
	
	if(TFminiOne.receiveComplete == true) {
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
		Serial.print(frequency);  //40~70Hz, It maybe higher if we don't print other thing. 
		Serial.println("Hz");
		
		TFminiOne.receiveComplete = false;
	}
}

void serialEvent() {
	getTFminiData(&TFminiOne);
}

// function that executes whenever data is requested by master
void requestEvent() {
	Wire.write(TFminiOne.distanceLow);
	Wire.write(TFminiOne.distanceHigh);
	Wire.write(TFminiOne.strengthLow);
	Wire.write(TFminiOne.strengthHigh);
}
















 
