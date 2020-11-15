#include <string.h>
#define DATA_OFFSET 10
ESP8266WebServer server(80);
WiFiClient espClient;
int WIFIDIR = 0;int estado=0;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
////////////////////////////////XMLEND//////////////////////////////////////////////////////////////////////////
////////////////////////////////XMLEND//////////////////////////////////////////////////////////////////////////
void xmlend(String xmlstring){/*SE REPITE EN TODOS LAS FUNCIONES XML*/
  String XML ="<?xml version='1.0'?><data id='respxml'>";XML+=xmlstring;XML+="</data>";
  server.send(200,"text/xml",XML);
  }
////////////////////////////////PLAINSUBSTRING//////////////////////////////////////////////////////////////////////////
////////////////////////////////PLAINSUBSTRING//////////////////////////////////////////////////////////////////////////
int q;
String * plainsubstring(String fullstring){/*DIVIDE STRING RECIBIDO DE POST EN SUBSTRING EN FUNCION DE UN CARACTER DETERMINADO*/
  int pos[20];int wexit=0;String next[20];String * postarg=new String[20];q=0;String sub[20];
  next[0]=fullstring;
  do{
    pos[q]=next[q].indexOf("?",q);
    //Serial.print("pos[");Serial.print(q);Serial.print("]: ");Serial.println(pos[q]);
    if(pos[q]!=-1){
      sub[q]=next[q].substring(0,pos[q]);
      if((sub[q].indexOf(":",0))>0){postarg[q]=sub[q].substring(sub[q].indexOf(":",0)+1,sub[q].length());}
      else{postarg[q]=sub[q];}
      next[q+1]=next[q].substring(pos[q]+1,next[q].length());
      //Serial.println("sub["+String(q)+"]: "+String(sub[q]);
      //Serial.println("next["+String(q+1)+"]: "+String(next[q+1]);
      //Serial.println("postarg["+String(q)+"]: "+String(postarg[q]);
      q++;
    }else{wexit=1;}   
  }while(wexit==0);
  return postarg;
}
////////////////////////////////HTMLXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////HTMLXML//////////////////////////////////////////////////////////////////////////
void htmlsendxml(){
  //POST= strtopic:strtopic?
  String * args2= plainsubstring(server.arg("plain"));
  String xmlresponse = mqttpublish(args2[0]);
  //Serial.println("htmlsendxml: "+String(xmlresponse));
  xmlend(xmlresponse);
//  RFloop();
}
void htmlreceivexml(){
  String strtopic;
  if(digitalRead(LED_BUILTIN)==LOW){strtopic ="1I";}
  if(digitalRead(LED_BUILTIN)==HIGH){strtopic = "1O";}
  String xmlresponse = strtopic;
  //Serial.println("htmlreceivexml: "+String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////MQTTSETUP//////////////////////////////////////////////////////////////////////////
////////////////////////////////MQTTSETUP//////////////////////////////////////////////////////////////////////////
extern char SERVER[50];extern char TOPIC[50];extern char PASSWORD[50];extern String USERNAME;extern int SERVERPORT;
void mqttsetup(){ 
  String * args2= plainsubstring(phpmqttinput());
  args2[0].toCharArray(SERVER, 50);Serial.println("SERVER: "+String(SERVER));
  SERVERPORT=args2[1].toInt();Serial.println("SERVERPORT: "+String(SERVERPORT));
  USERNAME = args2[3];Serial.println("USERNAME: "+String(USERNAME));
  args2[4].toCharArray(PASSWORD, 50);Serial.println("PASSWORD: "+String(PASSWORD));
  args2[5].toCharArray(TOPIC, 50);Serial.println("TOPIC: "+String(TOPIC));
  estado=400;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  //RFsetup();
}
////////////////////////////////PASSWORDXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////PASSWORDXML//////////////////////////////////////////////////////////////////////////
void passwordxml(){
  //POST= oldpassword:oldpassword?newpassword1:newpassword1?newpassword2:newpassword2
  String * args2= plainsubstring(server.arg("plain"));
  String xmlresponse = phpchangepassword(args2[0],args2[1],args2[2]);
  Serial.println("passwordxml: "+String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////CLIENTES//////////////////////////////////////////////////////////////////////////
////////////////////////////////CLIENTES//////////////////////////////////////////////////////////////////////////
void htmlclientesxml(){ 
  //VALUE1?VALUE2?VALUE3?
  //String * args2= plainsubstring(phpclientes());
  String xmlresponse = phpclientes();
  Serial.println("htmlclientesxml: "+String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////BOTSLOGINXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////BOTSLOGINXML//////////////////////////////////////////////////////////////////////////
void botsloginxml(){
  //POST=BOTSUSER:BOTSUSER?BOTSPASS:BOTSPASS?
  String * args2= plainsubstring(server.arg("plain"));
  String xmlresponse = phpbotslogin(args2[0],args2[1]);
  if(xmlresponse=="1"){
    estado=200;
    mqttsetup();
  }//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  Serial.println("botsloginxml: "+String(xmlresponse));
  xmlend(xmlresponse);  
}
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
String WiFiConnect(String STAssid,String STApassword){
  int contconexion = 0;
  WiFi.begin(STAssid, STApassword);
  while (WiFi.status() != WL_CONNECTED && contconexion < 20){contconexion++;delay(500);Serial.print(".");}
  Serial.print("STA IP: ");Serial.println(WiFi.localIP());
  if (WiFi.status() == WL_CONNECTED){    
    //writeString(STAssid,WIFIDIR);
    //writeString(STApassword,WIFIDIR+STAssid.length()+DATA_OFFSET);
    //String eepromssid = readString(WIFIDIR);
    //String eeprompass = readString(WIFIDIR+eepromssid.length()+DATA_OFFSET);
    //Serial.println("EEPROM GUARDADO--> SSID: "+eepromssid+" / PASS: "+eeprompass);
    return "1"; 
  }else{return "Error de conexion";}
}
////////////////////////////////WIFICONFIGXML//////////////////////////////////////////////////////////////////////////
void wificonfigxml(){
  //ESP.eraseConfig(); //se cae ap ??
  WiFi.disconnect(true);
  //POST=STAssid:STAssid?STApassword:STApassword?;
  Serial.println("\n\nxxx\n\n");
  String * args2 = plainsubstring(server.arg("plain"));
  Serial.print("SSID: ");Serial.print(args2[0]);Serial.print(" / PASS: ");Serial.println(args2[1]);
  Serial.println("\n\nyyyyyyyyy\n\n");
  String xmlresponse = WiFiConnect(args2[0],args2[1]);
  if(xmlresponse=="1"){estado=100;}//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  Serial.println("wificonfigxml: "+String(xmlresponse));
  xmlend(xmlresponse);  
}
////////////////////////////////SERVERHANDLER//////////////////////////////////////////////////////////////////////////
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
void botslogin(){server.send(200, "text/html", LOGIN);}
void user(){server.send(200, "text/html", USER);}
void landing(){//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  switch(estado){
    case 0:{server.send(200, "text/html", LANDING);break;}
    case 100:{botslogin();break;}
    case 200:{user();break;}
    case 300:{user();break;}
    case 400:{user();break;}
    default:{server.send(200, "text/html", LANDING);break;}
  }
}
////////////////////////////////SESSIONOUT//////////////////////////////////////////////////////////////////////////
void sessionout(){estado=100;landing();}
////////////////////////////////WIFICONFIG//////////////////////////////////////////////////////////////////////////
void wificonfig(){
  String APssid = "BOTS-"+String(WiFi.macAddress());
  String APpassword= String(WiFi.macAddress());
  WiFi.disconnect(true);
  ESP.eraseConfig();  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/",landing);server.on("/wificonfigxml", wificonfigxml);
  server.on("/botslogin",botslogin);server.on("/botsloginxml", botsloginxml);
  server.on("/user",user);server.on("/htmlclientesxml",htmlclientesxml);
  server.on("/htmlsendxml",htmlsendxml);server.on("/htmlreceivexml",htmlreceivexml);
  server.on("/sessionout",sessionout);
  server.on("/passwordxml",passwordxml);
  server.begin();
  Serial.println("AP CONFIG --> SSID: "+String(APssid)+" / PASS: "+String(APpassword));
  Serial.print("AP IP: ");Serial.println(myIP);
}
////////////////////////////////RESTARTCONFIG//////////////////////////////////////////////////////////////////////////
void restartconfig(){/*REINICIAR ARDUINO CON CONFIGURACIONES DE EEPROM*/
  if(readString(WIFIDIR).length()>0){
    String eepromssid= readString(WIFIDIR);
    String eeprompass= readString(WIFIDIR+eepromssid.length()+DATA_OFFSET);
    Serial.println("EEPROM GET--> SSID: "+String(eepromssid)+" / PASS: "+String(eeprompass));
    if(eepromssid!="" && eeprompass!=""){
      String temp = WiFiConnect(eepromssid,eeprompass);
      if (temp=="1"){
        estado=100;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
        Serial.println("wificonfig: "+String(temp));
        if(readString(BOTSDIR).length()>0){
          String eeprombotsuser=readString(BOTSDIR);
          String eeprombotspassword=readString(BOTSDIR+DATA_OFFSET+eeprombotsuser.length());
          Serial.println("EEPROM BOTS-->USER: "+String(eeprombotsuser)+" / PASS: "+String(eeprombotspassword));
          temp = phpbotslogin(eeprombotsuser,eeprombotspassword);
          if (temp=="1"){
            Serial.println("botslogin: "+String(temp));
            estado=200;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
            mqttsetup();
          }
        }
      }else{
        Serial.println("Configuracion WIFI EEPROM Fallada. Reiniciando Red AP...");
        wificonfig();
      }
    }
  }
}
////////////////////////////////WIFISETUP//////////////////////////////////////////////////////////////////////////
void wifisetup(){
  wificonfig();
  //restartconfig();
}
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
void wifiloop(){server.handleClient();}
