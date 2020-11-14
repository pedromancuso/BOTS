ESP8266WebServer server(80);
extern int EESSID;extern int EEPASS;
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
void WiFiConnect(String STAssid,String STApassword) {
  int contconexion = 0;
  WiFi.begin(STAssid, STApassword);
  while (WiFi.status() != WL_CONNECTED && contconexion < 20){contconexion++;delay(500);Serial.print(".");}
  Serial.print("STA IP: "); Serial.println(WiFi.localIP());
  if (WiFi.status() == WL_CONNECTED){
    estado = "Conexion Exitosa";   
      Serial.println("EEPROM SAVED");
      EEPROM.put(EESSID,STAssid);EEPROM.put(EEPASS,STApassword);EEPROM.commit();
      EEPROM.commit();
    
  }else{estado = "Error de Conexion";Serial.println("error de conexion");}
  WIFISSID verssid;WIFIPASS verpass;
  Serial.print("\nEEPROM --> SSID: ");Serial.print(EEPROM.get(EESSID,verssid.wfssid));Serial.print(" / PASS: ");Serial.println(EEPROM.get(EEPASS,verpass.wfpass));
}
////////////////////////////////RESPONSE//////////////////////////////////////////////////////////////////////////
////////////////////////////////RESPONSE//////////////////////////////////////////////////////////////////////////
void response(){
  ESP.eraseConfig(); //se cae ap ??
  WiFi.disconnect(true);
  server.send(200, "text/html", TEMP);
  if (server.hasArg("STAssid") && server.hasArg("STApassword")) {
    Serial.print("SSID: "); Serial.print(server.arg("STAssid"));
    Serial.print(" / PASS: "); Serial.println(server.arg("STApassword"));
    WiFiConnect(server.arg("STAssid"), server.arg("STApassword"));
  }else{WiFiConnect("nossid", "nopassword");}
}
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
void webpage() {
  String LANDING = LANDING1;
  if (estado != "waiting"){LANDING += "<div id='estadowifi'>"+estado+"</div>";}
  LANDING += LANDING2;
  server.send(200, "text/html", LANDING);
}
////////////////////////////////WIFISET//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFISET//////////////////////////////////////////////////////////////////////////
const char *APssid = "BOTS";
const char *APpassword = "password123";
void wifiset(){
  WIFISSID customssid;WIFIPASS custompass;
  if(EEPROM.get(EESSID,customssid.wfssid) && EEPROM.get(EEPASS,custompass.wfpass)){
    Serial.print("\nEEPROM --> SSID: ");Serial.print(EEPROM.get(EESSID,customssid.wfssid));Serial.print(" / PASS: ");Serial.println(EEPROM.get(EEPASS,custompass.wfpass));
  }else{EEPROM.put(EESSID,wifissid);EEPROM.put(EEPASS,wifipass);EEPROM.commit();}
  ESP.eraseConfig();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", webpage);
  server.on("/wificonfig", response);
  server.begin();
  Serial.print("AP CONFIG --> SSID: "); Serial.print(APssid); Serial.print(" / PASS: "); Serial.println(APpassword);
  Serial.print("AP IP: "); Serial.println(myIP);
  //if(WiFi.begin(STAssid, STApassword);  
}
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
void wifiloop(){server.handleClient();}
