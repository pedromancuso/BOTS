///////////////////////////////INCLUDE////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h> 
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <EEPROM_Rotate.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiUdp.h>
#include "html.h"
#include "EEPROM.hpp"
#include "PHP.hpp"
#include "MQTT.hpp  "
#include "WIFI.hpp"
///////////////////////////////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(9600);for(int t=0;t<50;t++){Serial.println("");}
  Serial.print("Sketch Start MAC: ");Serial.println(WiFi.macAddress());
  pinMode(LED_BUILTIN,OUTPUT);
  eepromsetup();
  wifisetup();
} 
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long previousMillis = 0;
void loop2(){
  wifiloop();
  if (WiFi.status() != WL_CONNECTED){
    estado=0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 1000){previousMillis = currentMillis;
      if (digitalRead(LED_BUILTIN)==LOW){digitalWrite(LED_BUILTIN,HIGH);}else{digitalWrite(LED_BUILTIN,LOW);}
    }
  }
  if(estado>=400){mqttclienthandle();}
}
///////////////////////////////SERIAL////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int incoming=1;int incomingByte;
void loop(){incomingByte=Serial.read(); // read the incoming byte:
    if(incomingByte=='0' || incomingByte=='1'){incoming=incomingByte;Serial.print("incomingByte: ");Serial.println(incomingByte);};
    switch(incoming){case '0':{break;}case '1':{loop2();break;}default:{loop2();break;}}
}
