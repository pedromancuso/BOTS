#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <stdlib.h>
#include <WiFiClient.h>
#include "landing.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h> 
int wificonectado;
const char *APssid = "BOTS";
const char *APpassword = "password123";
boolean APestado;
struct WIFICONF{String wfssid;String wfpass;};
WIFICONF wificonf = {"a","b"};
int eeAddress = 0;
/*WiFiClient espClient;
PubSubClient client(espClient);*/
ESP8266WebServer server(80);
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
void WiFiConnect(String STAssid,String STApassword){
  Serial.println("wificonnect");
  int contconexion = 0;
  wificonectado=0;
  WiFi.begin(STAssid, STApassword);
  while(WiFi.status() != WL_CONNECTED && contconexion<20){contconexion++;delay(500);Serial.print(".");}
  Serial.print("STA IP: ");Serial.println(WiFi.localIP());
  if(WiFi.status() == WL_CONNECTED){Serial.print("Conexion Exitosa");estado="Conexion Exitosa";}
  else{Serial.print("Error de Conexion");estado="Error de Conexion";}
  Serial.println("Server.send!"); 
}
////////////////////////////////RESPONSE//////////////////////////////////////////////////////////////////////////
////////////////////////////////RESPONSE//////////////////////////////////////////////////////////////////////////
void response(){
  Serial.println("response"); 
  ESP.eraseConfig();
  WiFi.disconnect(true);
  server.send(200, "text/html", TEMP);
  if(server.hasArg("STAssid") && server.hasArg("STApassword")){
    String var1 = server.arg("STAssid");
    String var2 = server.arg("STApassword");
    wificonf = {var1,var2};
    EEPROM.put(eeAddress,wificonf);
    String STAssid = EEPROM.get(eeAddress,wificonf.wfssid);
    String STApassword = EEPROM.get(eeAddress,wificonf.wfpass);
    Serial.print("SSID:\t");Serial.print(STAssid);
    Serial.print(", PASS:\t");Serial.println(STApassword);
    WiFiConnect(STAssid,STApassword);
  }
}
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
void webpage(){
  Serial.println("webpage");
  String LANDING =LANDING1;
  if(estado!="waiting"){LANDING += "<div id='recuperarcontrasena'>"+estado+"</div>";}
  LANDING +=LANDING2;
  server.send(200, "text/html", LANDING);
}
///////////////////////////////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////SETUP///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(115200);system("cls");
  eeAddress += sizeof(WIFICONF);
  ESP.eraseConfig();
  EEPROM.put(eeAddress,wificonf);
  String xSTAssid = EEPROM.get(eeAddress,wificonf.wfssid);
  String xSTApassword = EEPROM.get(eeAddress,wificonf.wfpass);
  Serial.print("SSID:\t");Serial.print(xSTAssid);
  Serial.print(", PASS:\t");Serial.println(xSTApassword);
  wificonectado=0;
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  WiFi.mode(WIFI_AP_STA);
  APestado = WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", webpage);
  server.on("/wificonfig",response);
  //server.on("/",HTTP_GET, webpage);
  //server.on("/",HTTP_POST,response);
  server.begin();
  if(APestado==true){
    Serial.print("Configuración AP--> SSID: ");Serial.print(APssid);Serial.print(" / PASS: ");Serial.println(APpassword);
    Serial.print("AP IP: ");Serial.println(myIP);
  }else{Serial.println("AP No Iniciado");}
} 
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){server.handleClient();}
