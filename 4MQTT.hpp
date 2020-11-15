WiFiClient WiFiclient;
PubSubClient client(WiFiclient);
ESP8266WebServer WiFiserver(80);
char SERVER[50];char TOPIC[50];char PASSWORD[50];char PLACA[50];String USERNAME; int SERVERPORT;
char actuadorid[100];char MQTTCALLBACK[100];
extern bool MQTTCONNECTED;
extern bool programasset;
String actuadores[][15]={{},{}};
extern int qa;
/*----------------------------------------------------------------------------------------------------------------------RECONNECT--------------------------------*/
void suscriber(){
  char temp[50];
  char temp2[50];
  for(int i=0;i<qa;i++){
    strcpy(temp,TOPIC);
    strcat(temp,"/");
    actuadores[i][0].toCharArray(temp2,actuadores[i][0].length()+1);
    strcat(temp,temp2);
    client.subscribe(temp);
  }
    strcpy(temp,TOPIC);
    strcat(temp,"/0");
  client.subscribe(temp);
}
void reconnect() {
  int retries = 0;
  if(!client.connected() && retries <2){
    Serial.print("Intentando conexion MQTT...");
    String clientId = "ESP8266Client-"+String(random(0xffff), HEX);
    USERNAME.toCharArray(PLACA,USERNAME.length()+1);
    if(client.connect("",PLACA,PASSWORD)){Serial.println("Conectado");suscriber();}
    else{Serial.println("Fallo, rc=" + String(client.state())+" Intenta nuevamente en 3 segundos");delay(3000);}
    retries++;
  }
}
/*----------------------------------------------------------------------------------------------------------------------PUBLISH--------------------------------*/
String mqttpublish(String msgTOPIC,String strtopic){
  char valueStr[30];String ret;
  strtopic.toCharArray(valueStr, strtopic.length()+1);
  char temp[50];strcpy(temp,TOPIC);char temp2[50];msgTOPIC.toCharArray(temp2,msgTOPIC.length()+1);strcat(temp,temp2);
  if(client.publish(temp, (uint8_t*)valueStr,strlen(valueStr),true)){/*Serial.println("Enviando: [" +  String(TOPIC) + "] "+ String(strtopic));*/ret="1";}
  else{ret="0";}
  return ret;
}
/*----------------------------------------------------------------------------------------------------------------------onMessageArrived-------------------------*/
int RFReceive;
void onMessageArrived(char* callbacktopic, byte* payload, unsigned int length) {
  String PAYLOAD = "";
  for (int i = 0; i < length; i++) {PAYLOAD += (char)payload[i];}
  PAYLOAD.toCharArray(MQTTCALLBACK,PAYLOAD.length()+1);
  Serial.print("Mensaje Recibido: ["+String(callbacktopic)+"]: ");Serial.println(MQTTCALLBACK);
  if(strcmp(MQTTCALLBACK, "ProgramsChange")==0){programasset=true;}
  String * args2 = substr(callbacktopic,"/");
  args2[2].toCharArray(actuadorid,args2[2].length()+1);
  if(strcmp(actuadorid,"1")==0){char RFCode[50];strcpy(RFCode,"0100111000110001000000011000");/*EmisRF(RFCode);*/}
} 
/*----------------------------------------------------------------------------------------------------------------------SETUP & HANDLE---------------------------*/
void mqttclientsetup(char INSERVER[50],int INSERVERPORT,String INUSERNAME,char INPASSWORD[50],char INTOPIC[50]){
  strcpy(SERVER,INSERVER);
  SERVERPORT=INSERVERPORT;
  USERNAME=INUSERNAME;
  strcpy(PASSWORD,INPASSWORD);
  strcpy(TOPIC,INTOPIC);
  client.setServer(SERVER, SERVERPORT);
  client.setCallback(onMessageArrived);
  MQTTCONNECTED=true;
}
void mqttclienthandle(){if(!client.connected()){reconnect();}client.loop();}
