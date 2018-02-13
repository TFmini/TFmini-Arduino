# TFmini-Arduino
TFmini在Arduino上的一些例子.  

## TFmini_Arduino_HardwareSerial_Polling  
类似于Arduino Uno, 只有一个串口的Arduino板子, 并不妨碍我们读取TFmini的数据, 只需要连接TFmini的TX到Arduino的RX即可, 不影响Arduino的TX向PC发数据, 连接方式如下:  

![TFmini-Arduino](/Assets/TFmini-Arduino.png)  


TFmini | Arduino 
---------|----------
 5V(红) | 5V 
 GND(黑) | GND 
 TX(绿) | 0(RX)

采用轮询的方式, 代码如下:  

```Arduino
void getTFminiData(int* distance, int* strength) {
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
  int distance = 0;
  int strength = 0;

  getTFminiData(&distance, &strength);
  while(!distance) {
    getTFminiData(&distance, &strength);
    if(distance) {
      Serial.print(distance);
      Serial.print("cm\t");
      Serial.print("strength: ");
      Serial.println(strength);
    }
  }
}
```  

**注意下载程序的时候, 拔掉TFmini的TX, 下完程序后再插到Arduino的0(RX)上.**

---

## TFmini_Arduino_HardwareSerial_Interrupt  

连接方式同上, 我们在串口时间中接收, 接收完成后, 在loop()中打印:  

```Arduino
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
```

**注意下载程序的时候, 拔掉TFmini的TX, 下完程序后再插到Arduino的0(RX)上.**  

---

## TFmini_Arduino_SoftwareSerial  

使用软件串口库, 来模拟一个串口. 连接方式如下:  

![](/Assets/TFmini-SoftSerial.png)

代码如下: 

```Arduino
#include <SoftwareSerial.h>  //header file of software serial port

SoftwareSerial Serial1(2,3); //define software serial port name as Serial1 and define pin2 as RX and pin3 as TX

/* For Arduinoboards with multiple serial ports like DUEboard, interpret above two pieces of code and directly use Serial1 serial port*/

int dist;	//actual distance measurements of LiDAR
int strength;	//signal strength of LiDAR
int check;	//save check value
int i;
int uart[9];	//save data measured by LiDAR
const int HEADER=0x59;	//frame header of data package

void setup() {
	Serial.begin(9600);	//set bit rate of serial port connecting Arduino with computer
	Serial1.begin(115200);	//set bit rate of serial port connecting LiDAR with Arduino
}

void loop() { 
	if (Serial1.available()) {	//check if serial port has data input
		if(Serial1.read() == HEADER) {	//assess data package frame header 0x59
			uart[0]=HEADER;
			if (Serial1.read() == HEADER) { //assess data package frame header 0x59
				uart[1] = HEADER;
				for (i = 2; i < 9; i++) { //save data in array
					uart[i] = Serial1.read();
				}
				check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
				if (uart[8] == (check & 0xff)){ //verify the received data as per protocol
					dist = uart[2] + uart[3] * 256;     //calculate distance value
					strength = uart[4] + uart[5] * 256; //calculate signal strength value
					Serial.print("dist = ");
					Serial.print(dist); //output measure distance value of LiDAR
					Serial.print('\t');
					Serial.print("strength = ");
					Serial.print(strength); //output signal strength value
					Serial.print('\n');
				}
			}
		}
	}
}
```  

下载程序, 运行即可.  