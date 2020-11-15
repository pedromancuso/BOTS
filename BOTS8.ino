///////////////////////////////INCLUDE////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <PubSubClient.h>
#include <stdlib.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> /*Poner esta libreria despues de ESP8266WiFi.h*/
#include <EEPROM_Rotate.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiUdp.h>
#include "EEPROM.hpp"
#include "RF.hpp"
#include "MQTT.hpp"
#include "WIFI.hpp"
///////////////////////////////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(115200);for(int t=0;t<50;t++){Serial.println(F(""));}
  Serial.print("Sketch Start MAC: ");Serial.println(WiFi.macAddress());
  pinMode(LED_BUILTIN,OUTPUT);
  eepromsetup();
  RFSetup();
  wifisetup();
} 
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
  wifiloop();
  if(RFReceive==1){RecepRF();}
  //0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  if (WiFi.status() != WL_CONNECTED){digitalWrite(LED_BUILTIN,HIGH);estado=0;}
  if(estado>=400){digitalWrite(LED_BUILTIN,LOW);mqttclienthandle();}
}
