#include <DHT.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//SET VALUES 
int Set_Temperature;
int Set_Humidity;
int Set_Moisture;
int Set_Light;

#define delimiter '>'
#define eof '*'

int cm;
int Moisture;
//INPUT PINS
//#define Soil_Moisture 7
#define DHT_pin 4
#define TRIGGER 6
#define ECHO 7
#define LDR1 A8
#define LDR2 A9
#define Soil_Moisture A10

 
#define POT_Temperature A9
#define POT_Humidity A10
#define POT_Moisture A11
#define POT_Light A12


//OUTPUT PINS ANALOG
#define LED0 A0 //SYSTEM RUNNING
#define LED1 A1 //HUMIDITY
#define LED2 A2 //TEMPERATURE
#define LED3 A3 //SOIL MOISTURE
#define LED4 A4 //LIGHT INTENSITY
#define LED5 A5 //WATER LEVEL


//OUTPUT PINS DIGITAL
#define Water_Pump 8
#define motor 9
#define GrowingLED 10
//#define Low_Water 29

//STEPPER MOTORS
#define IN01 23
#define IN02 25
#define IN03 27
#define IN04 29

#define IN11 39
#define IN12 41
#define IN13 43
#define IN14 45


//SET DHT SENSOR
#define DHTTYPE DHT11
DHT dht(DHT_pin, DHTTYPE);

//SET INTERNAL VARIABLE
int internal;
int state;
int distance;
float temperatureActual;
float humidityActual;
int light_Intensity_Actual_1;
int light_Intensity_Actual_2;
int moisture_Actual;
int moistPct;

LiquidCrystal_I2C lcd(0x27,20,4);
void setup(){
  lcd.init();
  lcd.backlight();
  lcd.begin(20, 4);
  lcd.setCursor(0,0); 
  lcd.print("REMOTELY CONTROLLED");
  lcd.setCursor(4,1); 
  lcd.print("GREENHOUSE");
  lcd.setCursor(0,2); 
  lcd.print("1. VICTOR EPHRAIM");
  lcd.setCursor(0,3);
  lcd.print("2. DERICK KABYEMELA");
  delay(3000);
  
  Serial.begin(115200);
  
  dht.begin();
  
  pinMode(LDR1,INPUT);
  pinMode(LDR2,INPUT);
  
  pinMode(Soil_Moisture,INPUT);
  
  pinMode(LED0,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);
  
  pinMode(Water_Pump,OUTPUT);
  pinMode(GrowingLED,OUTPUT);
  pinMode(motor,OUTPUT);
  
  pinMode(IN01,OUTPUT);
  pinMode(IN02,OUTPUT);
  pinMode(IN03,OUTPUT);
  pinMode(IN04,OUTPUT);
  
  pinMode(IN11,OUTPUT);
  pinMode(IN12,OUTPUT);
  pinMode(IN13,OUTPUT);
  pinMode(IN14,OUTPUT);

  pinMode(TRIGGER,OUTPUT);
  pinMode(ECHO,INPUT);
 
  analogWrite(LED0,255);//SYSTEM LED ON
  lcd.clear();
}

void loop() {
  String sendData;
  
  distance= readDistance();
  temperatureActual = dht.readTemperature();
  humidityActual = dht.readHumidity();
  light_Intensity_Actual_1 = map(analogRead(LDR1),0, 1023, 0 ,1000);
  light_Intensity_Actual_2 = map(analogRead(LDR2),0, 1023, 0 ,1000);
  moisture_Actual = analogRead(Soil_Moisture);
  moistPct = map(moisture_Actual,0,1023,100,0);
  
  lcd.begin(20, 4);
  lcd.setCursor(0,0); 
  lcd.print("Distance: ");
  lcd.setCursor(12,0);
  lcd.print(distance);
  lcd.setCursor(18,0);
  lcd.print("cm");
  lcd.setCursor(0,1); 
  lcd.print("Temperature: ");
  lcd.setCursor(12,1);
  lcd.print(temperatureActual);
  lcd.setCursor(18,1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,2); 
  lcd.print("Humidity: ");
  lcd.setCursor(12,2);
  lcd.print(humidityActual);
  lcd.setCursor(19,2);
  lcd.print("%");
  lcd.setCursor(0,3); 
  lcd.print("Light: ");
  lcd.setCursor(12,3);
  lcd.print(light_Intensity_Actual_1);
  lcd.print(" & ");
  lcd.print(light_Intensity_Actual_2);
  delay(500);
  lcd.clear();
 
  lcd.setCursor(0,0); 
  lcd.print("Moisture: ");
  lcd.setCursor(12,0);
  lcd.print(moistPct);
  lcd.setCursor(19,0);
  lcd.print("%");
  lcd.setCursor(0,1); 
  lcd.print("Temperature: ");
  lcd.setCursor(12,1);
  lcd.print(temperatureActual);
  lcd.setCursor(18,1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,2); 
  lcd.print("Humidity: ");
  lcd.setCursor(12,2);
  lcd.print(humidityActual);
  lcd.setCursor(19,2);
  lcd.print("%");
  lcd.setCursor(0,3); 
  lcd.print("Light: ");
  lcd.setCursor(12,3);
  lcd.print(light_Intensity_Actual_1);
  lcd.print(" & ");
  lcd.print(light_Intensity_Actual_2);
  delay(500);
  
//Water Level
  if(distance > 28){
    analogWrite(LED1,255);
  }
  else{
    analogWrite(LED1,0);
  }

//Moisture Level
  if(moisture_Actual > 500){
    digitalWrite(Water_Pump,HIGH);
  }
  else{
    digitalWrite(Water_Pump,LOW);
  }

//Light Intensity
  if(light_Intensity_Actual_1 < 20 && light_Intensity_Actual_2 < 20){
    digitalWrite(GrowingLED,HIGH);
  }
  else{
    digitalWrite(GrowingLED,LOW);
  }
  
//Humidity Control
  if(humidityActual > 90){
    digitalWrite(motor,HIGH);
    openWindow();
  }
  else{
    digitalWrite(motor,LOW);
    closeWindow();
  }
sendData = String(temperatureActual) + String(delimiter) + String(moistPct) + String(delimiter) + String(humidityActual) + String(eof) ;
  Serial.println(sendData);
 Serial.flush();
delay(10000);
 sendData = " ";
}
int readDistance(){
  //ULTRASONIC DISTANCE MEASURE
  long duration, inches, cm;
  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  pinMode(ECHO, INPUT);
  duration = pulseIn(ECHO, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration); 
  delay(500);
  return cm;
   
}
long microsecondsToInches(long microseconds)
{
 return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
   return microseconds / 29 / 2;
}

void openWindow(){
  state =1;  
    if((state ==1 )&& (internal == 1)){
     int loopOne =millis();
     int incrementLoopOne = loopOne+10000;
     for(loopOne;loopOne<incrementLoopOne;loopOne){
          digitalWrite(IN01,LOW);
          digitalWrite(IN02,LOW);
          digitalWrite(IN03,LOW);
          digitalWrite(IN04,HIGH);
          delay(5);
          digitalWrite(IN01,LOW);
          digitalWrite(IN02,LOW);
          digitalWrite(IN03,HIGH);
          digitalWrite(IN04,HIGH);
          delay(5);
          digitalWrite(IN01,LOW);
          digitalWrite(IN02,LOW);
          digitalWrite(IN03,HIGH);
          digitalWrite(IN04,LOW);
          delay(5);
          digitalWrite(IN01,LOW);
          digitalWrite(IN02,HIGH);
          digitalWrite(IN03,HIGH);
          digitalWrite(IN04,LOW);
          delay(5);
          digitalWrite(IN01,LOW);
          digitalWrite(IN02,HIGH);
          digitalWrite(IN03,LOW);
          digitalWrite(IN04,LOW);
          delay(5);
          digitalWrite(IN01,HIGH);
          digitalWrite(IN02,HIGH);
          digitalWrite(IN03,LOW);
          digitalWrite(IN04,LOW);
          delay(5);
          digitalWrite(IN01,HIGH);
          digitalWrite(IN02,LOW);
          digitalWrite(IN03,LOW);
          digitalWrite(IN04,LOW);
          delay(5);
          digitalWrite(IN01,HIGH);
          digitalWrite(IN02,LOW);
          digitalWrite(IN03,LOW);
          digitalWrite(IN04,HIGH);
          delay(5); 
     }
     delay(200);
     internal = 0;
    }     
}
void closeWindow(){
  
}
