
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>



#define delimiter '>'
#define eof '*'
SoftwareSerial mySerial(0,1); // RX, TX

const char* ssid = "usipende vya bure";
const char* password =  "derick24011995";

const char *mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

String data1;
String data2;
String n;

int ind1;
int ind2;
int ind3;
String readString;
WiFiClient espClient;
PubSubClient client(espClient);;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  mySerial.begin(115200);
  WiFi.begin(ssid, password);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
while (WiFi.status() != WL_CONNECTED) {
delay(500);
mySerial.println("Connecting to WiFi..");

}
mySerial.println("connected to Wi-Fi");

client.setServer(mqttServer, mqttPort);
client.setCallback(callback);


  while (!client.connected()) {
    mySerial.println("Connecting to MQTT...");
    delay(1000);
    if(client.connect("esp8266-client")){
  
mySerial.println("Connected to the broker network");
}
else{
  mySerial.println("fAILED with state " +client.state());
  delay(2000);
}
 
}

mySerial.println("MQTT connected");
client.publish("dar/greenhouse/status", "ON");


  
}
void callback(char* topic, byte* payload, unsigned int length) {
 
  mySerial.print("Message arrived in topic: ");
  mySerial.println(topic);
}

void loop() { // run over and over
  client.loop();
  
  
 String inData;
  if (Serial.available()) {
    Serial.flush();
    mySerial.flush();
    inData = Serial.readString();
    mySerial.println(inData);
    
  if( inData.indexOf('*') > 0){
     int i= inData.indexOf(eof);
     ind1 = inData.indexOf(delimiter);
     data1 = inData.substring(0,ind1);
     ind2 = inData.indexOf(delimiter,ind1+1);
     data2 = inData.substring(ind1+1,ind2);
     ind3 = inData.indexOf(eof,ind2+1);
     n = inData.substring(ind2+1,ind3);

   mySerial.println("data1: "+data1);
   mySerial.println("data2: "+data2);
   mySerial.println("n: "+n);

   client.publish("dar/temperature",data1.c_str());

client.publish("dar/moisture",data2.c_str());

client.publish("dar/humidity",n.c_str());
  }
  else{
    readString += inData;
  }
  data1= "";
  data2="" ;
  n = "";
}

delay(2000);
}
