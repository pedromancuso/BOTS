  ///////////////////////////////INCLUDE////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <PubSubClient.h>
#include <stdlib.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> /*Poner esta libreria despues de ESP8266WiFi.h*/
#include <EEPROM_Rotate.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiUdp.h>
#include "1Auxiliar.hpp"
#include "2EEPROM.hpp"
#include "3RF.hpp"
#include "4MQTT.hpp"
#include "5BOTS.hpp"
#include "6WIFI.hpp"
unsigned long NTPcheck=20000;
///////////////////////////////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(115200);for(int t=0;t<50;t++){Serial.println("");}
  Serial.print("Sketch Start MAC: ");Serial.println(WiFi.macAddress());
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  eepromsetup();
  RFSetup();
  wifisetup();
  millis();
} 
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool mqttset=true;
bool botsset=true;
bool programasset=true;
bool botsrestart=true;
bool mqttrestart=true;
bool wifirestart=true;
bool BOTSCONNECTED=false;
bool MQTTCONNECTED=false;
unsigned long prevMillis;
unsigned long prevMi;
void loop(){
  wifiloop();
  if(RFReceive==1){RecepRF();}
  /*----------------------------HANDLER--------------------------*/
  if(WiFi.status() == WL_CONNECTED){
    if(BOTSCONNECTED==true){
      if(botsset==true){botsset=botssetup();}
      if(programasset==true){programasset=programassetup();}
      if(MQTTCONNECTED==true){
        if(client.connected()){digitalWrite(LED_BUILTIN,LOW);}else{digitalWrite(LED_BUILTIN,HIGH);}
        mqttclienthandle();
        if((millis()-prevMillis)>=NTPcheck){prevMillis=millis();NTPUpdate();ProgramasHandler();}
      }else{if(mqttset==true){mqttset=mqttsetup();}}
    }else{if(botsrestart==true){botsrestart=botseepromrestart();}}
  }else{if(wifirestart==true){wifirestart=wifieepromrestart();}}
}



  
