#define DATA_OFFSET 10
#define WIFIDIR 0
#define BOTSDIR 50
#define ACTUADORESDIR 150
#define PROGRAMASDIR 200
WiFiServer Httpserver(8080);
HTTPClient Httpclient;
extern bool BOTSCONNECTED;
char espserver[] = "http://casabots.000webhostapp.com";
/*-----------------------------------------------------------------------------------------------------------XMLHttpRequest------------------------------------*/
char * XMLHttpRequest(String Header,String data){
  char phpresponse[1000];
  //Serial.print("Args: ");Serial.println(data);
  WiFiClient WiFiclient=Httpserver.available();
  if(WiFiclient.connect(espserver,80)){
    Httpclient.begin(espserver+Header);
    Httpclient.addHeader("Content-Type", "application/x-www-form-urlencoded");  
    int httpCode = Httpclient.POST(data);    
    if(httpCode==200){
      String phppayload = Httpclient.getString();
      phppayload.toCharArray(phpresponse,phppayload.length()+1);
    }else{strcpy(phpresponse,"Error de Conexion con Servidor PHP. Contacte Proveedor.");}
    Httpclient.end(); 
    Serial.print(Header);Serial.print(": ");Serial.println(String(httpCode));
  }else{strcpy(phpresponse,"Error de Conexion con Servidor PHP. Contacte Proveedor.");}
  //Serial.print(WiFiserver.uri());Serial.println(": ");Serial.println(phpresponse);
  return phpresponse;
}
void HttpRequest(){
  char response[1000];
  strcpy(response, XMLHttpRequest(WiFiserver.uri(),WiFiserver.arg("plain")));
  WiFiserver.send(200, "text/xml",response);
  bots(WiFiserver.uri(),WiFiserver.arg("plain"),response);
}
/*-------------------------------------------------------------------------------------------------------------------SETUP-------------------------------------*/
bool mqttsetup(){
  char response[1000];
  bool ret=false;
  strcpy(response,XMLHttpRequest("/mqttuser.php","&mqttuser="+String(readChar(BOTSDIR))));
  if(strchr(response,'&') == NULL) {ret=true;}
  else{
    String * args2 =split(response,'&','=');
    char SERVER[50];int SERVERPORT;String USERNAME;char PASSWORD[50];char TOPIC[50];
    args2[0].toCharArray(SERVER, 50); 
    SERVERPORT = args2[1].toInt();
    USERNAME = args2[3];
    args2[4].toCharArray(PASSWORD, 50);
    args2[5].toCharArray(TOPIC, 50);
    mqttclientsetup(SERVER,SERVERPORT,USERNAME,PASSWORD,TOPIC);
    ret=false;
  }
  return ret;
}
void mqtt() {
  //POST= &topic&value
  Serial.println(WiFiserver.uri());
  char response[75];
  if(WiFiserver.uri()=="/sendmqtt"){
    String * args2 = split(WiFiserver.arg("plain"),'&','=');
    String temp=mqttpublish(args2[0],args2[1]);
    temp.toCharArray(response,temp.length()+1);
  }
  if(WiFiserver.uri()=="/receivemqtt"){
    strcat(response,actuadorid);
    strcat(response,"&");
    strcat(response,MQTTCALLBACK);
  }
  WiFiserver.send(200, "text/xml",response);
}
bool botssetup(){
  char response[500];
  bool ret=false;
  String data = "&user="+String(readChar(BOTSDIR))+"";
  strcpy(response,XMLHttpRequest("/cli.php",data));
  if(strchr(response,'$') == NULL) {ret=true;}
  else{bots("/cli.php",data,response);ret=false;}
  return ret;
}
bool programassetup(){
  char response[1000];
  bool ret=false;
  String data = "&user="+String(readChar(BOTSDIR))+"";
  strcpy(response,XMLHttpRequest("/prgcli.php",data));
  if(strchr(response,'$') == NULL) {ret=true;}
  else{bots("/prgcli.php",data,response);ret=false;}
  return ret;
}
/*----------------------------------------------------------------------------------------------------------------------WIFI------------------------------------*/
char * WiFiConnect(String STAssid, String STApassword) {
  Serial.println("SSID: " + String(STAssid) + " / PASS: " + String(STApassword));
  char response[100];
  WiFi.begin(STAssid, STApassword);
  int retries = 0;
  Serial.print("Intentando Conexión");
  while(WiFi.status() != WL_CONNECTED && retries <3){
    int contconexion = 0;
    while (WiFi.status() != WL_CONNECTED && contconexion < 20) {contconexion++;delay(500);Serial.print(".");}
    if(WiFi.status() != WL_CONNECTED && retries<2){Serial.println("");Serial.print("Reintenando Conexión");}
    retries++;
  }
  Serial.print("STA IP: "); Serial.println(WiFi.localIP());
  if (WiFi.status() == WL_CONNECTED) {strcpy(response, "1");}
  else{strcpy(response, "Error de conexion");}
  return response;
}
void LandingCheck(){
  char response[1000];
  if(WiFi.status() != WL_CONNECTED){strcpy(response,"OFFLINE");}else{strcpy(response,readChar(WIFIDIR));}
  strcat(response,"&");
  if(BOTSCONNECTED==false){strcat(response,"false");}else{strcat(response,readChar(BOTSDIR));}
  WiFiserver.send(200, "text/xml",response);
}
void WiFiloginphp(){
  Serial.print("WiFiserver.arg('plain'): ");Serial.println(WiFiserver.arg("plain"));
  //WiFi.disconnect(true);
  String * args2 = split(WiFiserver.arg("plain"),'&','='); 
  char response[50]; strcpy(response, WiFiConnect(args2[0], args2[1]));
  if (strcmp(response,"1")==0) {
    writeString(args2[0], WIFIDIR);
    writeString(args2[1], WIFIDIR + args2[0].length() + DATA_OFFSET);
    Serial.println("Credenciales WiFi guardadas en EEPROM.");
  }
  Serial.println("wifilogin: " + String(response));
  WiFiserver.send(200, "text/xml",response);
}
/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.*/
#define ESPLANDING "<html><head><meta charset='utf-8'/><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1, width=device-width, viewport-fit=cover'><title>BOTS</title><link rel='stylesheet' type='text/css' href='botsstyle.css'></head><body class='bodylanding'><div id='weblanding' style='display: none'></div><div id='banner'><!--img src='./img/logo.png' style='width: 125px'--><h2 id='deviceready'>BOTS</h2><ul class='actions'><div class='app'><!--div class='event listening' id='listening' style='display: none;''><h2>Conectando a Dispositivo....</h2></div--><div class='event received'><div id='loginmod'> <h5 class='title' id='cfgWiFi'>CONFIGURACION WIFI &#8964;</h5><div id='cfgWiFi1'><input class='inputland' style='display:none' value='login'/><div><input class='inputland marg' id='WiFiUSER' placeholder='SSID' type='text'/></div><div><input class='inputland marg' id='WiFiPASS' placeholder='PASSWORD' type='pass'/></div><div id='WiFires'></div><div class='actions marg2'><div><input id='WiFi' type='submit' class='sp' value='CONECTAR'/></div></div></div><h5 class='title' id='cfgBOTS'>INICIO DE SESION BOTS &#8964;</h5><div id='cfgBOTS1' style='display: none;'><input class='inputland' style='display:none' value='login'/><div><input class='inputland marg' id='BOTSUSER' placeholder='USER' type='text'/></div><div><input class='inputland marg' id='BOTSPASS' placeholder='PASSWORD' type='pass'/></div><div id='BOTSres'></div><div class='actions marg2'><div><input id='BOTS' type='submit' class='sp' value='LOGIN!'/></div><div id='recoverpass' class='marg'>¿Olvido su contraseña?</div></div></div><div id='recoverpass1' style='display:none'><div>Ingrese E-mail para enviar contraseña de recupero</div><div><input class='inputland marg' id='recuuser' placeholder='Usuario' type='text'/></div><div><input class='inputland marg' id='mailrecu' placeholder='E-mail' type='email'/></div><div id='resrecu'></div><div class='actions marg2'><div class='col-2'><input id='cancelrecu' type='reset' class='alt' value='Cancel'/></div><div class='col-2'><input id='recuperar' type='submit' class='sp' value='Enviar!'/></div></div></div></div></div></div></ul></div></body><!--script type='text/javascript' src='cordova.js'></script--><!--script type='text/javascript' src='js/skel.min.js'></script--><!--script type='text/javascript' src='js/main.js'></script--><!--script type='text/javascript' src='js/util.js'></script--><!--script type='text/javascript' src='js/botsjs.js'></script--><script type='text/javascript' src='botsjs.js'></script></html>"
#define ESPUSER "<html style='color:#777;'><head><meta charset='utf-8'/><meta name='format-detection' content='telephone=no'><meta name='msapplication-tap-highlight' content='no'><link rel='icon' href='data:,'><meta name='viewport' content='initial-scale=1,width=device-width,viewport-fit=cover'><title>BOTS</title><link rel='stylesheet' type='text/css' href='botsstyle.css'></head><body class='userbody'><div id='webuser' style='display:none'></div><div  id='botsuser' style='display:none' value='' ></div><header id='header'><div id='deviceready' class='logo'><a id='logo'>BOTS</a><a id='menu'>&#9776; Menu</a></div><div id='tab'><div id='devtab' class='inn selected'>DISPOSITIVOS</div><div id='prgtab' class='inn'>PROGRAMAS</div></div></header><nav id='botsnav' style='display:none;'><ul><li><a id='pass'>Cambiar Contraseña</a></li><div id='pass1' style='display:none'><div><input class='inputland' style='display:none' value='changepass'/><div><input class='inputland marg' id='oldpass' placeholder='Contraseña Antigua' type='pass'/></div><div><input class='inputland marg' id='newpass1' placeholder='Nueva Contraseña' type='pass'/></div><div><input class='inputland marg' id='newpass2' placeholder='Confirmar Contraseña' type='pass'/></div></div><div id='res'></div><div class='actions marg' style='text-align:center;'><div class='col-2'><button id='cancel' class='alt'/>Cancel</div><div class='col-2'><button id='changepass' class='sp'/>Enviar!</div></div></div><li><a id='end'>Cerrar Sesión</a></li></ul><a id='closenav'>&#10006;</a></nav><div class='wrap' id='loader'><div id='loader2'><div  class='wrap loader'></div></div><div id='loader3'>Cargando...</div></div><div id='main' class='wrap' style='display: none;'><div id='clicont' class='cont'><div id='deviceready'><div id='clicont' class='table-wrap'><table><tbody id='cli'></tbody></table></div></div></div><div id='prgcont' style='display: none;' class='cont'><div id='prg'><div id='prgbar'><div id='addprg'>&#10010;</div><i id='trash' class='trash'>&#x2717;</i></div><div id='payload1' style='display: none;'></div><div id='tablaprg' class='table-wrap'><table><tbody id='prgcli'></tbody></table></div><div id='delprg'><i id='trashbig' class='trash'>&#x2717;</i><div id='del'>Eliminar</div></div></div></div><div id='editprg'><h3 id='editprgtitle'>Modificar Programa</h3><div id='prgid'><div class='marg'><input class='inputland' id='nprgedit' type='text' placeholder='Nombre Programa'/></div><div id='prgdays'><ul class='actions daylist'><li><a id='D1' class='day sp off'>L</a></li><li><a id='D2' class='day sp off'>M</a></li><li><a id='D3' class='day sp off'>M</a></li><li><a id='D4' class='day sp off'>J</a></li><li><a id='D5' class='day sp off'>V</a></li><li><a id='D6' class='day sp off'>S</a></li><li><a id='D7' class='day sp off'>D</a></li></ul></div><div id='prgHH'><div class='time'>Hora Inicio: <input class='inputland HH' type='time' id='HHINICIO'></div><div class='time'>Hora Fin: <input class='inputland HH' type='time' id='HHFIN'></div></div><div id='titletabla'>DISPOSITIVOS</div><div id='tabladev' class='table-wrap'><table><tbody id='devprg'></tbody></table></div><div class='actions editbutttons' id='prgbutton'><div class='col-2'><button id='canceledit' class='alt'/>Cancelar</div><div class='col-2'><button id='saveedit' class='sp'/>Guardar</div></div></div></div></div><footer id='footeruser'><ul><li>&copy; BOTS.</li></ul></footer> </body><!--script src='assets/js/mqttws31.js' type='text/javascript'></script><script type='text/javascript' src='cordova.js'></script><script type='text/javascript' src='js/skel.min.js'></script><script type='text/javascript' src='js/util.js'></script><script type='text/javascript' src='js/main.js'></script><script type='text/javascript' src='js/bots.js'></script--><script type='text/javascript' src='botsjs.js'></script></html>"
#define BOTSSTYLE "/*-------------------------------------------------------------WEBLOGIN/WEBLANDING---------------------------------------------*/*{font-family:'Raleway',Helvetica,sans-serif;}.bodylanding,.bodylogin{background-color:#1E2832;text-align:center;display:inline-block !important;width:100%;color:#fff;margin:0;}#banner{padding:5em 1em 3em 1em;background-color:#1E2832;text-align:center;}.actions{padding:0;width:100%;display:inline-block;max-width:500px;}.inputland{color:white;cursor:pointer;border-radius:4px;border-style:solid;border-width:1px;display:block;outline:0;padding:0 1em;text-decoration:none;line-height:2em;width:100%;background-color:rgba(144,144,144,0.075);border-color:rgba(144,144,144,0.25);height:3.5em;font-size:12pt;font-weight:400;}.marg{margin:1em 0em;}.marg2{margin:2em 0;}.title{cursor:pointer;border: solid white;border-width: 0 0 1px 0;padding: 1em;}.alt,.sp{width:70%;background-color:#51BAA4;transition:background-color 0.2s ease-in-out,color 0.2s ease-in-out;border-radius:2.5em;border:0;cursor:pointer;display:inline-block;font-weight:700;height:2.85em;line-height:2.95em;text-align:center;text-decoration:none;text-transform:uppercase;white-space:nowrap;}.alt{background-color:transparent;box-shadow:inset 0 0 0 2px #51BAA4;color:#51BAA4 !important;}.col-2{width:48%;display:inline-block;}#recoverpass{text-align:center;cursor:pointer;}.event,.received{display:block;}.img{width:7em;}/*-------------------------------------------------------------WEBUSER--------------------------------------------------------*/.userbody{text-align:center;display:inline-block !important;width:100%;margin:0;background-color:rgb(249,252,255);min-width:200px;color:#777}#header{min-width:200px !important;height:44px;line-height:44px;background:#07090c;color:rgba(255,255,255,0.75);cursor:default;text-align:right;width:100%;height:auto;}#logo{float:left;margin-left:0.5em;}a{font-size:1em;cursor:pointer;-moz-transition:color 0.2s ease-in-out;-webkit-transition:color 0.2s ease-in-out;-ms-transition:color 0.2s ease-in-out;transition:color 0.2s ease-in-out;color:#fff;display:inline-block;margin-right:1.25em;text-decoration:none;}#tab{white-space:nowrap;line-height:normal;}.selected{border-bottom:solid 4px white;transition:border-width 0.1s linear 0.1s;}.inn{display:inline-block;width:49%;cursor:pointer;font-size:15px;padding:0.5em 0;text-align:center;color:white;white-space:nowrap;}#botsnav{text-align:left; transition:transform 0.5s ease,box-shadow 0.5s ease,visibility 0.5s;padding:2.5em 1.75em;background:#07090c;color:#fff;height:100%;max-width:80%;overflow-y:auto;position:fixed;right:0;text-transform:uppercase;top:0;min-width:235px;z-index:10002;}#closenav{height:4em;line-height:4em;text-decoration:none;transition:color 0.2s ease-in-out;border:0;color:#a5a9ad;cursor:pointer;display:block;padding-right:1.25em;position:absolute;right:0;text-align:right;top:0;vertical-align:middle;width:7em;-webkit-tap-highlight-color:rgba(0, 0, 0, 0);}ul{padding:0;list-style:none;}li{line-height:3.5em;border:solid white;border-width:0 0 1px 0;}.wrap{position:absolute;padding:2% 0 0 0;text-align:center;width:100%;left:50%;transform:translateX(-50%);}.cont{margin-left:auto;margin-right:auto;width:90% !important;}.table-wrap{overflow-y:auto;}table{margin:0 0 2em 0;width:100%;border-collapse:collapse;border-spacing:0;display:table;}tbody{display:table-row-group;}table tbody tr:nth-child(2n+1){background-color:rgba(144,144,144,0.075);}table tbody tr{border-color:rgba(144,144,144,0.25);border-style:solid;border-width:1px;border-left:0;border-right:0;display:table-row;}table td{padding:0.75em 0.75em;}.mytd{width:49%;text-align:left;}.right{width:49%;text-align:right;}.time{display:inline-block;width:48%;height:unset;}.HH{display:inline-block;height:unset;width:48%;color:black;padding: 0;text-align: center;}input[type='time']::-webkit-calendar-picker-indicator{-webkit-appearance:none;display:none;}#prg{display:inline-block;width:100%;height:100%;}#prgbar{width:100%;display:inline-block;text-align:right;}#addprg{display:inline-block;margin:0 1em;cursor:pointer;}#trashbig{font-size:2em;}#del{font-size:1em;}.trash:before{-moz-osx-font-smoothing:grayscale;-webkit-font-smoothing:antialiased;font-style:normal;font-weight:normal;text-transform:none !important;}#delprg{position:absolute;width:90%;bottom:5px;display:none;cursor:pointer;}.delop{height:3em}#prgcli tr{cursor:pointer;}#editprg{max-width:500px;padding:0 1em; margin:1em 0;overflow:hidden; display:none;cursor:pointer;width:90%;color:#1e2832;height:97%;border:solid;border-width:0 2px;}#editprgtitle{text-align:left;margin:0;}#nprgedit{height:auto;color:black;}#prgdays ul{margin-bottom:0px;}#prgHH{margin:2% 0 2% 0;width:100%;display:inline-block;}#titletabla{margin:2% 0 2% 0;text-align:left;border:solid lightgrey;padding:0 0 1% 0;border-width:0 0 1px 0;}.day{min-width:unset;cursor:pointer;font-size:0.8em;display:block;margin:0 0 1em 0;width:100%;}.daylist{display:table;table-layout:fixed;margin-left:-0.5em;}.daylist li{display:table-cell;padding:0 0 0 0.5em;cursor:pointer;}.off{cursor:default;opacity:0.25;}.editbutttons{padding:0.5em 0 0.5em 0;position:absolute;bottom:0;left:50%;transform:translateX(-50%);background-color:white;}.switch{position:relative;display:inline-block;height:34px;margin:0 1em;}.switch input{opacity:0;width:0;height:0;display:none;}label{width:1em;height:1em;padding:0 1em;display:block;font-size:0.9em;font-weight:700;margin:0 0 1em 0;}.sliderround{border-radius:34px;width:60px;position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;-webkit-transition:.4s;transition:.4s;}input[type='checkbox']:checked+.sliderround{background-color:#2196F3;}input[type='checkbox']:checked+.sliderround:before{content:'';-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px);}.sliderround:before{content:'';border-radius:50%;position:absolute;height:26px;width:26px;left:4px;bottom:4px;background-color:white;-webkit-transition:.4s;transition:.4s;}.square{cursor:pointer;position:relative;}input[type='checkbox']:checked+.square:before{content:'✔';}.square:before{content:'';background-color:rgba(144,144,144,0.075);border-color:#51BAA4;color:#25383B;border-radius:4px;border-style:solid;border-width:1px;display:inline-block;height:2.1em;line-height:2.0125em;position:absolute;text-align:center;top:0;width:2.1em;-moz-osx-font-smoothing:grayscale;-webkit-font-smoothing:antialiased;font-family:FontAwesome;font-style:normal;font-weight:normal;text-transform:none !important;}footer{position:fixed;bottom:0;width:100%;background-color:#1E2832;color:rgba(255,255,255,0.75);text-align:center;height:35px;}footer ul{margin-block-start:0;}footer ul li{line-height:35px;}/*-------------------------------------------------------------LOADER--------------------------------------------------------*/.loader{border: 10px solid #f3f3f3;border-top: 10px solid #3498db;border-radius: 50%;width: 3em;left: 48%;position: absolute;height: 3em;animation: spin 2s linear infinite;padding: 0;}#loader2{height: 5em;}@keyframes spin {0%{transform:rotate(0deg);}100%{transform: rotate(360deg);}}"
//#define BOTSSCRIPT "async function recupassphp(){if(val('mailrecu')!=''){if(val('recuuser')!=''){iH('resrecu',await myajax('&user='+ID('recuuser').value+'&email='+ID('mailrecu').value+'','/recupass.php'));}else{iH('resrecu','Complete Usuario');}}else{iH('resrecu','Complete E-mail');}}async function changepassphp(){var msg=await myajax('&oldpass='+ID('oldpass').value+'&newpass1='+ID('newpass1').value+'&newpass2='+ID('newpass2').value+'','/changepass.php');if(msg==1){iH('res','Contraseña Modificada Correctamente.');}else{iH('res',msg);}}async function editstateprgphp(u){var st;if (ID('prgstate'+u).checked){st=true;}else{st=false;}var msg = await myajax('&ID='+u+'&ESTADO='+st+'','/editstateprg.php');if (msg==1) {ID('prgstate'+u).checked=st;}else{ID('prgstate'+u).checked!=st;alert('resp: '+msg);}prgping();}async function newprgphp(){var prgid=val('prgid');var nprg=val('nprgedit');if(nprg!=''){if ((nprg.split('&').length<=1)&&(nprg.split('$').length<=1)){var days='';for (var i=1;i<=7;i++){if(val('D'+i)==true){days+='D'+i;}}if(days!=''){var HHINICIO=val('HHINICIO');if(val('HHINICIO')!=''){var HHFIN=val('HHFIN');if(val('HHFIN')!=''){var dev='%';for (var i=0;i<res.length;i++){if(ID('devprg'+res[i][0]).checked==true){dev+=res[i][0]+'%';}}if(dev!='%'){var sta =1;if(prgid!=0){sta=ID('prgstate'+prgid).checked;}var msg=await myajax('&prgid='+prgid+'&nprg='+nprg+'&state='+sta+'&days='+days+'&HHINICIO='+HHINICIO+'&HHFIN='+HHFIN+'&dev='+dev+'','/newprg.php');if (msg.split('NID').length==2) {var NID=msg.split('NID')[1];var preslen = Object.keys(prgres).length;var newp=(NID+'&'+nprg+'&'+sta+'&'+days+'&'+HHINICIO+'&'+HHFIN+'&'+dev+'&$');if(preslen==0) {auxvacio('prgcli',0);}if (prgid==0) {prgres[preslen]=newp.split('&');auxmax(parseInt(NID));auxnewprg(NID,nprg);}else{for (var i=0;i<preslen;i++){if (prgres[i][0]==NID){prgres[i]=newp.split('&');iH('nprg'+NID,nprg);ID('prgstate'+NID).checked=sta;}}}}else{alert('resp: '+msg);}epd(0);auxstateprg();}else{alert('DISPOSITIVOS NO SELECCIONADOS');}}else{alert('HH FIN INCORRECTA');}}else{alert('HH INICIO INCORRECTA');}}else{alert('SIN DIAS SELECCIONADOS');}}else{alert('NOMBRE DE PROGRAMA NO PUEDE CONTENTER $ o &');}}else{alert('INGRESE NOMBRE DE PROGRAMA');}prgping();}async function delprgphp(){var prgdel='$';for (var i=0;i<Object.keys(prgres).length;i++){if(ID('del'+prgres[i][0]).checked==true){prgdel+=prgres[i][0]+'$';}}var msg=await myajax('&prgdel='+prgdel+'','/prgdel.php');if (msg==1) {var len=0;for (var i=0;i<Object.keys(prgres).length;i++){if(ID('del'+prgres[i][0]).checked==true){var row = document.getElementById('prg'+prgres[i][0]);row.parentNode.removeChild(row);prgres.splice(i, 1);i--;}}if(Object.keys(prgres).length==0) {if (document.body.contains(ID('addprg'))==false){auxvacio('prgcli',1);}}}else{alert('resp: '+msg);}delmenudisplay();prgping();}/*-------------------------------------CSSDISPLAY----------------------------------------------------------------------------------*/function tog(id){if(di(id)=='none'){tg(id,'block');}else{tg(id,'none');}}function diatog(x){ID(x).value=!val(x);if(val(x)==false){cN(x,'day sp off');}if(val(x)==true){cN(x,'day sp');}}function delmenudisplay(){var x='delprg';var x1=document.getElementsByClassName('delop');var x2=document.getElementsByClassName('prgswitch');if (di(x)=='none') {tg(x,'table-cell');for(i=0;i<x1.length;i++){x1[i].style.display='inline-block';x2[i].style.display='none';}}else{tg(x,'none');for(i=0;i<x1.length;i++){x1[i].style.display='none';x2[i].style.display='inline-block';}}}function devdisplay(s){tg('prg','inline-block');tg('editprg','none');tg('delprg','none');cN('main','wrap');if(s==1){cN('devtab','inn selected');cN('prgtab','inn');tg('clicont','block');tg('prgcont','none');}else if(s==0){cN('devtab','inn');cN('prgtab','inn selected');tg('clicont','none');tg('prgcont','block');if (di('delprg')!='none') {delmenudisplay();}auxstateprg();}}function epd(IDx){if(IDx==0){ID('prgid').value='0';iH('editprgtitle','NUEVO PROGRAMA');ID('nprgedit').value='';ID('HHINICIO').value='';ID('HHFIN').value='';for (var i=1;i<=7;i++){ID('D'+i).value=false;cN('D'+i,'day sp off');}for (var i=0;i<res.length;i++){ID('devprg'+res[i][0]).checked=false;}}else{iH('editprgtitle','MODIFICAR PROGRAMA');for (var i=0;i<Object.keys(prgres).length;i++){if(prgres[i][0]==IDx){ID('prgid').value=prgres[i][0];ID('nprgedit').value=prgres[i][1];var days=prgres[i][3].split('D');for (var d=0;d<=days.length-1;d++){if(days[d]!='D'&&days[d]!=''){diatog('D'+days[d]);}}ID('HHINICIO').value=auxcortarHH(prgres[i][4]);ID('HHFIN').value=auxcortarHH(prgres[i][5]);var dev =prgres[i][6].split('%');for (var v=0;v<dev.length;v++){if(dev[v]!='%'&&dev[v]!=''){ID('devprg'+dev[v]).checked=true;}}}}}var x='prg';var y='editprg';if(di(x)=='none'){tg(x,'inline-block');tg(y,'none');cN('main','wrap');}else{tg(x,'none');tg(y,'inline-block');cN('main','wrap border');}ID('tabladev').style.height=(((ID('prgbutton').getBoundingClientRect().top)-(ID('titletabla').getBoundingClientRect().bottom)))+'px';}var auxcortarHH=function(string){var cut=[];cut=string.split(':');if (cut[0].length==1) {cut[0]='0'+cut[0];}if (cut[1].length==1) {cut[1]='0'+cut[1];}return (cut[0]+':'+cut[1]);};/*-------------------------------------MQTT----------------------------------------------------------------------------------*/"
#define BOTSSCRIPT "var path = window.location.pathname;var filename = path.substring(path.lastIndexOf('/')+1);var page= filename.substring(3,filename.indexOf('.'));var disp= filename.substring(0,3);var botsuser;var dns='';var client;var clientId;var connect = false;console.log('page='+page+' / disp='+disp);var ID=function(id){return document.getElementById(id);};var di=function(id){return ID(id).style.display;};var val=function(id){return ID(id).value;};var tg=function(id,sta){return ID(id).style.display=sta;};var iH=function(id,con){return document.getElementById(id).innerHTML=con;};var cN=function(id,con){return document.getElementById(id).className=con;};var app={initialize:function(){if(disp=='app'){document.addEventListener('deviceready', this.onDeviceReady.bind(this),false);}else{document.addEventListener('deviceready', this.onDeviceReady());}},onDeviceReady:function(){this.receivedEvent('deviceready');},receivedEvent:function(id){if (page=='landing') {if (disp=='app') {dns='https://bots.net.ar';console.log('window.localStorage.getItem(userclient): '+ window.localStorage.getItem('userclient'));if (window.localStorage.getItem('userclient')){window.location.href='./appuser.html';}else{tg('listening','none');tg('received','block');tog('cfgBOTS1');}}else if(disp=='esp'){landing();}}else if(page=='user'){if (disp=='app'){console.log('window.localStorage.getItem(userclient): '+ window.localStorage.getItem('userclient'));dns='https://bots.net.ar';botsuser=window.localStorage.getItem('userclient');}user();}}};app.initialize();/*-------------------------------------MAPEO----------------------------------------------------------------------------------*/document.addEventListener('click',function(e){switch (e.target&&e.target.id){case ('menu'):tog('botsnav');break;case ('main'):if(di('botsnav')!='none'){tog('botsnav');};break;case ('closenav'):tog('botsnav');break;case ('WiFi'):login('WiFi');break;case ('BOTS'):login('BOTS');break;case ('cancel'):tog('pass1');break;case ('pass'):tog('pass1');break;case ('devtab'):devdisplay(1);break;case ('prgtab'):devdisplay(0);break;case ('addprg'):epd(0);break;case ('addprg2'):epd(0);break;case ('canceledit'):epd(0);break;case ('addprg'):epd(0);break;case ('cfgBOTS'):tog('cfgBOTS1');break;case ('cfgWiFi'):tog('cfgWiFi1');break;case ('trash'):delmenudisplay();break;case ('recoverpass'):tog('recoverpass1');tog('cfgBOTS1');break;case ('cancelrecu'):tog('recoverpass1');tog('cfgBOTS1');break;case ('changepass'):changepassphp();break;case ('delprg'):delprgphp();break;case ('trashbig'):delprgphp();break;case ('del'):delprgphp();break;case ('saveedit'):newprgphp();break;case ('recuperar'):recuperarphp();break;case ('end'):botsend();break;default:break;}for (var y=0;y<=max;y++){if(e.target && e.target.id==('prg'+y)) {epd(i);}if(e.target && e.target.id==('nprg'+y)) {epd(y);}if(e.target && e.target.id==('nprgright'+y)) {epd(y);}if(e.target && e.target.id==('check'+y)) {check(y);}if(e.target && e.target.id==('prgstate'+y)) {editstateprgphp(y);}}for (var y=1;y<=7;y++){if(e.target&&e.target.id==('D'+y)){diatog('D'+y);}}});function myajax(stringpost,endpoint){return new Promise(resolve=>{setTimeout(()=>{var xmlHttp=new XMLHttpRequest();xmlHttp.onreadystatechange=function(){if(xmlHttp.readyState===4&&xmlHttp.status===200){try{resolve(xmlHttp.response);}catch(err){console.log(err);alert('resp: '+err);}}};xmlHttp.open('POST',dns+endpoint,true);xmlHttp.setRequestHeader('Content-type','application/x-www-form-urlencoded');xmlHttp.send(stringpost);}, 2000);});}function user(){var HeaderHeight=ID('header').getBoundingClientRect().height;var footerHeight=ID('footeruser').getBoundingClientRect().height;var mainHeight=document.body.clientHeight-HeaderHeight-footerHeight;ID('main').style.top=HeaderHeight+'px';ID('main').style.bottom=footerHeight+'px';setbotsuser();}async function setbotsuser(){if (disp!='app') {botsuser=await myajax(null,'/retrieveuser.php');}if (botsuser!=''){cliphp();}else{setTimeout(setbotsuser,1000);}}async function landing(){var msg=await myajax(null,'/LandingCheck');var est=msg.split('&');if(est[0]!='OFFLINE'){iH('cfgWiFi',`CONFIGURACION WIFI <span style='color: green;'>✔</span>`);iH('WiFires','WiFi conectado a Red: '+ est[0]);tog('cfgWiFi1');tog('cfgBOTS1');}if(est[1]!='false'){iH('botsres','BOTS USER: '+ est[1]);iH('cfgBOTS',`INICIO DE SESION BOTS <span style='color: green;'>✔</span>`);}}function prgping(){if(disp!='esp'){sendmqtt('0','ProgramsChange');}}/*-------------------------------------HTTP----------------------------------------------------------------------------------*/async function login(NAME){iH(NAME+'res','Ingresando...');var msg=await myajax('&user='+ID(NAME+'USER').value+'&pass='+ID(NAME+'PASS').value+'','/'+NAME+'login.php');if(msg=='1'){iH('cfg'+NAME,'INICIO DE SESION '+BOTS+`<span style='color: green;'>✔</span>`);tog('cfg'+NAME+'1');if(dns!=''){window.localStorage.setItem('userclient', ID(NAME+'USER').value);window.location.href='./appuser.html';}else{window.location.href='/';}}else{iH(NAME+'res',msg);iH('cfg'+NAME,'INICIO DE SESION '+NAME+' &#8964;');}}async function botsend(){await myajax(null,'/botsend.php');if (disp=='app') {window.localStorage.clear();}window.location.href=disp+'landing.html';}/*-------------------------------------PHP----------------------------------------------------------------------------------*/var max=0;var res=[];async function cliphp(){res=[];iH('cli','');var msg=await myajax('&user='+botsuser+'','/cli.php');res = auxnewlist('cli',msg);prgcliphp();}var prgres=[];async function prgcliphp(){prgres=[];var msg=await myajax('&user='+botsuser+'','/prgcli.php');iH('prgcli','');iH('devprg','');prgres = auxnewlist('prgcli',msg);auxlistdev();setTimeout(mqttinput,1000);}function auxnewlist(NAME,msg){var temp=[];var progx=msg.split('$');if(progx.length>1){for (var i=0;i<progx.length-1;i++){temp[i]=progx[i].split('&');temp[i].splice(0, 1);for (var k = 0;k<=temp[i].length-1; k++) {temp[i][k]=temp[i][k].split('=')[1];}auxmax(parseInt(temp[i][0]));auxnew(NAME,temp[i][0],temp[i][1]);}if(NAME=='prgcli'){auxstateprg();}}else{auxvacio(NAME,1);}return temp;}function auxnew(NAME,NID,NIDNAME){if (NAME=='prgcli'){ID(NAME).innerHTML+=`<tr id='prg`+NID+`'><td class='mytd' id='nprg`+NID+`'>`+NIDNAME+`</td><td id='nprgright`+NID+`' class='mytd right'><label class='switch prgswitch'><input type='checkbox' id='prgstate`+NID+`'><div class='sliderround'></div></label><label class='delop switch' style='display: none;' id='delprg`+NID+`'><input type='checkbox' id='del`+NID+`'><div class='square'></div></label></td></tr>`;}if (NAME=='cli'){ID(NAME).innerHTML+=`<tr><td class='mytd'>`+NIDNAME+`</td><td class='mytd right'><label class='switch' id='label`+NID+`'><input type='checkbox' id='check`+NID+`'><div class='sliderround'></div></label></td></tr>`;}}function auxvacio(show){if (NAME=='prgcli') {if (show==1) {ID(NAME).innerHTML+=`<tr id='addprg'><td id='addprg2'>+ Agregar prg</td></tr>`;}else{ID('addprg').parentNode.removeChild(ID('addprg'));}}if (NAME=='cli') {ID(NAME).innerHTML+=`<tr><td id='nodev'>NO DEVS</td></tr>`;}}function auxlistdev(){for (var j=0;j<Object.keys(res).length;j++){ID('devprg').innerHTML+=`<tr><td class='mytd'>`+res[j][1]+`</td><td class='mytd right'><label class='switch'><input type='checkbox' id='devprg`+res[j][0]+`'><div class='square'></div></label></td></tr>`;}}function auxmax(NMAX){if(NMAX>=max) {max=NMAX;}}function auxstateprg(){for (var i=0;i<Object.keys(prgres).length;i++){var u=ID('prgstate'+prgres[i][0]);if(prgres[i][2]==1 || prgres[i][2]==true || prgres[i][2]=='true'){u.checked=true;}else{u.checked=false;}}}async function mqttinput(){if (disp=='esp') {console.log('espmqttinput');onMessageArrived();tg('loader','none');tg('main','block');}else{console.log('app/webmqttinput');var message=await myajax('&mqttuser='+botsuser+'','/mqttuser.php');var mqttdata=message.split('&');if(mqttdata.length>1){var mqttvar=[];for (var i=0;i<mqttdata.length;i++){mqttvar[i]=mqttdata[i].split('=');}if(disp=='app'){url+='tcp://';}url+=mqttvar[1][1];if(disp=='app'){port=parseInt(mqttvar[2][1]);}else{port = parseInt(mqttvar[3][1]);}userName=mqttvar[4][1];password=mqttvar[5][1];topic=mqttvar[6][1];setTimeout(mqttconn,1000);}else{setTimeout(mqttinput,6000);}}}/*-------------------------------------ESP----------------------------------------------------------------------------------*/async function sendmqtt(idu,dato){await myajax('&topic:'+idu+'&value:'+dato,'/sendmqtt');}var tempres;async function onMessageArrived(a){var resp=await myajax(null,'/receivemqtt');tempresp=resp;if (resp!=null && resp!='' ) {if (resp.split('&')[0]!='0') {ID('check'+resp.split('&')[0]).checked=parseInt(resp.split('&')[1]);}}}function check(idx){if(ID('check'+idx).checked===true){sendmqtt(idx,'1');}else{sendmqtt(idx,'0');}}var url='';var port;var userName;var password;var topic;"
void esplanding(){WiFiserver.send(200,"text/html", ESPLANDING);}
void espuser(){Serial.println("espuserin");WiFiserver.send(200,"text/html", ESPUSER);Serial.println("espuserout");}
void deploy(){
  if (WiFi.status() == WL_CONNECTED && BOTSCONNECTED==true){WiFiserver.sendHeader("Location","/espuser.html",true);}
  else{WiFiserver.sendHeader("Location","/esplanding.html",true);}
  WiFiserver.send(302, "text/plain","");
} 

void botsjsjs(){WiFiserver.send(200,"text/javascript", BOTSSCRIPT);}
void botsstylecss(){WiFiserver.send(200,"text/css", BOTSSTYLE);}
void retrieveuserphp(){Serial.println("retrieveuser");WiFiserver.send(200, "text/xml",readChar(BOTSDIR));}
void botscerrarsesionphp(){BOTSCONNECTED=false;deploy();}
void wifisetup(){
  char APssid[25];
  char APpassword[20];
  WiFi.macAddress().toCharArray(APpassword, WiFi.macAddress().length() + 1);
  strcpy(APssid, "BOTS-");
  strcat(APssid, APpassword);
  WiFi.disconnect(true);
  ESP.eraseConfig();
  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  /*HTTP*/
  WiFiserver.on("/", deploy);
  WiFiserver.on("/esplanding.html", esplanding);
  WiFiserver.on("/espuser.html", espuser);
  WiFiserver.on("/LandingCheck", LandingCheck);   
  WiFiserver.on("/botsstyle.css",botsstylecss);
  WiFiserver.on("/botsjs.js",botsjsjs);
  WiFiserver.on("/retrieveuser.php",retrieveuserphp); 
  WiFiserver.on("/botsend.php",botscerrarsesionphp);
  WiFiserver.on("/WiFilogin.php", WiFiloginphp);
  /*MQTT*/
  WiFiserver.on("/sendmqtt", mqtt); 
  WiFiserver.on("/receivemqtt", mqtt);
  /*PHP*/
  WiFiserver.on("/prgcli.php", HttpRequest);
  WiFiserver.on("/BOTSlogin.php", HttpRequest);
  WiFiserver.on("/cli.php", HttpRequest);
  WiFiserver.on("/prgdel.php", HttpRequest); 
  WiFiserver.on("/newprg.php", HttpRequest); 
  WiFiserver.on("/edistateprg.php", HttpRequest); 
  WiFiserver.on("/recupass.php", HttpRequest);
  WiFiserver.on("/changepass.php", HttpRequest);
  WiFiserver.onNotFound(deploy);
  WiFiserver.begin();
  Serial.print("AP IP: ");Serial.print(myIP);Serial.println(" /// SSID: " + String(APssid) + " /// PASS: " + String(APpassword));
}
void wifiloop(){WiFiserver.handleClient();}
/*---------------------------------------------------------------------------------------------------------------------------------RESTART-----------------------*/
bool botseepromrestart() {
  bool ret=false;
  if (EEPROMr.read(BOTSDIR + DATA_OFFSET) != 255 && EEPROMr.read(BOTSDIR + DATA_OFFSET) != 0) {
    char eeprombotsuser[30]; strcpy(eeprombotsuser, readChar(BOTSDIR));
    char eeprombotspassword[30]; strcpy(eeprombotspassword, readChar(BOTSDIR + strlen(eeprombotsuser) + DATA_OFFSET));
    if(strcmp(XMLHttpRequest("/BOTSlogin.php","&user="+String(eeprombotsuser)+"&pass="+String(eeprombotspassword)+""),"1")==0){
      Serial.println("Credenciales BOTS recuperadas de EEPROM.");
      BOTSCONNECTED=true;     
    }else{Serial.println("Configuracion BOTS EEPROM Fallada. Continuar por AP...");}
  }else{Serial.println("EEPROM sin credenciales BOTS. Continuar por AP...");}
  return ret;
}
bool wifieepromrestart() {
  bool ret=false;
  if (EEPROMr.read(WIFIDIR + DATA_OFFSET) != 255 && EEPROMr.read(WIFIDIR + DATA_OFFSET) != 0) {
    Serial.print("EEPROM GET--> ");
    char eepromSSID[30]; strcpy(eepromSSID, readChar(WIFIDIR));
    char eepromPASS[30]; strcpy(eepromPASS, readChar(WIFIDIR + strlen(eepromSSID) + DATA_OFFSET));
    if(strcmp(WiFiConnect(eepromSSID,eepromPASS),"1")==0){
      Serial.println("Credenciales WiFi recuperadas de EEPROM.");
      ret=false;}
    else{
      Serial.println("Configuracion WiFi EEPROM Fallada. Reiniciando Red AP...");
      wifisetup();
    }
  }else{Serial.println("EEPROM sin credenciales WiFi. Continuar por AP...");}
  return ret;
}