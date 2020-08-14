#define DATA_OFFSET 10
WiFiServer phpserver(8080);
ESP8266WebServer server(80);
byte WIFIDIR = 0; int estado=0;byte BOTSDIR=50;
////////////////////////////////XMLEND//////////////////////////////////////////////////////////////////////////
////////////////////////////////XMLEND//////////////////////////////////////////////////////////////////////////
void xmlend(char xmlchar[]) { /*SE REPITE EN TODOS LAS FUNCIONES XML*/
  String xmlstring;
  for (int i = 0; i < strlen(xmlchar) ; i++) {xmlstring += xmlchar[i];}
  String XML = "<?xml version='1.0'?><data id='respxml'>"; XML += xmlstring; XML += "</data>";
  server.send(200, "text/xml", XML);
}
////////////////////////////////PLAINSUBSTRING//////////////////////////////////////////////////////////////////////////
////////////////////////////////PLAINSUBSTRING//////////////////////////////////////////////////////////////////////////
String * plainsubstring(String fullstring) { /*DIVIDE STRING RECIBIDO DE POST EN SUBSTRING EN FUNCION DE UN CARACTER DETERMINADO*/
  int pos[20]; int wexit = 0; String next[20]; String * postarg = new String[20];int q= 0; String sub[20];
  next[0] = fullstring;
  do{
    pos[q] = next[q].indexOf("?", q);
    if (pos[q] != -1) {sub[q] = next[q].substring(0, pos[q]);
    if ((sub[q].indexOf(":", 0)) > 0) {postarg[q] = sub[q].substring(sub[q].indexOf(":", 0) + 1, sub[q].length());}else{postarg[q] = sub[q];}
    next[q + 1] = next[q].substring(pos[q] + 1, next[q].length());
    q++;
    }else{wexit = 1;}
  }while(wexit == 0);
  return postarg;
}
/////////////////////////PHPREQUEST/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////PHPREQUEST/////////////////////////////////////////////////////////////////////////////////////////////////////
char * phprequest(String Header,String data){
  char phpresponse[600];
  char espserver[] = "http://casabots.000webhostapp.com";
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;
    http.begin(espserver+Header);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  
    byte httpCode = http.POST(data);
    String phppayload = http.getString();
    phppayload.toCharArray(phpresponse,phppayload.length()+1);
    Serial.println(String(Header)+" HttpCode: "+String(httpCode));
    http.end();
  }else{strcpy(phpresponse,"Error de Conexion con Servidor PHP. Contacte Proveedor.");}
  return phpresponse;
}
////////////////////////////////HTMLXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////HTMLXML//////////////////////////////////////////////////////////////////////////
void htmlsendxml() {
  //POST= strtopic:strtopic?
  char xmlresponse[150];
  String * args2 = plainsubstring(server.arg("plain"));
  String temp = mqttpublish(args2[0]);
  temp.toCharArray(xmlresponse,temp.length()+1);
  xmlend(xmlresponse);
}
void htmlreceivexml(){xmlend(MQTTCALLBACK);}
////////////////////////////////PENDIENTE//////////////////////////////////////////////////////////////////////////
////////////////////////////////PENDIENTE//////////////////////////////////////////////////////////////////////////
void htmlprogamaeliminarxml(){
  //POST= progamaeliminar:progamaeliminar
  String * args2 = plainsubstring(server.arg("plain"));
  String data = "USER="+String(readChar(BOTSDIR))+"&progamaeliminar="+String(args2[0])+"";
  char xmlresponse[150]; strcpy(xmlresponse, phprequest("/progamaeliminar.php",data));
  Serial.println("Eliminar Programa: " + String(xmlresponse));
  xmlend(xmlresponse);
}
void htmlnuevoprogramaxml(){
  //POST= estado:estado,programid:programid,nombreprograma:nombreprograma,days:days,horainicio:horainicio,horafin:horafin,devi:devi
  String * args2 = plainsubstring(server.arg("plain"));
  String data = "USER="+String(readChar(BOTSDIR))+"&estado="+String(args2[0])+"&programid="+String(args2[1])+"&nombreprograma="+String(args2[2])+"&days="+String(args2[3])+"&horainicio="+String(args2[4])+"&horafin="+String(args2[5])+"&devi="+String(args2[6])+"";
  char xmlresponse[150]; strcpy(xmlresponse, phprequest("/nuevoprograma.php",data));
  Serial.println("Nuevo Programa: " + String(xmlresponse));
  xmlend(xmlresponse); 
}
void htmlmodificarestadoprogramaxml(){
  //POST= ID:ID,ESTADO:ESTADO
  String * args2 = plainsubstring(server.arg("plain"));
  String data = "USER="+String(readChar(BOTSDIR))+"&ID="+String(args2[0])+"&ESTADO="+String(args2[1])+"";
  char xmlresponse[150]; strcpy(xmlresponse, phprequest("/modificarestadoprograma.php",data));
  Serial.println("Estado Programa: " + String(xmlresponse));
  xmlend(xmlresponse);
}
void htmlprogramasclientesxml() {
  //VALUE1?VALUE2?VALUE3?
  char data[50];strcpy(data,"USER=");strcat(data,readChar(BOTSDIR));
  char xmlresponse[200]; strcpy(xmlresponse, phprequest("/programasclientes.php",data));
  xmlend(xmlresponse);
}
////////////////////////////////MQTTSETUP//////////////////////////////////////////////////////////////////////////
////////////////////////////////MQTTSETUP//////////////////////////////////////////////////////////////////////////
extern char SERVER[50]; extern char TOPIC[50]; extern char PASSWORD[50]; extern String USERNAME; extern int SERVERPORT;
void mqttsetup(){
  char data[50];strcpy(data,"mqttuser=");strcat(data,readChar(BOTSDIR));
  String * args2 = plainsubstring(phprequest("/mqttuser.php",data));
  args2[0].toCharArray(SERVER, 50); Serial.println("SERVER: " + String(SERVER));
  SERVERPORT = args2[1].toInt(); Serial.println("SERVERPORT: " + String(SERVERPORT));
  USERNAME = args2[3]; Serial.println("USERNAME: " + String(USERNAME));
  args2[4].toCharArray(PASSWORD, 50); Serial.println("PASSWORD: " + String(PASSWORD));
  args2[5].toCharArray(TOPIC, 50); Serial.println("TOPIC: " + String(TOPIC));
  mqttclientsetup();
  estado=400;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
}
////////////////////////////////RECUPASSXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////RECUPASSXML//////////////////////////////////////////////////////////////////////////
void recupassxml(){
  //POST= user:user?email:email?
  String * args2 = plainsubstring(server.arg("plain"));
  String data = "user="+String(args2[0])+"&email="+String(args2[1])+"";  
  char xmlresponse[150]; strcpy(xmlresponse, phprequest("/recupass.php",data));
  Serial.println("recupassxml: " + String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////PASSWORDXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////PASSWORDXML//////////////////////////////////////////////////////////////////////////
void passwordxml(){
  //POST= oldpassword:oldpassword?newpassword1:newpassword1?newpassword2:newpassword2
  String * args2 = plainsubstring(server.arg("plain"));
  String data = "USER="+String(readChar(BOTSDIR))+"&oldpassword="+String(args2[0])+"&newpassword1="+String(args2[1])+"&newpassword2="+String(args2[2])+"";
  char xmlresponse[150]; strcpy(xmlresponse, phprequest("/changepassword.php",data));
  if(strcmp(xmlresponse,"Contraseña Modificada Correctamente.")==0){strcpy(xmlresponse,"1");writeString(args2[1],BOTSDIR+DATA_OFFSET+strlen(readChar(BOTSDIR)));}
  Serial.println("passwordxml: " + String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////CLIENTESXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////CLIENTESXML//////////////////////////////////////////////////////////////////////////
void htmlclientesxml() {
  //VALUE1?VALUE2?VALUE3?
  char data[50];strcpy(data,"USER=");strcat(data,readChar(BOTSDIR));
  char xmlresponse[200]; strcpy(xmlresponse, phprequest("/clientes.php",data));
  xmlend(xmlresponse);
}
////////////////////////////////BOTSLOGINXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////BOTSLOGINXML//////////////////////////////////////////////////////////////////////////
void botsloginxml() {
  //POST=BOTSUSER:BOTSUSER?BOTSPASS:BOTSPASS?
  String * args2 = plainsubstring(server.arg("plain"));
  String data = "user="+String(args2[0])+"&pass="+String(args2[1])+"";
  char xmlresponse[200]; strcpy(xmlresponse, phprequest("/login.php",data));
  if(strcmp(xmlresponse,"1")==0){
    writeString(args2[0],BOTSDIR);
    writeString(args2[1],BOTSDIR+DATA_OFFSET+args2[0].length());
    estado=200;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
    mqttsetup();
    Serial.println("Credenciales guardadas en EEPROM.");
  }
  Serial.println("botsloginxml: " + String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
////////////////////////////////WIFICONNECT//////////////////////////////////////////////////////////////////////////
char * WiFiConnect(String STAssid, String STApassword) {
  char response[50];
  byte contconexion = 0;
  WiFi.begin(STAssid, STApassword);
  while (WiFi.status() != WL_CONNECTED && contconexion < 20) {contconexion++;delay(500);Serial.print(".");}
  Serial.print("STA IP: "); Serial.println(WiFi.localIP());
  if (WiFi.status() == WL_CONNECTED) {
    writeString(STAssid, WIFIDIR);
    writeString(STApassword, WIFIDIR + STAssid.length() + DATA_OFFSET);
    char eepromssid[30]; strcpy(eepromssid, readChar(WIFIDIR));
    byte WIFIPASS = WIFIDIR + strlen(eepromssid) + DATA_OFFSET;
    char eeprompass[30]; strcpy(eeprompass, readChar(WIFIPASS));
    Serial.println("EEPROM GUARDADO--> SSID: " + String(eepromssid) + " / PASS: " + String(eeprompass));
    strcpy(response, "1");
  }else{strcpy(response, "Error de conexion");}
  return response;
}
////////////////////////////////WIFICONFIGXML//////////////////////////////////////////////////////////////////////////
void wificonfigxml(){
  WiFi.disconnect(true);
  //POST=STAssid:STAssid?STApassword:STApassword?;
  String * args2 = plainsubstring(server.arg("plain"));
  Serial.print("SSID: "); Serial.print(args2[0]); Serial.print(" / PASS: "); Serial.println(args2[1]);
  char xmlresponse[50]; strcpy(xmlresponse, WiFiConnect(args2[0], args2[1]));
  if (xmlresponse == "1") {estado=100;}/*0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK*/
  Serial.println("wificonfigxml: " + String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////SERVERHANDLER//////////////////////////////////////////////////////////////////////////
////////////////////////////////SERVERHANDLER//////////////////////////////////////////////////////////////////////////
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
#define BOTSSTYLE "*{font-family:'Raleway', Helvetica, sans-serif;}.landing,.login{background-color:#1E2832;text-align: center;display: inline-block !important;width: 100%;color:#fff;}#banner{padding:5em 2em 3em 2em;background-color:#1E2832;text-align:center;}.logtitle{font-size:1.5em;padding:0;color:#fff;}/*h2{font-size:1.5em;padding:0;line-height: 1.5em;}h2{color: #25383B;line-height: 1.5em;font-weight: 700;margin: 0 0 1em 0;text-transform: uppercase;}*/.actions{padding: 0;width:100%;display: inline-block;}.inputland{border-radius: 4px;border-style: solid;border-width: 1px;display: block;outline: 0;padding: 0 1em;text-decoration: none;line-height: 2em;width: 100%;background-color: rgba(144, 144, 144, 0.075);border-color: rgba(144, 144, 144, 0.25);height: 3.5em;font-size: 12pt;font-weight: 400;}.alt,.special{max-width: 49%;min-width:45%;background-color: #51BAA4;transition: background-color 0.2s ease-in-out, color 0.2s ease-in-out;border-radius: 2.5em;border: 0;cursor: pointer;display: inline-block;font-weight: 700;height: 2.85em;line-height: 2.95em;width: 49%;padding: 0 1.5em;text-align: center;text-decoration: none;text-transform: uppercase;white-space: nowrap;}.alt{background-color: transparent;box-shadow: inset 0 0 0 2px #51BAA4;color: #51BAA4 !important;}.marg{margin: 1em 0em;}.marg2{margin: 2em 0;}.userbody{text-align: center;display: inline-block !important;width: 100%;margin: 0;background-color: rgb(249, 252, 255);min-width:200px;color: #777}#header{min-width: 200px !important;height: 44px;line-height: 44px;background: #07090c;color: rgba(255, 255, 255, 0.75);cursor: default;text-align: right;width: 100%;height: auto;}h1{left: 1em;color: #fff;display: inline-block;height: inherit;left: 1.25em;line-height: inherit;margin: 0;padding: 0;position: absolute;top: 0;text-decoration: none;font-size: 1em;}a{font-size: 1em;-moz-transition: color 0.2s ease-in-out;-webkit-transition: color 0.2s ease-in-out;-ms-transition: color 0.2s ease-in-out;transition: color 0.2s ease-in-out;color: #fff;display: inline-block;margin-right: 1.25em;text-decoration: none;}#header a[href='#nav'] {text-decoration: none;-webkit-tap-highlight-color: transparent;}#header a[href='#nav']:before {margin: 0 0.5em 0 0;-moz-osx-font-smoothing: grayscale;-webkit-font-smoothing: antialiased;font-family: FontAwesome;font-style: normal;font-weight: normal;text-transform: none !important;}#header a + a[href='#nav']:last-child {border-left: solid 1px transparent;padding-left: 1.25em;margin-left: 0.5em;}header.major h2 {font-size: 1.5em;}header.major{margin-bottom: 2em;}header.major p {font-size: 1em;}header h2 + p {font-size: 1.25em;margin-top: -1em;line-height: 1.85em;}header p {color: #25383B;margin: 0 0 1.5em 0;position: relative;text-transform: uppercase;}footer{position:absolute;bottom:0;text-decoration:none;width:100%;background-color:#1E2832;color:rgba(255, 255, 255, 0.75);text-align:center;}ul{list-style: none;padding-left: 1em;}ul li {padding-left: 0.5em;}#nav{transition: transform 0.5s ease, box-shadow 0.5s ease, visibility 0.5s;padding: 2.5em 1.75em;background: #07090c;color: #fff;height: 100%;max-width: 80%;overflow-y: auto;position: fixed;right: 0;text-transform: uppercase;top: 0;width: 20em;z-index: 10002;}#nav > ul.links {text-align: left;}#nav > ul {margin: 0 0 1em 0;}#nav > ul.links > li {padding: 0;}#nav .close {height: 4em;line-height: 4em;text-decoration: none;transition: color 0.2s ease-in-out;border: 0;color: #a5a9ad;cursor: pointer;display: block;padding-right: 1.25em;position: absolute;right: 0;text-align: right;top: 0;vertical-align: middle;width: 7em;}#nav > ul.links > li > a:not(.button) {border: 0;border-top: solid 1px transparent;color: inherit;line-height: 3.5em;text-decoration: none;}#main{padding: 2em 0 0.1em 0;}.wrapper{position: relative;}.container{margin-left: auto;margin-right: auto;width: 90% !important;}table{margin: 0 0 2em 0;width: 100%;border-collapse: collapse;border-spacing: 0;display: table;}tbody {display: table-row-group;}table td{padding: 0.75em 0.75em;}table tbody tr:nth-child(2n + 1) {background-color: rgba(144, 144, 144, 0.075);}table tbody tr {border-color: rgba(144, 144, 144, 0.25);border-style: solid;border-width: 1px;border-left: 0;border-right: 0;display: table-row;}input[type='checkbox']{-moz-appearance: none;-webkit-appearance: none;    -ms-appearance: none;    appearance: none;    display: block;    float: left;    margin-right: -2em;    opacity: 0;    width: 1em;    z-index: -1;}input[type='checkbox'] + label{    text-decoration: none;    color: #777;    cursor: pointer;    display: inline-block;    font-size: 1em;    font-weight: 400;    padding-left: 2.85em;    padding-right: 0.75em;    position: relative;}input[type='checkbox']:checked + label:before{    background-color: rgba(144, 144, 144, 0.075);    border-color: #51BAA4;    color: #25383B;    content: '\2714';}input[type='checkbox'] + label:before{    border-radius: 4px;    border-style: solid;    border-width: 1px;    content: '';    display: inline-block;    height: 2.1em;    left: 0;    line-height: 2.0125em;    position: absolute;    text-align: center;    top: 0;    width: 2.1em;    -moz-osx-font-smoothing: grayscale;    -webkit-font-smoothing: antialiased;    font-family: FontAwesome;    font-style: normal;    font-weight: normal;    text-transform: none !important;}input[type='password']{background-color: rgba(144, 144, 144, 0.075);border-color: rgba(144, 144, 144, 0.25);color: inherit;height: 3.5em;border-radius: 4px;border-style: solid;border-width: 1px;display: block;outline: 0;padding: 0 1em;text-decoration: none;width: 100%;}input[type='text'], input[type='password'], input[type='email'], select, textarea {background-color: rgba(144, 144, 144, 0.075);border-color: rgba(144, 144, 144, 0.25);color: inherit;border-radius: 4px;    border-style: solid;    border-width: 1px;    display: block;    outline: 0;    padding: 0 1em;    text-decoration: none;height: 3.5em !important;    width: 100%;}    ul.actions.fit > li {    display: table-cell;    padding: 0 0 0 0.5em;}    .button.special {    background-color: #51BAA4;    box-shadow: none;    color: #fff !important;}.button.disabled, .button:disabled {    cursor: default;    opacity: 0.25;}.button.fit {    display: block;    margin: 0 0 1em 0;    width: 100%;}ul.actions.fit.small {    margin-left: -0.5em;    width: calc(100% + 0.5em);}ul.actions.fit.small > li {    padding: 0 0 0 0.5em;}ul.actions.fit {    display: table;    margin-left: -1em;    padding: 0;    table-layout: fixed;    width: calc(100% + 1em);}.button.small {    font-size: 0.8em;}/////////////////////////////////**/#cerrarsesion{border: 0;background-color: #07090c;text-transform: uppercase;padding: 0;line-height: 3.5em;font-size: 1em;}.passinput{margin: 1em 0em;background-color: rgba(144, 144, 144, 0.075);border-color: rgba(144, 144, 144, 0.25);color: inherit;height: 3.5em;border-radius: 4px;border-style: solid;border-width: 1px;display: block;outline: 0;padding: 0 1em;text-decoration: none;width: 100%;}.switch{position: relative;display: inline-block;width: 60px;height: 34px;margin: 0 1em;}.switch input{opacity: 0;width: 0;height: 0;display: none;}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;-webkit-transition:.4s;transition:.4s;}.slider:before{position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background-color:white;-webkit-transition:.4s;transition:.4s;}input:checked + .slider{background-color: #2196F3;}input:focus + .slider {box-shadow: 0 0 1px #2196F3;}input:checked + .slider:before {content:'';-webkit-transform: translateX(26px);-ms-transform: translateX(26px);transform: translateX(26px);}.slider.round {border-radius: 34px;width: 60px;}.slider.round:before {border-radius: 50%;}.option{display: inline-block;margin: 0 1em;height: 34px;line-height: 34px;vertical-align: top;}.estado{vertical-align:middle;display: none;}.xas{display: inline-block;height: auto;}.border{border:solid;border-width: 0 2px;}.inn{display: inline-block;width: 50%;cursor: pointer;font-size: 15px;padding: 0.5em;text-align: center;color: white;}.selected{border-bottom: solid 4px white;transition:border-width 0.1s linear 0.1s;}select{text-align-last:center;padding: 0;}label{width: 1em;height: 1em;padding: 0 1em;display: block;font-size: 0.9em;font-weight: 700;margin: 0 0 1em 0;}.mytd{height: 3em;vertical-align: middle;padding: 0 0 0 1em;text-align: left;}.deleteop{height: 3em;vertical-align: middle;padding: 0 0 0 1em;}.modificacionprograma{padding:0 1em; margin: 1em 0;overflow: hidden; display: none;cursor:pointer;width:90%;color:#1e2832;height: 97%;border:solid;border-width: 0 2px;}.pli{padding: 0 !important;display: inline-block !important;width: 48% !important;}.pa{font-size: 70% !important;width: 70% !important;display: inline-block !important;}.trash:before{content: '\2718';-moz-osx-font-smoothing: grayscale;-webkit-font-smoothing: antialiased;font-family: FontAwesome;font-style: normal;font-weight: normal;text-transform: none !important;}#footerx {position:absolute;bottom:0;width: 100%;background-color: #1E2832;color: rgba(255, 255, 255, 0.75);text-align: center;}#footer {background-color: #1E2832;color: rgba(255, 255, 255, 0.75);text-align: center;}li {display: list-item;text-align: -webkit-match-parent;width: 15%;}"
#define LANDING "<html><head><meta charset='utf-8'/><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1, width=device-width, viewport-fit=cover'><title>BOTS</title><link rel='stylesheet' type='text/css' href='botsstyle.css'></head><body class='landing'><div id='weblanding' style='display: none'></div><div id='banner'><h2 id='deviceready'>BOTS</h2><ul class='actions'><div class='app'><div class='event received' style='display: block;'><div id='loginmod'> <h5>CONFIGURACION WIFI</h5><div id='wificonfigform'><input class='inputland' style='display:none' name='action' value='login'/><div><input class='inputland marg' id='STAssid' placeholder='SSID' type='text'/></div><div><input class='inputland marg' id='STApassword' placeholder='PASSWORD' type='password'/></div><div id='result'></div><div class='actions marg2'><div><input id='connect' type='submit' class='special' value='CONECTAR' /></div></div></div></div></div></div></ul></div></body><script type='text/javascript' src='botsjs.js'></script></html>"
#define LOGIN "<html><head><meta charset='utf-8'/><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1, width=device-width, viewport-fit=cover'><title>BOTS</title><link rel='stylesheet' type='text/css' href='botsstyle.css'></head><body class='login'><div id='weblogin' style='display: none'></div><div id='banner'><h2 class='logtitle' id='deviceready'>BOTS</h2><ul class='actions'><div class='app'><div class='event received' style='display: block;'><div id='loginmod'> <h5>INICIO DE SESION BOTS</h5><div id='botsloginform'><input class='inputland' style='display:none' name='action' value='login'/><div><input class='inputland marg' id='BOTSUSER' placeholder='USER' type='text' /></div><div><input class='inputland marg' id='BOTSPASS' placeholder='PASSWORD' type='password' /></div><div id='result'></div><div class='actions marg2'><div><input id='login' type='submit' class='special' value='LOGIN!' /></div><div id='recuperarcontraseña' class='marg' style='text-align:center;cursor:pointer'>¿Olvido su contraseña?</div></div></div><div id='recucontramod' style='display:none'><div>Ingrese E-mail para enviar contraseña de recupero</div><div><input class='inputland marg' id='recuuser' placeholder='Usuario' type='text' /></div><div><input class='inputland marg' id='mailrecu' placeholder='E-mail' type='email' /></div><div id='resultrecu'></div><div class='actions marg2'><div><input id='cancelrecu' type='reset' class='alt' value='Cancel' /><input id='recuperar' type='submit' class='special' value='Recuperar!' /></div></div></div></div></div></div></ul></div></body><script type='text/javascript' src='botsjs.js'></script></html>"
#define BOTSUSER "<html style='color:#777;'><head><meta charset='utf-8'/><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1, width=device-width, viewport-fit=cover'><title>BOTS</title><link rel='stylesheet' type='text/css' href='botsstyle.css'></head><body class='userbody'><div id='webuser' style='display: none'></div><header id='header'><div><a style='float: left;margin-left: 0.5em'>BOTS</a><a id='menu'>&#9776; Menu</a></div><div id='1' style='white-space: nowrap;line-height: normal;'><div id='dispositivostab' class='inn selected'>DISPOSITIVOS</div><div id='programastab' class='inn'>PROGRAMAS</div></div></header><nav id='nav' style='display: none;'><ul class='links'><li><a id='passwordbutton' style='cursor:pointer'>Cambiar Contraseña</a></li><li><a id='cerrarsesion' style='cursor:pointer'>Cerrar Sesión</a></li></ul><div id='passwordsurvey' style='display:none'><div><input  style='display:none' name='action' value='changepassword'/><div class='12u$(xsmall)'><input class='marg' id='oldpassword' name='oldpassword' placeholder='Contraseña Antigua' type='password' /></div><div class='12u$(xsmall)'><input class='marg' id='newpassword1' name='newpassword1' placeholder='Nueva Contraseña' type='password' /></div><div class='12u$(xsmall)'><input class='marg' id='newpassword2' name='newpassword2' placeholder='Confirmar Contraseña' type='password' /></div></div><div id='result'></div><div style='margin-top:1em' class='actions'><div><input id='cancel' type='reset' class='alt' value='Cancelar' /><input id='changepass' type='submit' class='special' value='Cambiar!' /></div></div></div><a id='closenav' class='close' style='-webkit-tap-highlight-color: rgba(0, 0, 0, 0);'>&#10006;</a></nav><div  id='main' class='wrapper' style='position: absolute; padding: 0em;text-align: center;width: 100%;left:50%;transform:translateX(-50%);'><div id='CLIENTESCONTAINER' class='container' style='padding:1em;'><div id='deviceready' class='major'><h2>BIENVENIDO!!!</h2><div id='clientescont' class='table-wrapper' style='overflow-y: auto;'><table><tbody id='clientes'><!--prueba--><tr><td class='mytd' style='width:100%;'>PORTON</td><td style='text-align: right;width:25%;'><label class='switch' id='label1' style='display:block'><input type='checkbox' id='check1' class='optioninput'><span class='slider round'></span></label></td><td id='estado1' class='estado'>O</td></tr><!--prueba--><tr><td class='mytd' style='width:100%;'>CALEFACCION</td><td clas='mytd' style='text-align: right;width:25%;'><label class='switch' id='label4' style='display:block'><input type='checkbox' id='check4' class='optioninput'><span class='slider round'></span></label></td><td id='estado4' class='estado'></td></tr><!--prueba--></tbody></table></div></div></div><div id='PROGRAMASCONTAINER' style='display: none;width:100% !important:padding:1em;' class='container'><div id='programas' style='display: inline-block;width: 100%;height: 100%;'><div style='width:100%;display:inline-block;text-align: right;'><div id='addprogram' style='display: inline-block;margin: 0 1em;cursor: pointer;'>&#10010;</div><i id='trash' class='trash'></i></div><div id='payload1' style='display: none;'></div><div id='tablaprogramas' class='table-wrapper' style='overflow-y: auto;'><table><tbody id='programasclientes'><!--prueba--><tr id='programa+programasres[i][0]+' style='cursor:pointer;'><td class='deleteop mytd' id='deleteprogram+programasres[i][0]+' style='display: none;'><input type='checkbox' id='del+programasres[i][0]+' checked data-toggle='toggle'><label for='del+programasres[i][0]+'></label></td><td class='mytd' id='nombreprograma+programasres[i][0]+' style='width: 100%;'>programasres[i][1]</td><td class='mytd' style='text-align: right;width: 25%'><label class='switch' style='display:block'><input type='checkbox' id='programaestado+programasres[i][0]+' class='optioninput'><span class='slider round'></span></label></td></tr><!--prueba--><!--prueba--><tr id='programa+programasres[i][1]+' style='cursor:pointer;'><td class='deleteop mytd' id='deleteprogram+programasres[i][1]+' style='display: none;'><input type='checkbox' id='del+programasres[i][1]+' checked data-toggle='toggle'><label for='del+programasres[i][0]+'></label></td><td class='mytd' id='nombreprograma+programasres[i][1]+' style='width: 100%;'>programasres[i][2]</td><td class='mytd' style='text-align: right;width: 25%'><label class='switch' style='display:block'><input type='checkbox' id='programaestado+programasres[i][1]+' class='optioninput'><span class='slider round'></span></label></td></tr><!--prueba--></tbody></table></div><div id='eliminarprogramas' style='position: absolute;width: 90%;display: table-cell;bottom: 5px;display: none;cursor: pointer;'><i style='font-size: 2em' class='trash'></i><div style='font-size: 1em'>Eliminar</div></div></div></div><div id='modificacionprograma' class='modificacionprograma'><h3 id='editarprogramatitulo' style='text-align: left;margin:0; '>Modificar Programa</h3><div id='programid'><div style='margin:2% 0%;width: 100%;align-items: center;'><div class='12u$(xsmall)'><input  id='nombreprogramaedit' type='text' placeholder='Nombre Programa' style='height: auto;' /></div></div><div class='12u$(xsmall)' style='margin:3% 0' id='programdays'><ul class='actions fit small' style='margin-bottom: 0px;'><li><a style='min-width: unset;width:15%;cursor: pointer;' id='D1' class='button special fit small disabled'>L</a></li><li><a style='min-width: unset;width:15%;cursor: pointer;' id='D2' class='button special fit small disabled'>M</a></li><li><a style='min-width: unset;width:15%;cursor: pointer;' id='D3' class='button special fit small disabled'>M</a></li><li><a style='min-width: unset;width:15%;cursor: pointer;' id='D4' class='button special fit small disabled'>J</a></li><li><a style='min-width: unset;width:15%;cursor: pointer;' id='D5' class='button special fit small disabled'>V</a></li><li><a style='min-width: unset;width:15%;cursor: pointer;' id='D6' class='button special fit small disabled'>S</a></li><li><a style='min-width: unset;width:15%;cursor: pointer;' id='D7' class='button special fit small disabled'>D</a></li></ul></div><div class='12u$' style='margin:2% 0 2% 0;text-align: left;white-space:nowrap;display: inline-block;' id='programhorarios'><div class='6u$' style='display: inline-block;white-space:nowrap'>Hora Inicio: </div><div class='6u$' style='display: inline-block;white-space:nowrap;text-align:right;'><div class='5u$' style='white-space: nowrap;display: inline-block;max-width: 100px;'><select class='xas' name='category' id='HORAINICIO'><option value='HH'>HH &#9660;</option></select></div><b>:</b><div class='5u$' style='white-space: nowrap;display: inline-block;max-width: 100px;'><select  class='xas' name='category' id='MINUTOINICIO'><option value='MM'>MM &#9660;</option></select></div></div></div><div id='HMF' class='12u$' style='margin:0% 0 2% 0;text-align: left;white-space:nowrap;display: inline-block;'><div class='6u$' style='display: inline-block;white-space:nowrap'>Hora Fin: </div><div class='6u$' style='display: inline-block;white-space:nowrap;text-align:right;'><div class='5u$' style='white-space: nowrap;display: inline-block;max-width: 100px;'><select  class='xas' name='category' id='HORAFIN'><option value='HH'>HH &#9660;</option></select></div><b>:</b><div class='5u$' style='white-space: nowrap;display: inline-block;max-width: 100px;'><select  class='xas' name='category' id='MINUTOFIN'><option value='MM'>MM &#9660;</option></select></div></div></div><div id='tabladevices' class='table-wrapper' style='overflow-y: scroll;padding-right: 0;overflow-x:hidden;'><table><tbody id='devicesprogram'></tbody></table></div><div class='12u$(xsmall)' style='padding:0 1em;white-space:nowrap; position:absolute;bottom:0;left:50%;transform:translateX(-50%);width:100%;' id='programbutton'><ul class='actions fit small' style='margin:1% 0 1% 0;max-width: 425px;width:98%; display: inline-block;'><li class='pli'><a id='canceledit' href='#' class='button fit small pa'>Cancelar</a></li><li class='pli'><a id='saveedit' href='#' class='button special fit small pa'>Guardar</a></li></ul></div></div></div></div><footer id='footerx' style='position:fixed;height: 35px;' ><div class='inner' style='padding: 0'><ul class='copyright' style='margin: 0'><li>&copy; BOTS.</li></ul></div></footer> </body><script type='text/javascript' src='botsjs.js'></script></html>"
#define BOTSSCRIPT "var ID = function(id){return document.getElementById(id);};document.addEventListener('click',function(e){switch (e.target && e.target.id) {case ('menu'):ID('nav').style.display='block';break;case ('main'):ID('nav').style.display='none';break;case ('closenav'):ID('nav').style.display='none';break;case ('passwordbutton'):ID('passwordsurvey').style.display='inline-block';break;case ('cancel'):ID('passwordsurvey').style.display='none';break;case ('changepass'):passwordxml();break;case ('dispositivostab'):dispositivos(1);break;case ('programastab'):dispositivos(0);break;case ('agregarprograma'):programeditshow(0);break;case ('canceledit'):programeditshow(0);break;case ('addprogram'):programeditshow(0);break;case ('eliminarprogramas'):eliminarprogramas();break;case ('saveedit'):nuevoprograma();break;case ('trash'):delmenu();break;case ('connect'):wificonfigxml();break;case ('login'):botsloginxml();break;case ('recuperarcontraseña'):recutoggle();break;case ('cancelrecu'):recutoggle();break;case ('recuperar'):recuperarxml();break;default:break;}for (var i = 0; i <= max; i++) {if(e.target && e.target.id== ('nombreprograma'+i)){programeditshow(i);}if(e.target && e.target.id== ('check'+i)){check(i);}if(e.target && e.target.id== ('programaestado'+i)){modificarestadonprograma(i);}}for (var i = 1; i <= 7; i++) {if(e.target && e.target.id== ('D'+i)){selectday('D'+i);}}});function myajax(stringpost,url){return new Promise(resolve => {setTimeout(() => {var xmlHttp = new XMLHttpRequest();xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {resolve(xmlHttp.responseXML.getElementById('respxml').childNodes[0].nodeValue);}};xmlHttp.open('POST',url,true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.send(stringpost);}, 2000);});}var max=0;var res=[];async function mqtt(){res=[];var message = await myajax(null,'/htmlclientesxml');ID('clientes').innerHTML='';var progx = message.split('$');if (progx.length>1) {for (var i = 0; i < progx.length-1; i++) {res[i] = progx[i].split('?');if (res[i][0]>max) {max=res[i][0];}ID('clientes').innerHTML+=`<tr><td class='option'>`+res[i][1]+`</td><td style='text-align: right;'><label id='label`+res[i][0]+`' class='switch'><input id='check`+res[i][0]+`' type='checkbox' class='optioninput'><span class='slider round'></span></label><span id='estado`+res[i][0]+`' class='estado'></span></td></tr>`;}}else{ID('programasclientes').innerHTML+=`<tr style='cursor:pointer'><td id='nodev' class='mytd'>NO DEVS</td></tr>`;}programasclientes();}var programasres=[];async function programasclientes(){programasres=[];var message=await myajax(null,'/htmlprogramasclientesxml');ID('programasclientes').innerHTML='';ID('devicesprogram').innerHTML='';var progx=message.split('$');if (progx.length>1){for (var i=0;i<progx.length-1;i++) {programasres[i]=progx[i].split('?');if (programasres[i][0]>max) {max=programasres[i][0];}ID('programasclientes').innerHTML+=`<tr id='programa`+programasres[i][0]+`' style='cursor:pointer;'><td class='deleteop mytd' id='deleteprogram`+programasres[i][0]+`' style='display: none;'><input type='checkbox' id='del`+programasres[i][0]+`' checked data-toggle='toggle'><label for='del`+programasres[i][0]+`'></label></td><td class='mytd' id='nombreprograma`+programasres[i][0]+`' style='width: 100%;'>`+programasres[i][1]+`</td><td class='mytd' style='text-align: right;width: 25%'><label class='switch' style='display:block'><input type='checkbox' id='programaestado`+programasres[i][0]+`' class='optioninput'><span class='slider round'></span></label></td></tr>`;}for (var i=0;i<programasres.length;i++){var u= ID('programaestado'+programasres[i][0]);if (programasres[i][2]==1){u.checked=true;}else if(programasres[i][2]==0){u.checked=false;}}}else{ID('programasclientes').innerHTML+=`<tr style='cursor:pointer'><td id='agregarprograma' class='mytd'>+ Agregar programa</td></tr>`;}for (var j=0;j<res.length;j++){ID('devicesprogram').innerHTML+=`<tr><td class='mytd'>`+res[j][1]+`</td><td class='mytd' style='text-align: right;'><input type='checkbox' id='devicesprogram`+res[j][0]+`'><label for='devicesprogram`+res[j][0]+`'></label></td></tr>`;}setTimeout(receive,1000);}function recutoggle(){var x1 = ID('recucontramod');var x2 = ID('botsloginform');if (x1.style.display=='block') {x1.style.display='none';x2.style.display='block';}else {x1.style.display='block';x2.style.display='none';}}function selectday(a){ID(a).value=!ID(a).value;if (ID(a).value==false){ID(a).className = 'button special fit small disabled';}if (ID(a).value==true){ID(a).className = 'button special fit small';}}function delmenu() {var cols = document.getElementsByClassName('deleteop');for(i = 0; i < cols.length; i++) {if (cols[i].style.display=='none'){cols[i].style.display='table-cell';ID('eliminarprogramas').style.display='table-cell';}else{cols[i].style.display='none';ID('eliminarprogramas').style.display='none';}}}function dispositivos(s){ID('PROGRAMASCONTAINER').className = 'container';ID('programas').style.display='inline-block';ID('modificacionprograma').style.display = 'none';ID('eliminarprogramas').style.display='none';ID('main').className = 'wrapper';if(s==1){ID('dispositivostab').className = 'inn selected';ID('programastab').className = 'inn';ID('CLIENTESCONTAINER').style.display = 'block';ID('PROGRAMASCONTAINER').style.display = 'none';}else if (s==0) {ID('dispositivostab').className = 'inn';ID('programastab').className = 'inn selected';ID('CLIENTESCONTAINER').style.display = 'none';ID('PROGRAMASCONTAINER').style.display = 'block';var cols = document.getElementsByClassName('deleteop');for(i = 0; i < cols.length; i++) {cols[i].style.display='none';}for (var i = 0; i <= programasres.length-1; i++) {var u= ID('programaestado'+programasres[i][0]);if (programasres[i][2]==1) {u.checked=true;} else if(programasres[i][2]==0) {u.checked=false;}}}}function programeditshow(IDx){if (IDx==0) {ID('programid').value='0';ID('editarprogramatitulo').innerHTML='NUEVO PROGRAMA';ID('nombreprogramaedit').value='';for (var i = 1; i <= 7; i++) {ID('D'+i).value=false;ID('D'+i).className='button special fit small disabled';}ID('HORAINICIO').value='HH';ID('MINUTOINICIO').value='MM';ID('HORAFIN').value='HH';ID('MINUTOFIN').value='MM';for (var i = 0; i < res.length; i++) {ID('devicesprogram'+res[i][0]).checked=false;}}else{ID('editarprogramatitulo').innerHTML='MODIFICAR PROGRAMA';for (var i = 0; i < programasres.length; i++) {if(programasres[i][0]==ID){ID('programid').value=programasres[i][0];ID('nombreprogramaedit').value=programasres[i][1];var days = programasres[i][3].split('D');for (var d = 0; d <= days.length-1; d++) {if(days[d]!='D' && days[d]!=''){selectday('D'+days[d]);}}var inicio = programasres[i][4].split(':');ID('HORAINICIO').value=inicio[0];ID('MINUTOINICIO').value=inicio[1];var fin = programasres[i][5].split(':');ID('HORAFIN').value=fin[0];ID('MINUTOFIN').value=fin[1];var dev =programasres[i][6].split('%');for (var v = 0; v < dev.length; v++) {if(dev[v]!='%' && dev[v]!=''){ID('devicesprogram'+dev[v]).checked=true;}}}}}if(ID('programas').style.display==='none'){ID('programas').style.display='inline-block';ID('modificacionprograma').style.display='none';ID('main').className ='wrapper';}else{ID('programas').style.display='none';ID('modificacionprograma').style.display='inline-block';ID('main').className='wrapper border';}ID('tabladevices').style.height=(((ID('programbutton').getBoundingClientRect().top)-(ID('HMF').getBoundingClientRect().bottom))*0.95)+'px';}function check(idx){if (ID('check'+idx).checked===true){send(idx+'I');}else{send(idx+'O');}}async function wificonfigxml() {var msg = await myajax('STAssid:'+ID('STAssid').value+'?STApassword:'+ID('STApassword').value+'?','/wificonfigxml');if (msg=='1') {window.location.href = '/botslogin';}else{ID('result').innerHTML = msg;}}async function botsloginxml(){ID('result').innerHTML = 'Ingresando...';var msg = await myajax('BOTSUSER:'+ID('BOTSUSER').value+'?BOTSPASS:'+ID('BOTSPASS').value+'?','/botsloginxml');if (msg=='1'){window.location.href = '/user';}else{ID('result').innerHTML = msg;}}async function recuperarxml(){if(ID('mailrecu').value!=''){if(ID('recuuser').value!=''){ID('resultrecu').innerHTML = await myajax('user:'+ID('recuuser').value+'?email:'+ID('mailrecu').value+'?','/recupassxml');}else{ID('resultrecu').innerHTML='Complete Usuario';}}else{ID('resultrecu').innerHTML='Complete E-mail';}}async function passwordxml(){var message = await myajax('oldpassword:'+ID('oldpassword').value+'?newpassword1:'+ID('newpassword1').value+'?newpassword2:'+ID('newpassword2').value+'?','/passwordxml');if (message==1) {ID('result').innerHTML='Contraseña Modificada Correctamente.';}else{ID('result').innerHTML=message;}}async function receive(){var message = await myajax(null,'/htmlreceivexml');var estado = message.substring(message.length - 1, message.length);var id = message.substring(0, message.length-1);ID('estado'+id).innerHTML=estado;if(estado=='I'){ID('check'+id).checked=true;setTimeout(receive,500);}if(estado=='O'){ID('check'+id).checked=false;setTimeout(receive,500);}}async function eliminarprogramas(){var progamaeliminar='?';for (var i=0;i<programasres.length;i++) {if(ID('del'+programasres[i][0]).checked==true){progamaeliminar+=programasres[i][0]+'?';}}var message = await myajax({progamaeliminar:progamaeliminar},'/htmlprogamaeliminarxml');programasclientes();delmenu();}async function modificarestadonprograma(u){await myajax({ID:u,ESTADO:ID('programaestado'+u).checked},'/htmlmodificarestadoprogramaxml');programasclientes();}async function send(dato){await myajax('strtopic:'+dato+'?','/htmlsendxml');}async function nuevoprograma(){var programid=ID('programid').value;var nombreprograma=ID('nombreprogramaedit').value;if (nombreprograma!='') {var days='';for (var i=1;i<=7;i++) {if(ID('D'+i).value==true){days+='D'+i;}}if (days!='') {var horainicio = ID('HORAINICIO').value+':'+ID('MINUTOINICIO').value;if (ID('HORAINICIO').value!='HH' && ID('MINUTOINICIO').value!='MM') {var horafin = ID('HORAFIN').value+':'+ID('MINUTOFIN').value;if (ID('HORAFIN').value!='HH' && ID('MINUTOFIN').value!='MM') {var devi = '%';for (var i=0;i<res.length;i++){if(ID('devicesprogram'+res[i][0]).checked==true){devi+=res[i][0]+'%';}}if (devi!='%') {var sta =true;if (programid!=0) {sta=ID('programaestado'+programid).checked;}await myajax({estado:sta,programid:programid,nombreprograma:nombreprograma,days:days,horainicio:horainicio,horafin:horafin,devi:devi},'/htmlnuevoprogramaxml');programasclientes();programeditshow(0);}else{alert('DISPOSITIVOS NO SELECCIONADOS');}}else{alert('HORA FIN INCORRECTA');}}else{alert('HORA INICIO INCORRECTA');}}else{alert('SIN DIAS SELECCIONADOS');}}else{alert('INGRESE NOMBRE DE PROGRAMA');}}function user(){var HeaderHeight=ID('header').getBoundingClientRect().height;var footerHeight=ID('footerx').getBoundingClientRect().height;var mainHeight=document.body.clientHeight-HeaderHeight-footerHeight;ID('main').style.top=HeaderHeight+'px';ID('main').style.bottom=footerHeight+'px';for (var i = 0; i <= 60; i++) {var line =`<option value='`+i+`'>`+i+`</option>`;if (i<=24) {ID('HORAINICIO').innerHTML+=line;ID('HORAFIN').innerHTML+=line;}ID('MINUTOINICIO').innerHTML+=line;ID('MINUTOFIN').innerHTML+=line;}mqtt();}if(document.body.contains(ID('webuser'))){console.log('webuser');user();}if(document.body.contains(ID('weblogin'))){console.log('weblogin');}if(document.body.contains(ID('weblanding'))){console.log('weblanding');}"

void botsjs(){server.send(200,"text/javascript", BOTSSCRIPT);}
void botsstyle(){server.send(200,"text/css", BOTSSTYLE);}
void botslogin(){server.send(200,"text/html", LOGIN);}
void user(){server.send(200, "text/html", BOTSUSER);}
void landingpage(){server.send(200, "text/html", LANDING);}
void landing(){//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  switch (estado){
    case 0: {landingpage();break;}
    case 100: {botslogin();break;}
    case 200: {user();break;}
    case 300: {user();break;}
    case 400: {user();break;}
    default: {landingpage();break;}
  }
}
////////////////////////////////SESSIONOUT//////////////////////////////////////////////////////////////////////////
void sessionout() {estado=100;landing();}//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
////////////////////////////////WIFICONFIG//////////////////////////////////////////////////////////////////////////
void wificonfig(){
  char APssid[25]; strcpy(APssid, "BOTS-");
  char APpassword[20];
  WiFi.macAddress().toCharArray(APpassword, WiFi.macAddress().length() + 1);
  strcat(APssid, APpassword);
  WiFi.disconnect(true);
  ESP.eraseConfig();
    WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  /*HTML*/
  server.on("/", landing); 
  server.on("/botsstyle.css", botsstyle);
  server.on("/botsjs.js", botsjs);
  server.on("/botslogin", botslogin); 
  server.on("/user", user); 
  server.on("/sessionout", sessionout);
  /*XML*/
  server.on("/htmlprogramasclientesxml", htmlprogramasclientesxml);
  server.on("/wificonfigxml", wificonfigxml);
  server.on("/botsloginxml", botsloginxml);
  server.on("/htmlclientesxml", htmlclientesxml);
  server.on("/htmlsendxml", htmlsendxml); 
  server.on("/htmlprogamaeliminarxml", htmlprogamaeliminarxml); 
  server.on("/htmlnuevoprogramaxml", htmlnuevoprogramaxml); 
  server.on("/htmlmodificarestadoprogramaxml", htmlmodificarestadoprogramaxml); 
  server.on("/htmlreceivexml", htmlreceivexml);
  server.on("/recupassxml", recupassxml);
  server.on("/passwordxml", passwordxml);
  server.begin();
  Serial.println("AP CONFIG --> SSID: " + String(APssid) + " / PASS: " + String(APpassword));
  Serial.print("AP IP: "); Serial.println(myIP);
}
////////////////////////////////RESTARTCONFIG//////////////////////////////////////////////////////////////////////////
void restartconfig() { /*REINICIAR ARDUINO CON CONFIGURACIONES DE EEPROM*/
  if (EEPROMr.read(WIFIDIR + DATA_OFFSET) != 255 && EEPROMr.read(WIFIDIR + DATA_OFFSET) != 0) {
    char eepromssid[30]; strcpy(eepromssid, readChar(WIFIDIR));
    byte WIFIPASS = WIFIDIR + strlen(eepromssid) + DATA_OFFSET;
    char eeprompass[30]; strcpy(eeprompass, readChar(WIFIPASS));
    Serial.println("EEPROM GET--> SSID: " + String(eepromssid) + " / PASS: " + String(eeprompass));
    String temp = WiFiConnect(eepromssid, eeprompass);
    if (temp == "1") {
      estado=100;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
      Serial.println("wificonfig: " + String(temp));
      if (EEPROMr.read(BOTSDIR + DATA_OFFSET) != 255 && EEPROMr.read(BOTSDIR + DATA_OFFSET) != 0) {
        char eeprombotsuser[30]; strcpy(eeprombotsuser, readChar(BOTSDIR));
        byte BOTSPASS = BOTSDIR + strlen(eeprombotsuser) + DATA_OFFSET;
        char eeprombotspassword[30]; strcpy(eeprombotspassword, readChar(BOTSPASS));
        Serial.println("EEPROM BOTS-->USER: " +String(eeprombotsuser) + " / PASS: " +String(eeprombotspassword));
        temp = phprequest("/login.php","user="+String(eeprombotsuser)+"&pass="+String(eeprombotspassword)+"");
        if (temp == "1") {
          Serial.println("botslogin: " + String(temp));
          estado=200;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
          mqttsetup();
        }
      }
    }else{Serial.println("Configuracion WIFI EEPROM Fallada. Reiniciando Red AP...");wificonfig();}
  }else{Serial.println("EEPROM sin registro de Inicio. Continuar por AP...");}
}
////////////////////////////////WIFISETUP//////////////////////////////////////////////////////////////////////////
void wifisetup(){wificonfig();restartconfig();}
////////////////////////////////WIFILOOP//////////////////////////////////////////////////////////////////////////
void wifiloop(){server.handleClient();}
