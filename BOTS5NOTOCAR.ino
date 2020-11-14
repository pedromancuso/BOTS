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
#include "weblanding.h"
#include "weblogin.h"
#include "webuser.h"
#include "EEPROM.hpp"
#include "PHP.hpp"
#include "WIFI.hpp"
#include "MQTT.hpp"
///////////////////////////////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(115200);for(int t=0;t<50;t++){Serial.println("");}Serial.println("Sketch Iniciado");
  pinMode(LED_BUILTIN,OUTPUT);
  eepromsetup();
  wifisetup();
} 
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
  wifiloop();
  if(WiFi.status() == WL_CONNECTED){
    //digitalWrite(LED_BUILTIN,LOW);
      mqttloop();
  }else{delay(1000);if (digitalRead(LED_BUILTIN)==LOW){digitalWrite(LED_BUILTIN,HIGH);}else{digitalWrite(LED_BUILTIN,LOW);}} 
}
