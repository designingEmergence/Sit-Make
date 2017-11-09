//ORDER
/*
Flex Short L
Flex Short R
Flex Long L
Flex Long R
Softpot L
Softpot R
Methane
FSR L
FSR R
Ultrasonic
Temp
Accel
Button
*/

// connect Multiplexer S0~S3 to Arduino D7~D4 respectively
// connect Multiplexer pin 1 to Arduino A0

// connect accelerometer 3.3V to Arduino 3.3V
// connect accelerometer GND to Arduino GND
// connect accelerometer SDA to Arduino A4 via 330 Ohm Resistor
// connect accelerometer SCL to Arduino A5 via 330 Ohm Resistor

// connect ultrasonic Echo to Arduino pin 13 Trig to Arduino pin 12
// connect ultrasonic VCC to arduino 5v GND to arduino GND

//connect button to digital pin 10

#include <Wire.h> // Must include Wire library for I2C
#include <SFE_MMA8452Q.h> // Includes the SFE_MMA8452Q library
#include <OneWire.h> 
#include <DallasTemperature.h>

#define trigPin 13
#define echoPin 12
#define temperaturePin 2
#define buttonPin 10

MMA8452Q accel;
float maxAccelZ;
boolean buttonPressed = false;

int multiplexerPins = 9;
String output = "";
unsigned long previousMillis = 0;
const long interval = 1000;

OneWire oneWire(temperaturePin);
DallasTemperature sensors(&oneWire);

byte controlPins[] = {B00000000, 
                  B10000000,
                  B01000000,
                  B11000000,
                  B00100000,
                  B10100000,
                  B01100000,
                  B11100000,
                  B00010000,
                  B10010000,
                  B01010000,
                  B11010000,
                  B00110000,
                  B10110000,
                  B01110000,
                  B11110000 }; 
 
// holds incoming values from 74HC4067                  
int muxValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
 
void setup()
{
  Serial.begin(9600);
  Serial.println("start");
  maxAccelZ =0;
  
  DDRD = B11111100; // set PORTD (digital 7~0) to outputs
  accel.init(SCALE_8G); //sets accelerometer to measure up to 8Gs
  sensors.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT);  
}

void loop(){
  

  unsigned long currentMillis = millis();  
  getAccel();
  getButton();
  //Serial.println("hi");
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    getMultiplexerData();
    getUltrasonic();
    getTemp();
    printMaxAccelZ();
    printButton();
    Serial.println(output);
    buttonPressed = false;
  }
  output = "";
}

void getAccel(){
  if (accel.available()){
    accel.read();
    if (abs(accel.cz) > maxAccelZ){
      maxAccelZ = abs(accel.cz);
    }
  }
}

void getButton(){
  if(digitalRead(buttonPin) == HIGH){
    buttonPressed = true;
  }  
}

void getMultiplexerData(){
   for (int i = 0; i < multiplexerPins; i++){
    setPin(i); // choose an input pin on the 74HC4067
    muxValues[i]=analogRead(0); // read the vlaue on that pin and store in array
   }
   displayMultiplexerData();
}

void setPin(int outputPin){
// function to select pin on 74HC4067
  PORTD = controlPins[outputPin];
}

void displayMultiplexerData(){
// dumps captured data from array to serial monitor
  for (int i = 0; i < multiplexerPins; i++)
  {
    //Serial.print("input I"); 
    output += muxValues[i]; 
    output += " "; 
  }
}

void getTemp(){
  sensors.requestTemperatures();
  output += sensors.getTempFByIndex(0);
  output += " ";
}

void getUltrasonic(){
  long duration;
  float distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/29.1;
  output += distance;
  output += " ";
}

void printButton(){
  if (buttonPressed == true){
    output += 1;
  }
  else if( buttonPressed == false){
    output += 0;
  }

  output += " ";
}
void printMaxAccelZ(){
  output += String(maxAccelZ,3);
  output += " ";  
}


 

