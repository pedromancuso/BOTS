#include <string.h>
ESP8266WebServer server(80);
WiFiClient espClient;
#define DATA_OFFSET 10
int WIFIDIR = 0;
extern String clientespayload;
String eepromssid;String eeprompass;String XML;String xmlresponse;
String eeprombotsuser;String eeprombotspassword;
int estado=0;//0:UNSET-200:WIFIOK-201:PHPSERVEROK-202:BOTSUSEROK-400:WIFIERROR-401:PHPSERVERERROR
const char espserver[] = "casabots.000webhostapp.com";
////////////////////////////////XML//////////////////////////////////////////////////////////////////////////
////////////////////////////////XML//////////////////////////////////////////////////////////////////////////
void XMLcontent(){
  int q=0;int pos[20];String sub[20];String next[20];
  next[0]=server.arg("plain");int wexit=0;
  do{
    pos[q]=next[q].indexOf("?",q);
    //Serial.print("pos[");Serial.print(q);Serial.print("]: ");Serial.println(pos[q]);
    if(pos[q]!=-1){
      sub[q]=next[q].substring(0,pos[q]);
      next[q+1]=next[q].substring(pos[q]+1,next[q].length());
      //Serial.print("sub[");Serial.print(q);Serial.print("]: ");Serial.println(sub[q]);
      //Serial.print("next[");Serial.print(q+1);Serial.print("]: ");Serial.println(next[q+1]);
      q++;
    }else{wexit=1;}   
  }while(wexit==0);
  String oldpass=sub[0].substring(sub[0].indexOf(":",0)+1,sub[0].length());
  String newpass1=sub[1].substring(sub[1].indexOf(":",0)+1,sub[1].length());
  String newpass2=sub[2].substring(sub[2].indexOf(":",0)+1,sub[2].length());
  changepassword(oldpass,newpass1,newpass2);
  XML ="<?xml version='1.0'?><data id='respxml'>";XML+=xmlresponse;XML+="</data>";
  server.send(200,"text/xml",XML);
  }
////////////////////////////////LOGIN//////////////////////////////////////////////////////////////////////////
////////////////////////////////LOGIN//////////////////////////////////////////////////////////////////////////
void login(){
  server.send(200, "text/html", TEMP);
  String phpresponse;
  if (server.hasArg("BOTSUSER") && server.hasArg("BOTSPASS")){
    phpresponse= botslogin(server.arg("BOTSUSER"),server.arg("BOTSPASS"));
  }else{
    phpresponse= botslogin(eeprombotsuser,eeprombotspassword);
  }
  //Serial.print("LoginPayload: ");Serial.println(phpresponse);//Print request response payload
}
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
void WiFiConnect(String STAssid,String STApassword){
  int contconexion = 0;
  WiFi.begin(STAssid, STApassword);
  while (WiFi.status() != WL_CONNECTED && contconexion < 20){contconexion++;delay(500);Serial.print(".");}
  Serial.print("STA IP: "); Serial.println(WiFi.localIP());
  if (WiFi.status() == WL_CONNECTED){
    estado = 200;//0:UNSET-200:WIFISTARTEDOK-201:PHPSERVEROK-202:BOTSUSEROK-400:WIFIERROR-401:PHPSERVERERROR
    writeString(STAssid,WIFIDIR);
    writeString(STApassword,WIFIDIR+STAssid.length()+DATA_OFFSET);
    eepromssid = readString(WIFIDIR);
    eeprompass = readString(WIFIDIR+eepromssid.length()+DATA_OFFSET);
    Serial.print("EEPROM GUARDADO--> SSID: ");Serial.print(eepromssid);Serial.print(" / PASS: ");Serial.println(eeprompass);
    if(readString(BOTSDIR).length()>0){
      eeprombotsuser=readString(BOTSDIR);
      eeprombotspassword=readString(BOTSDIR+DATA_OFFSET+eeprombotsuser.length());
      Serial.print("EEPROM BOTS-->USER: ");Serial.print(eeprombotsuser);Serial.print(" / PASS: ");Serial.println(eeprombotspassword);
      login();
    }
  }else{
    estado = 400;//0:UNSET-200:WIFISTARTEDOK-201:PHPSERVEROK-202:BOTSUSEROK-400:WIFIERROR-401:PHPSERVERERROR
    Serial.println("Error de conexion, EEPROM NO GUARDADA");
  }
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
void webpage(){
  String DEPLOY;
  switch (estado){//0:UNSET-200:WIFISTARTEDOK-201:PHPSERVEROK-202:BOTSUSEROK-400:WIFIERROR-401:PHPSERVERERROR
    case 0:{DEPLOY=LANDING1;DEPLOY +=LANDING2;break;}
    case 200:{DEPLOY=LOGIN1;DEPLOY+=LOGIN2;break;}
    case 201:{DEPLOY=LOGIN1;DEPLOY+="<div id='estadowifi'>"+phppayload+"</div>";DEPLOY+=LOGIN2;break;}
    case 202:{
      DEPLOY=USER1;
      int q=0;int pos[20];String sub[20];String next[20];
      //String response=clientes();
      next[0]=clientespayload;int wexit=0;
      do{
        pos[q]=next[q].indexOf("?",q);
        //Serial.print("pos[");Serial.print(q);Serial.print("]: ");Serial.println(pos[q]);
        if(pos[q]!=-1){
          sub[q]=next[q].substring(0,pos[q]);
          next[q+1]=next[q].substring(pos[q]+1,next[q].length());
          //Serial.print("sub[");Serial.print(q);Serial.print("]: ");Serial.println(sub[q]);
          //Serial.print("next[");Serial.print(q+1);Serial.print("]: ");Serial.println(next[q+1]);
          q++;
        }else{wexit=1;}   
      }while(wexit==0);
      for (int q2 = 0; q2 < q; q2++) {
        int q3=q2+1;
        DEPLOY+="<div><p class='option'>"+sub[q2]+":</p><label id='label"+q3+"' class='switch'><input id='check"+q3+"' type='checkbox' class='optioninput'>";
        DEPLOY+="<span class='slider round'></span></label><span id='estado";
        DEPLOY+=q3;
        DEPLOY+="' class='estado'></span></div>";
      }
      DEPLOY+=USER2;
      break;}
    case 400:{DEPLOY=LANDING1;DEPLOY+="<div id='estadowifi'>WiFi No Conectado</div>";DEPLOY+=LANDING2;break;}
    default:{DEPLOY=LANDING1;DEPLOY+="<div id='estadowifi'>WiFi No Conectado</div>";DEPLOY+=LANDING2;break;}
  }
  server.send(200, "text/html", DEPLOY);
}
////////////////////////////////SESSIONOUT//////////////////////////////////////////////////////////////////////////
////////////////////////////////SESSIONOUT//////////////////////////////////////////////////////////////////////////
void sessionout(){estado=200;webpage();}
////////////////////////////////WIFISETUP//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFISETUP//////////////////////////////////////////////////////////////////////////
const char *APssid = "BOTS";
const char *APpassword = "password123";
void wifisetup(){
  ESP.eraseConfig();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", webpage);
  server.on("/wificonfig", response);
  server.on("/login", login);
  server.on("/sessionout", sessionout);
  server.on("/xmlpeter",XMLcontent);
  server.begin();
  Serial.print("AP CONFIG --> SSID: "); Serial.print(APssid); Serial.print(" / PASS: "); Serial.println(APpassword);
  Serial.print("AP IP: "); Serial.println(myIP);
  if(readString(WIFIDIR).length()>0){
    eepromssid= readString(WIFIDIR);
    eeprompass= readString(WIFIDIR+eepromssid.length()+DATA_OFFSET);
    Serial.print("EEPROM GET--> SSID: ");Serial.print(eepromssid);Serial.print(" / PASS: ");Serial.println(eeprompass);
    if(eepromssid!="" && eeprompass!=""){
      //WiFiConnect(eepromssid,eeprompass);
      /*if (WiFi.status() != WL_CONNECTED){writeString("",EEDIR);wifiset();}*/
    }
  }
}
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
void wifiloop(){server.handleClient();}
