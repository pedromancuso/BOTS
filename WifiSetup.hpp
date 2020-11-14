extern int wificonectado;
ESP8266WebServer server(80);
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
void WiFiConnect(String ssidx,String passx){
  int contconexion = 0;
  //bool WiFi.softAPdisconnect(bool wifioff = false);// desconectar el AP;
  WiFi.begin(ssidx, passx);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50){
    Serial.print("WiFi conectado IP: ");
    Serial.println(WiFi.localIP());
  }else{
    Serial.println("Error de conexion");
    server.send(400,"text/html","<html><body><h1>HTTP Error 400</h1><p>Bad request. Please enter a value.</p></body></html>");
  }
  if(WiFi.status() == WL_CONNECTED){
    wificonectado=1;
    server.send(200,"text/html","<html><body><h1>Successful</h1><a href='/'>Home</a></body></html>");
  }
}
////////////////////////////////RESPONSE//////////////////////////////////////////////////////////////////////////
////////////////////////////////RESPONSE//////////////////////////////////////////////////////////////////////////
void response(){
    //EEPROM.update(0,server.arg("pass"));
    if(server.hasArg("ss") && server.hasArg("pass")){
      EEPROM.put(0,server.arg("ss"));
      EEPROM.put(0,server.arg("pass"));
      String inString1 = EEPROM.get(0,server.arg("ss"));
      String inString2 = EEPROM.get(0,server.arg("pass"));
      Serial.print("SSID:\t");Serial.print(inString1);
      Serial.print(", PASS:\t");Serial.println(inString2);
      WiFiConnect(inString1,inString2);
    }
}
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
void webpage(){
  server.send(200,"text/html","<html><body><form name='frm'  method='post'><input placeholder='ssid' type='text' name='ss'><br><input placeholder='pass' type='text' name='pass'><br><input type='submit' value='Submit'></form></body></html>");
}
////////////////////////////////WIFISET//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFISET//////////////////////////////////////////////////////////////////////////
const char *ssid = "BOTS";
const char *password = "password123";
void wifiset(){
  Serial.println("Configurando AP...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");Serial.println(myIP);
  server.on("/",HTTP_GET, webpage);
  server.on("/",HTTP_POST,response);
  server.begin();
  Serial.println("HTTP server started");
}
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
void wifiloop(){server.handleClient();}
