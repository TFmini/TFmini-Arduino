#include <Wire.h>

#define I2C_SLAVE_ADDR1  0x03 

typedef struct {
	char distanceLow;
	char distanceHigh;
	int distance;
	char strengthLow;
	char strengthHigh;
	int strength;
	boolean receiveComplete;
}TFmini;

TFmini TFminiOne = {0, 0, false};

void setup() {
	Wire.begin(); 	// join i2c bus (address optional for master)
	Serial.begin(115200);
}

void loop() {

	Wire.requestFrom(I2C_SLAVE_ADDR1, 4);    // request 4 bytes from slave device 
	char receiveCount = 0;
	while(Wire.available()) {
		switch(++receiveCount) {
			case 1: TFminiOne.distanceLow = Wire.read(); break;
			case 2: TFminiOne.distanceHigh = Wire.read(); break;
			case 3: TFminiOne.strengthLow = Wire.read(); break;
			case 4: 
				TFminiOne.strengthHigh = Wire.read(); 
				receiveCount = 0; 
				TFminiOne.receiveComplete = true;
				break;
		}
	}    
	
	static unsigned long lastTime = millis();
	static unsigned int count = 0;
	static unsigned int frequency = 0;

	if(TFminiOne.receiveComplete == true) {
		TFminiOne.receiveComplete = false;

		++count;
		if(millis() - lastTime > 999) {
			lastTime = millis();
			frequency = count;
			count = 0;
		}   

		TFminiOne.distance = TFminiOne.distanceLow + TFminiOne.distanceHigh * 256;
		TFminiOne.strength = TFminiOne.strengthLow + TFminiOne.strengthHigh * 256;
		
		Serial.print(TFminiOne.distance);
		Serial.print("cm\t");
		Serial.print("strength1: ");
		Serial.print(TFminiOne.strength);
		Serial.print("\t");
		Serial.print(frequency);  //40~70Hz, It maybe higher if we don't print other thing. 
		Serial.println("Hz");
	}

}
