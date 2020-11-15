#define DATA_OFFSET 10
WiFiServer phpserver(8080);
ESP8266WebServer server(80);
const char espserver[] = "http://casabots.000webhostapp.com";
int WIFIDIR = 0; int estado=0;int BOTSDIR=50;
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
    //Serial.print("pos[");Serial.print(q);Serial.print("]: ");Serial.println(pos[q]);
    if (pos[q] != -1) {sub[q] = next[q].substring(0, pos[q]);
    if ((sub[q].indexOf(":", 0)) > 0) {postarg[q] = sub[q].substring(sub[q].indexOf(":", 0) + 1, sub[q].length());}else{postarg[q] = sub[q];}
    next[q + 1] = next[q].substring(pos[q] + 1, next[q].length());
    //Serial.println("sub["+String(q)+"]: "+String(sub[q]);
    //Serial.println("next["+String(q+1)+"]: "+String(next[q+1]);
    //Serial.println("postarg["+String(q)+"]: "+String(postarg[q]);
    q++;
    }else{wexit = 1;}
  }while(wexit == 0);
  return postarg;
}
/////////////////////////PHPREQUEST/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////PHPREQUEST/////////////////////////////////////////////////////////////////////////////////////////////////////
char * phprequest(String Header,String data){
  char phpresponse[600];
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;//Declare object of class HTTPClient
    http.begin(espserver+Header);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  
    int httpCode = http.POST(data);//Send the request
    String phppayload = http.getString();//Get the response payload
    phppayload.toCharArray(phpresponse,phppayload.length()+1);
    Serial.println(String(Header)+" HttpCode: "+String(httpCode));//Print HTTP return code
    http.end();//Close connection
  }else{strcpy(phpresponse,"Error de Conexion con Servidor PHP. Contacte Proveedor.");}
  //Serial.println(" // "+String(Header)+": "+String(phpresponse));
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
void htmlreceivexml(){
  /*char xmlresponse[100];
  strcpy(xmlresponse, MQTTCALLBACK);
  xmlend(xmlresponse);*/ 
  xmlend(MQTTCALLBACK);
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
  char xmlresponse[600]; strcpy(xmlresponse, phprequest("/clientes.php",data));
  xmlend(xmlresponse);
}
////////////////////////////////BOTSLOGINXML//////////////////////////////////////////////////////////////////////////
////////////////////////////////BOTSLOGINXML//////////////////////////////////////////////////////////////////////////
void botsloginxml() {
  //POST=BOTSUSER:BOTSUSER?BOTSPASS:BOTSPASS?
  String * args2 = plainsubstring(server.arg("plain"));
  String data = "user="+String(args2[0])+"&pass="+String(args2[1])+"";
  char xmlresponse[600]; strcpy(xmlresponse, phprequest("/login.php",data));
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
  int contconexion = 0;
  WiFi.begin(STAssid, STApassword);
  while (WiFi.status() != WL_CONNECTED && contconexion < 20) {contconexion++;delay(500);Serial.print(".");}
  Serial.print("STA IP: "); Serial.println(WiFi.localIP());
  if (WiFi.status() == WL_CONNECTED) {
    writeString(STAssid, WIFIDIR);
    writeString(STApassword, WIFIDIR + STAssid.length() + DATA_OFFSET);
    char eepromssid[30]; strcpy(eepromssid, readChar(WIFIDIR));
    int WIFIPASS = WIFIDIR + strlen(eepromssid) + DATA_OFFSET;
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
  if (xmlresponse == "1") {estado=100;}//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  Serial.println("wificonfigxml: " + String(xmlresponse));
  xmlend(xmlresponse);
}
////////////////////////////////SERVERHANDLER//////////////////////////////////////////////////////////////////////////
////////////////////////////////SERVERHANDLER//////////////////////////////////////////////////////////////////////////
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
char LANDING[]="<html><head><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><meta charset='utf-8'/><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1, width=device-width, viewport-fit=cover'><style>*{font-family:'Raleway', Helvetica, sans-serif;color:#fff;}body{background-color:#1E2832;text-align: center;display: inline-block !important;width: 100%;}#banner{padding:5em 2em 3em 2em;background-color:#1E2832;text-align:center;}h2{font-size:1.5em;padding:0;line-height:1.5em;}.actions{padding: 0;width:100%;max-width: 500px; display: inline-block;}input{border-radius: 4px;border-style: solid;border-width: 1px;display: block;outline: 0;padding: 0 1em;text-decoration: none;line-height: 2em;background-color: rgba(144, 144, 144, 0.075);border-color: rgba(144, 144, 144, 0.25);height: 3.5em;font-size: 12pt;font-weight: 400;width: 100%;} #estadowifi{text-align:center;margin:1em 0;}input[type='submit'].special{background-color: #51BAA4;transition: background-color 0.2s ease-in-out, color 0.2s ease-in-out;border-radius: 2.5em;border: 0;cursor: pointer;display: inline-block;font-weight: 700;height: 2.85em;line-height: 2.95em;font-size: 12pt;width: 49%;padding: 0 1.5em;text-align: center;text-decoration: none;text-transform: uppercase;white-space: nowrap;}</style><title>BOTS</title></head><body class='landing'><div id='banner'><h2 id='deviceready'>BOTS</h2><ul class='actions'><div class='app'><div class='event received' style='display: block;'><div id='loginmod'> <h5>CONFIGURACION WIFI</h5><div id='wificonfigform'><input style='display:none' name='action' value='login'/><div class='12u$(xsmall)'><input style='margin: 1em 0em' id='STAssid' placeholder='SSID' type='text' /></div><div class='12u$(xsmall)'><input style='margin: 1em 0em' id='STApassword' placeholder='PASSWORD' type='text' /></div><div id='result'></div><div class='actions' style='margin:2em 0;'><div><input style='min-width:49%' id='login' type='submit' class='special' value='CONECTAR' /></div></div></div></div></div></div></ul></div></body><script type='text/javascript'>document.getElementById('login').addEventListener('click',wificonfigxml);function wificonfigxml(){document.getElementById('result').innerHTML = 'Ingresando...';var xmlHttp = new XMLHttpRequest();var params ='STAssid:'+document.getElementById('STAssid').value+'?STApassword:'+document.getElementById('STApassword').value+'?';xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {var xmlDoc = xmlHttp.responseXML;var message = xmlDoc.getElementById('respxml').childNodes[0].nodeValue;if (message=='1') {window.location.href = '/botslogin';}else{document.getElementById('result').innerHTML = message;}}};xmlHttp.open('POST','wificonfigxml',true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.send(params);}</script></html>";
char LOGIN[]="<html> <head><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><meta charset='utf-8'/><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1, width=device-width, viewport-fit=cover'><style>*{font-family:'Raleway', Helvetica, sans-serif;color:#fff;}body{background-color:#1E2832;text-align: center;display: inline-block !important;width: 100%;}#banner{padding:5em 2em 3em 2em;background-color:#1E2832;text-align:center;}h2{font-size:1.5em;padding:0;line-height:1.5em;}.actions{padding: 0;width:100%;max-width: 500px; display: inline-block;}input{border-radius: 4px;border-style: solid;border-width: 1px;display: block;outline: 0;padding: 0 1em;text-decoration: none;width: 100%;background-color: rgba(144, 144, 144, 0.075);border-color: rgba(144, 144, 144, 0.25);height: 3.5em;font-size: 12pt;font-weight: 400;line-height: 2em;} #estadowifi{text-align:center;margin:1em 0;}input[type='submit'].special{background-color: #51BAA4;transition: background-color 0.2s ease-in-out, color 0.2s ease-in-out;border-radius: 2.5em;border: 0;cursor: pointer;display: inline-block;font-weight: 700;height: 2.85em;line-height: 2.95em;font-size: 12pt;width: 49%;padding: 0 1.5em;text-align: center;text-decoration: none;text-transform: uppercase;white-space: nowrap;}input[type='reset'].alt{background-color: transparent;box-shadow: inset 0 0 0 2px #51BAA4;color: #51BAA4 !important;}input[type='reset']{background-color: #25383B;box-shadow: none;color: #fff !important;}input[type='reset']{width: 49%;-webkit-appearance: none;transition: background-color 0.2s ease-in-out, color 0.2s ease-in-out;border-radius: 2.5em;border: 0;cursor: pointer;display: inline-block;font-weight: 700;height: 2.85em;line-height: 2.95em;padding: 0 1.5em;text-align: center;text-decoration: none;text-transform: uppercase;white-space: nowrap;}</style><title>BOTS</title></head><body class='landing'><div id='banner'><h2 id='deviceready'>BOTS</h2><ul class='actions'><div class='app'><div class='event received' style='display: block;'><div id='loginmod'> <h5>INICIO DE SESION BOTS</h5><div id='botsloginform'><input style='display:none' name='action' value='login'/><div class='12u$(xsmall)'><input style='margin: 1em 0em' id='BOTSUSER' placeholder='USER' type='text' /></div><div class='12u$(xsmall)'><input style='margin: 1em 0em' id='BOTSPASS' placeholder='PASSWORD' type='text' /></div><div id='result'></div><div class='actions' style='margin:2em 0;'><div><input style='min-width:49%' id='login' type='submit' class='special' value='LOGIN!' /></div><div id='recuperarcontraseña' style='text-align:center;margin:1em 0;cursor:pointer'>¿Olvido su contraseña?</div></div></div><div id='recucontramod' style='display:none'><div>Ingrese E-mail para enviar contraseña de recupero</div><div class='12u$(xsmall)'><input style='margin: 1em 0em' id='recuuser' placeholder='Usuario' type='text' /></div><div class='12u$(xsmall)'><input style='margin: 1em 0em' id='mailrecu' placeholder='E-mail' type='email' /></div><div id='resultrecu'></div><div class='actions' style='margin:2em 0;'><div><input style='min-width:49%' id='cancelrecu' type='reset' class='alt' value='Cancel' /><input style='min-width:49%' id='recuperar' type='submit' class='special' value='Recuperar!' /></div></div></div></div></div></div></ul></div></body><script type='text/javascript'>document.getElementById('login').addEventListener('click',botsloginxml);document.getElementById('recuperarcontraseña').addEventListener('click',recu);document.getElementById('cancelrecu').addEventListener('click',norecu);function recu(){document.getElementById('recucontramod').style.display='block';document.getElementById('botsloginform').style.display='none';}function norecu(){document.getElementById('recucontramod').style.display='none';document.getElementById('botsloginform').style.display='block';}function botsloginxml(){document.getElementById('result').innerHTML = 'Ingresando...';var xmlHttp = new XMLHttpRequest();var params ='BOTSUSER:'+document.getElementById('BOTSUSER').value+'?BOTSPASS:'+document.getElementById('BOTSPASS').value+'?';xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {var xmlDoc = xmlHttp.responseXML;var message = xmlDoc.getElementById('respxml').childNodes[0].nodeValue;if (message=='1'){window.location.href = '/user';}else{document.getElementById('result').innerHTML = message;}}};xmlHttp.open('POST','botsloginxml',true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.send(params);}document.getElementById('recuperar').addEventListener('click',recuperar);function recuperar(){if(document.getElementById('mailrecu').value!=''){if(document.getElementById('recuuser').value!=''){var xmlHttp = new XMLHttpRequest();var params ='user:'+document.getElementById('recuuser').value+'?email:'+document.getElementById('mailrecu').value+'?';xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {var xmlDoc = xmlHttp.responseXML;var message = xmlDoc.getElementById('respxml').childNodes[0].nodeValue;document.getElementById('resultrecu').innerHTML = message;}};xmlHttp.open('POST','recupassxml',true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.send(params);}else{document.getElementById('resultrecu').innerHTML='Complete Usuario';}}else{document.getElementById('resultrecu').innerHTML='Complete E-mail';}}</script></html>";
char USER[]="<html><head><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><meta charset='utf-8'/><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1, width=device-width, viewport-fit=cover'><style>*{font-family:'Raleway', Helvetica, sans-serif;color:#fff;}body{text-align: center;display: inline-block !important;width: 100%;margin: 0;}#header{height: 44px;line-height: 44px;background: #07090c;color: rgba(255, 255, 255, 0.75);cursor: default;text-align: right;width: 100%;}h1{left: 1em;color: #fff;display: inline-block;height: inherit;left: 1.25em;line-height: inherit;margin: 0;padding: 0;position: absolute;top: 0;text-decoration: none;font-size: 1em;}a{font-size: 1em;-moz-transition: color 0.2s ease-in-out;-webkit-transition: color 0.2s ease-in-out;-ms-transition: color 0.2s ease-in-out;transition: color 0.2s ease-in-out;color: #fff;display: inline-block;margin-right: 1.25em;text-decoration: none;}#header a[href='#nav'] {text-decoration: none;-webkit-tap-highlight-color: transparent;}#header a[href='#nav']:before {/*content: '';*/-moz-osx-font-smoothing: grayscale;-webkit-font-smoothing: antialiased;font-family: FontAwesome;font-style: normal;font-weight: normal;text-transform: none !important;}#header a[href='#nav']:before {margin: 0 0.5em 0 0;}#header a + a[href='#nav']:last-child {border-left: solid 1px transparent;padding-left: 1.25em;margin-left: 0.5em;}header.major h2 {font-size: 1.5em;}h2{color: #25383B;line-height: 1.5em;font-weight: 700;margin: 0 0 1em 0;text-transform: uppercase;}header.major p {font-size: 1em;}header h2 + p {font-size: 1.25em;margin-top: -1em;line-height: 1.85em;}header p {color: #25383B;margin: 0 0 1.5em 0;position: relative;text-transform: uppercase;}footer{position:absolute;bottom:0;text-decoration:none;width:100%;background-color:#1E2832;color:rgba(255, 255, 255, 0.75);text-align:center;}ul{list-style: none;padding-left: 1em;}ul li {padding-left: 0.5em;}#nav{transition: transform 0.5s ease, box-shadow 0.5s ease, visibility 0.5s;padding: 2.5em 1.75em;background: #07090c;color: #fff;height: 100%;max-width: 80%;overflow-y: auto;position: fixed;right: 0;text-transform: uppercase;top: 0;width: 20em;z-index: 10002;}#nav > ul.links {text-align: left;}#nav > ul {margin: 0 0 1em 0;}#nav > ul.links > li {padding: 0;}li {display: list-item;text-align: -webkit-match-parent;}#nav .close {height: 4em;line-height: 4em;text-decoration: none;transition: color 0.2s ease-in-out;border: 0;color: #a5a9ad;cursor: pointer;display: block;padding-right: 1.25em;position: absolute;right: 0;text-align: right;top: 0;vertical-align: middle;width: 7em;}#nav > ul.links > li > a:not(.button) {border: 0;border-top: solid 1px transparent;color: inherit;line-height: 3.5em;text-decoration: none;}#main{padding: 2em 0 0.1em 0;}.wrapper{position: relative;}.container{margin-left: auto;margin-right: auto;width: 90% !important;}header.major{margin-bottom: 2em;}#cerrarsesion{border: 0;background-color: #07090c;text-transform: uppercase;padding: 0;line-height: 3.5em;font-size: 1em;}.passinput{margin: 1em 0em;background-color: rgba(144, 144, 144, 0.075);border-color: rgba(144, 144, 144, 0.25);color: inherit;height: 3.5em;border-radius: 4px;border-style: solid;border-width: 1px;display: block;outline: 0;padding: 0 1em;text-decoration: none;width: 100%;}.alt{background-color: transparent;box-shadow: inset 0 0 0 2px #51BAA4;color: #51BAA4 !important;min-width: 45%;border: 0;transition: background-color 0.2s ease-in-out, color 0.2s ease-in-out;border-radius: 2.5em;cursor: pointer;display: inline-block;font-weight: 700;height: 2.85em;line-height: 2.95em;min-width: 10em;padding: 0 1.5em;text-align: center;text-decoration: none;text-transform: uppercase;white-space: nowrap;}#changepass{background-color: #51BAA4;box-shadow: none;color: #fff !important;border-radius: 2.5em;border: 0;cursor: pointer;display: inline-block;font-weight: 700;height: 2.85em;line-height: 2.95em;min-width: 10em;padding: 0 1.5em;text-align: center;text-decoration: none;text-transform: uppercase;white-space: nowrap;transition: background-color 0.2s ease-in-out, color 0.2s ease-in-out;}/*/////////////////////////SLIDER///////////////////////////////////////////////////////////////////////////////*/.switch{position: relative;display: inline-block;width: 60px;height: 34px;margin: 0 1em;}/* The switch - the box around the slider */.switch input{opacity: 0;width: 0;height: 0;display: none;}/* Hide default HTML checkbox */.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;-webkit-transition:.4s;transition:.4s;}/* The slider */.slider:before{position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background-color:white;-webkit-transition:.4s;transition:.4s;}input:checked + .slider{background-color: #2196F3;}input:focus + .slider {box-shadow: 0 0 1px #2196F3;}input:checked + .slider:before {-webkit-transform: translateX(26px);-ms-transform: translateX(26px);transform: translateX(26px);}.optioninput{}.slider.round {border-radius: 34px;}/* Rounded sliders */.slider.round:before {border-radius: 50%;}.option{display: inline-block;margin: 0 1em;height: 34px;line-height: 34px;vertical-align: top;}.estado{vertical-align:top;margin:0 1em;color: #25383B;}</style><title>BOTS</title></head><body><!-- Header --><header id='header' ><h1><a href='index.html'>BOTS</a></h1><a id='menu' style='cursor: pointer;'>Menu</a></header><!-- Nav --><nav id='nav' style='display: none;'> <ul class='links'><li><a id='passwordbutton' style='cursor:pointer'>Cambiar Password</a></li><div id='passwordsurvey' style='display:none;width: 90%;'><div><!--input style='display:none' name='action' value='changepassword'/--><div><input class='passinput' id='oldpassword' name='oldpassword' placeholder='Password Antigua' type='password' /></div><div><input class='passinput' id='newpassword1' name='newpassword1' placeholder='Nueva Password' type='password' /></div><div><input class='passinput' id='newpassword2' name='newpassword2' placeholder='Confirmar Password' type='password' /></div></div><div id='result'></div><div style='margin-top:1em' class='actions'><div><input style='min-width:45%' id='cancel' type='reset' class='alt' value='Cancelar' /><input style='min-width:45%' id='changepass' type='submit' class='special' value='Cambiar!' /></div></div></div><form action='sessionout' method='get'><input type='submit' style='cursor:pointer' id='cerrarsesion' value='Cerrar Sesion' /></form></ul><a id='closenav' class='close' style='-webkit-tap-highlight-color: rgba(0, 0, 0, 0);'>X</a></nav><!-- Main --><section id='main' class='wrapper'><div class='container'><header id='deviceready' class='major special'><h2>BIENVENIDO!!!</h2><p id='clientes'></p></header></div></section><!-- Footer --><footer id='footer'><div class='inner'><ul class='copyright'><li>&copy; BOTS.</li></ul></div></footer> </body><!-- Scripts -->      <script type='text/javascript'>document.getElementById('menu').addEventListener('click',shownav);function shownav(){document.getElementById('nav').style.display='block';}document.getElementById('main').addEventListener('click',hidenav);document.getElementById('closenav').addEventListener('click',hidenav);function hidenav(){document.getElementById('nav').style.display='none';}document.getElementById('passwordbutton').addEventListener('click',verpasschange);function verpasschange(){document.getElementById('passwordsurvey').style.display='inline-block';}document.getElementById('cancel').addEventListener('click',cancel);function cancel(){document.getElementById('passwordsurvey').style.display='none';}document.getElementById('changepass').addEventListener('click',passwordxml);function passwordxml(){var xmlHttp = new XMLHttpRequest();var params ='oldpassword:'+document.getElementById('oldpassword').value+'?newpassword1:'+document.getElementById('newpassword1').value+'?newpassword2:'+document.getElementById('newpassword2').value+'?';xmlHttp.open('POST','passwordxml',true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {var xmlDoc = xmlHttp.responseXML;var message = xmlDoc.getElementById('respxml').childNodes[0].nodeValue;if (message==1) {document.getElementById('result').innerHTML = 'Contraseña Modificada Correctamente.';}else{document.getElementById('result').innerHTML = message;}}};xmlHttp.send(params);}function receive(){var xmlHttp = new XMLHttpRequest();xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {var xmlDoc = xmlHttp.responseXML;var message = xmlDoc.getElementById('respxml').childNodes[0].nodeValue;/*console.log('Recibiendo: '+message);*/var a = message;var estado = a.substring(a.length - 1, a.length);var id = a.substring(0, a.length-1);document.getElementById('estado'+id).innerHTML=estado;if(estado=='I'){document.getElementById('check'+id).checked = true;setTimeout(receive,500);}if(estado=='O'){document.getElementById('check'+id).checked = false;setTimeout(receive,500);}}};xmlHttp.open('POST','htmlreceivexml',true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.send();}function check(idx){if (document.getElementById('check'+idx).checked === true){send(idx+'I');}else if(document.getElementById('check'+idx).checked === false){send(idx+'O');}}function send(dato){var xmlHttp = new XMLHttpRequest();var params='strtopic:'+dato+'?';xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {var xmlDoc = xmlHttp.responseXML;var message = xmlDoc.getElementById('respxml').childNodes[0].nodeValue;/*console.log('htmlsendxml: '+message);*/}};xmlHttp.open('POST','htmlsendxml',true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.send(params);}function mqtt(){var xmlHttp = new XMLHttpRequest();xmlHttp.onreadystatechange=function() {if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {var xmlDoc = xmlHttp.responseXML;var message = xmlDoc.getElementById('respxml').childNodes[0].nodeValue;/*console.log('mqtt: '+message);*/var clientesres = message.split('?');for (var i = 0; i < clientesres.length-1; i++) {document.getElementById('clientes').innerHTML+=\"<div>\"+\"<p class='option'>\"+clientesres[i]+\"</p>\"+\"<label id='label\"+(i+1)+\"' class='switch'>\"+\"<input id='check\"+(i+1)+\"' type='checkbox' class='optioninput'>\"+\"<span id='slider\"+(i+1)+\"' class='slider round'></span>\"+\"</label>\"+\"<span id='estado\"+(i+1)+\"' class='estado'></span>\"+\"</div>\";}}setTimeout(receive,1000);};xmlHttp.open('POST','htmlclientesxml',true);xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xmlHttp.send();}mqtt();document.addEventListener('click',function(e){if(e.target && e.target.id== 'check1'){check(1);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check2'){check(2);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check3'){check(3);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check4'){check(4);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check5'){check(5);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check6'){check(6);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check7'){check(7);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check8'){check(8);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check9'){check(9);}});document.addEventListener('click',function(e){if(e.target && e.target.id== 'check10'){check(10);}});</script></html>  ";
void botslogin(){server.send(200, "text/html", LOGIN);}
void user(){server.send(200, "text/html", USER);}
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
void sessionout() {
  estado=100;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
  landing();
}
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
  server.on("/botslogin", botslogin); 
  server.on("/user", user); 
  server.on("/sessionout", sessionout);
  /*XML*/
  server.on("/wificonfigxml", wificonfigxml);
  server.on("/botsloginxml", botsloginxml);
  server.on("/htmlclientesxml", htmlclientesxml);
  server.on("/htmlsendxml", htmlsendxml); 
  server.on("/htmlreceivexml", htmlreceivexml);
  server.on("/recupassxml", recupassxml);
  server.on("/passwordxml", passwordxml);
  server.begin();
  Serial.print("AP CONFIG --> SSID: " + String(APssid) + " / PASS: " + String(APpassword));
  Serial.print("AP IP: "); Serial.println(myIP);
}
////////////////////////////////RESTARTCONFIG//////////////////////////////////////////////////////////////////////////
void restartconfig() { /*REINICIAR ARDUINO CON CONFIGURACIONES DE EEPROM*/
  if (EEPROMr.read(WIFIDIR + DATA_OFFSET) != 255 && EEPROMr.read(WIFIDIR + DATA_OFFSET) != 0) {
    char eepromssid[30]; strcpy(eepromssid, readChar(WIFIDIR));
    int WIFIPASS = WIFIDIR + strlen(eepromssid) + DATA_OFFSET;
    char eeprompass[30]; strcpy(eeprompass, readChar(WIFIPASS));
    Serial.println("EEPROM GET--> SSID: " + String(eepromssid) + " / PASS: " + String(eeprompass));
    String temp = WiFiConnect(eepromssid, eeprompass);
    if (temp == "1") {
      estado=100;//0:UNSET-100:WIFIOK-200:BOTSLOGINOK-300:CLIENTEOK-400:MQTTSETUPOK
      Serial.println("wificonfig: " + String(temp));
      if (EEPROMr.read(BOTSDIR + DATA_OFFSET) != 255 && EEPROMr.read(BOTSDIR + DATA_OFFSET) != 0) {
        char eeprombotsuser[30]; strcpy(eeprombotsuser, readChar(BOTSDIR));
        int BOTSPASS = BOTSDIR + strlen(eeprombotsuser) + DATA_OFFSET;
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
