#define DATA_OFFSET 10
WiFiServer phpserver(8080);
extern WiFiClient espClient;
const char espserver[] = "casabots.000webhostapp.com";
String phppayload;int BOTSDIR=50;
/////////////////////////CHANGEPASS/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////CHANGEPASS/////////////////////////////////////////////////////////////////////////////////////////////////////
String phpchangepassword(String oldpass,String newpass1,String newpass2){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/changepassword.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String eeprombotsuser=readString(BOTSDIR);
    String data = "USER="+eeprombotsuser+"&oldpassword="+oldpass+"&newpassword1="+newpass1+"&newpassword2="+newpass2+"";
    //Serial.println(data);
    int httpCode = http.POST(data);//Send the request
    phppayload = http.getString();//Get the response payload
    Serial.println("changepasswordHttpCode: "+String(httpCode));//Print HTTP return code
    http.end();//Close connection
    if(phppayload=="Contraseña Modificada Correctamente."){
      phppayload="1";
      writeString(newpass1,BOTSDIR+DATA_OFFSET+eeprombotsuser.length());
    }
  }else{phppayload="Error de Conexion con Servidor. Contacte Proveedor.";}
  Serial.println("changepasswordpayload: "+String(phppayload));
  return phppayload;
}
////////////////////////////////MQTTINPUT//////////////////////////////////////////////////////////////////////////
////////////////////////////////MQTTINPUT//////////////////////////////////////////////////////////////////////////  
String phpmqttinput(){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/mqttuser.php");//Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");//Specify content-type header
    String data = "mqttuser="+readString(BOTSDIR);
    int httpCode = http.POST(data);//Send the request
    phppayload = http.getString();//Get the response payload
    Serial.println("mqttinputHttpCode: "+String(httpCode));//Print HTTP return code
    http.end();//Close connection
  }else{phppayload="Error de Conexion con Servidor. Contacte Proveedor.";}
  //Serial.println("mqttinputPayload: "+String(phppayload));//Print request response payload
  return phppayload;
}
////////////////////////////////CLIENTES//////////////////////////////////////////////////////////////////////////
////////////////////////////////CLIENTES//////////////////////////////////////////////////////////////////////////  
String phpclientes(){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/clientes.php");//Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");//Specify content-type header
    String data = "USER="+readString(BOTSDIR);
    //Serial.println(data);
    int httpCode = http.POST(data);//Send the request
    phppayload = http.getString();//Get the response payload
    Serial.println("ClientesHttpCode: "+String(httpCode));//Print HTTP return code
    http.end();//Close connection
  }else{phppayload="Error de Conexion con Servidor. Contacte Proveedor.";}
  //Serial.println("ClientesPayload: "+String(phppayload));//Print request response payload
  return phppayload;
}
////////////////////////////////LOGIN//////////////////////////////////////////////////////////////////////////
////////////////////////////////LOGIN//////////////////////////////////////////////////////////////////////////
String phpbotslogin(String BOTSUSER, String BOTSPASS){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    Serial.println("Conexión PHP Exitosa");
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/login.php");//Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");//Specify content-type header
    String data = "user="+BOTSUSER+"&pass="+BOTSPASS+"";
    int httpCode = http.POST(data);//Send the request
    phppayload = http.getString();//Get the response payload
    Serial.println("LoginHttpCode: "+String(httpCode));//Print HTTP return code
    http.end();//Close connection
    if(phppayload=="1"){
      writeString(BOTSUSER,BOTSDIR);
      writeString(BOTSPASS,BOTSDIR+DATA_OFFSET+BOTSUSER.length());
    }
  }else{phppayload="Error de Conexion con Servidor PHP. Contacte Proveedor.";}
  return phppayload;
}
