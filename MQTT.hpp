PubSubClient client(espClient);
extern int wificonectado;
char SERVER[50];
int SERVERPORT;
String USERNAME;   
char TOPIC[50];
char valueStr[15];
String strtopic = "";
unsigned long previousMillis = 0;
char PASSWORD[50];
char PLACA[50];
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////RECONNECT//////////////////////////////////////////////////////////////////////////
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
////////////////////////////////MQTTLOOP//////////////////////////////////////////////////////////////////////////
////////////////////////////////MQTTLOOP//////////////////////////////////////////////////////////////////////////
////////////////////////////////MQTTLOOP//////////////////////////////////////////////////////////////////////////
////////////////////////////////MQTTLOOP//////////////////////////////////////////////////////////////////////////
void mqttloop(){  
  client.setServer(SERVER, SERVERPORT);
  client.setCallback(callback);
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
}
