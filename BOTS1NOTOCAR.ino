#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h> 
int wificonectado=0;
WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);
char SERVER[50]   = "soldier.cloudmqtt.com";
int SERVERPORT   = 13223;
String USERNAME = "BOTS";   
char TOPIC[50];
char valueStr[15];
String strtopic = "";
unsigned long previousMillis = 0;
char PASSWORD[50] = "BOTS";     
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
char PLACA[50];
void reconnect() {
  uint8_t retries = 3;
  while (!client.connected()){// Loop hasta que estamos conectados
    Serial.print("Intentando conexion MQTT...");
    String clientId = "ESP8266Client-"+String(random(0xffff), HEX);// Crea un ID de cliente al azar
    USERNAME.toCharArray(PLACA, 50);
    if (client.connect("",PLACA, PASSWORD)) {// Attempt to connect
      Serial.println("conectado");
      client.subscribe(TOPIC);
    }else{
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intenta nuevamente en 5 segundos");
      // espera 5 segundos antes de reintentar
      delay(5000);
    }
    retries--;
    if(retries==0){while (1);}// esperar a que el WDT lo reinicie
  }
}
////////////////////////////////CALLBACK//////////////////////////////////////////////////////////////////////////
////////////////////////////////CALLBACK//////////////////////////////////////////////////////////////////////////
////////////////////////////////CALLBACK//////////////////////////////////////////////////////////////////////////
////////////////////////////////CALLBACK//////////////////////////////////////////////////////////////////////////
void callback(char* callbacktopic, byte* payload, unsigned int length) {
  String PAYLOAD = "";
  Serial.print("Mensaje Recibido: ["+String(callbacktopic)+"]: ");
  for (int i = 0; i < length; i++) {PAYLOAD += (char)payload[i];}
  Serial.println(PAYLOAD);
    if (PAYLOAD == "1I"){digitalWrite(LED_BUILTIN, LOW);}
    if (PAYLOAD == "1O"){digitalWrite(LED_BUILTIN, HIGH);}
}
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
void WiFiConnect(String ssidx,String passx){
  Serial.println("WiFiConnect");
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
      Serial.println("response");
      WiFiConnect(inString1,inString2);
    }
}
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
////////////////////////////////WEBPAGE//////////////////////////////////////////////////////////////////////////
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
void webpage(){
  Serial.println("webpage");
  server.send(200,"text/html","<html><body><form name='frm'  method='post'><input placeholder='ssid' type='text' name='ss'><br><input placeholder='pass' type='text' name='pass'><br><input type='submit' value='Submit'></form></body></html>");
}

///////////////////////////////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////SETUP///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *ssid = "BOTS";
const char *password = "password123";
void setup(){
  EEPROM.write(0,0);
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
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
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
  if(wificonectado==1){
    client.setServer(SERVER, SERVERPORT);
    client.setCallback(callback);
    String temp = USERNAME + "/" + "34324012"; 
    temp.toCharArray(TOPIC, 50);
    if (!client.connected()){reconnect();}
    client.loop();
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 10000){//envia la temperatura cada 10 segundos
      previousMillis = currentMillis;
      if(digitalRead(LED_BUILTIN)==LOW){strtopic = String("1I");}
      if(digitalRead(LED_BUILTIN)==HIGH){strtopic = String("1O");}
      strtopic.toCharArray(valueStr, 15);
      Serial.println("Enviando: [" +  String(TOPIC) + "] " + strtopic);
      client.publish(TOPIC, valueStr);
    }
    }else if(wificonectado==0){server.handleClient();}
}
