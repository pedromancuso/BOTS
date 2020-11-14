WiFiServer phpserver(8080);
int BOTSDIR=50;
extern WiFiClient espClient;
extern const char espserver[];
extern char SERVER[50];extern char TOPIC[50];extern char PASSWORD[50];extern int SERVERPORT;extern String USERNAME;   
String phppayload;String clientespayload;String changepasswordpayload;String mqttinputpayload;
extern int estado;extern String xmlresponse;
#define DATA_OFFSET 10
/////////////////////////CHANGEPASS/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////CHANGEPASS/////////////////////////////////////////////////////////////////////////////////////////////////////
void changepassword(String oldpass,String newpass1,String newpass2){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/changepassword.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String eeprombotsuser=readString(BOTSDIR);
    String data = "USER="+eeprombotsuser+"&oldpassword="+oldpass+"&newpassword1="+newpass1+"&newpassword2="+newpass2+"";
    Serial.println(data);
    int httpCode = http.POST(data);//Send the request
    changepasswordpayload = http.getString();//Get the response payload
    Serial.print("changepasswordHttpCode: ");Serial.println(httpCode);//Print HTTP return code
    if(changepasswordpayload=="Contraseña Modificada Correctamente."){
        writeString(newpass1,BOTSDIR+DATA_OFFSET+eeprombotsuser.length());
      }
    http.end();//Close connection
  }else{changepasswordpayload="Error de Conexion con Servidor. Contacte Proveedor.";}
  Serial.print("changepasswordpayload: ");Serial.println(changepasswordpayload);
  xmlresponse = changepasswordpayload;
}
////////////////////////////////MQTTINPUT//////////////////////////////////////////////////////////////////////////
////////////////////////////////MQTTINPUT//////////////////////////////////////////////////////////////////////////  
void mqttinput(){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/mqttuser.php");//Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");//Specify content-type header
    String data = "mqttuser="+readString(BOTSDIR);
    Serial.println(data);
    int httpCode = http.POST(data);//Send the request
    mqttinputpayload = http.getString();//Get the response payload
    Serial.print("mqttinputHttpCode: ");Serial.println(httpCode);//Print HTTP return code
    http.end();//Close connection
  }else{
    mqttinputpayload="Error de Conexion con Servidor. Contacte Proveedor.";
    estado=200;
  }
  //Serial.print("mqttinputPayload: ");Serial.println(mqttinputpayload);//Print request response payload
  int q=0;int pos[20];String sub[20];String next[20];
  next[0]=mqttinputpayload;int wexit=0;
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
  String temp = sub[0];temp.toCharArray(SERVER, 50);Serial.print("SERVER: ");Serial.println(SERVER);
  temp = sub[1];SERVERPORT=temp.toInt();Serial.print("SERVERPORT: ");Serial.println(SERVERPORT);
  USERNAME = sub[3];Serial.print("USERNAME: ");Serial.println(USERNAME);
  temp = sub[4];temp.toCharArray(PASSWORD, 50);Serial.print("PASSWORD: ");Serial.println(PASSWORD);
    temp = sub[5];temp.toCharArray(TOPIC, 50);Serial.print("TOPIC: ");Serial.println(TOPIC);
}
////////////////////////////////CLIENTES//////////////////////////////////////////////////////////////////////////
////////////////////////////////CLIENTES//////////////////////////////////////////////////////////////////////////  
String clientes(){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/clientes.php");//Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");//Specify content-type header
    String data = "USER="+readString(BOTSDIR);
    Serial.println(data);
    int httpCode = http.POST(data);//Send the request
    clientespayload = http.getString();//Get the response payload
    Serial.print("ClientesHttpCode: ");Serial.println(httpCode);//Print HTTP return code
    http.end();//Close connection
    mqttinput();
  }else{
    clientespayload="Error de Conexion con Servidor. Contacte Proveedor.";
    estado=200;
  }
  Serial.print("ClientesPayload: ");Serial.println(clientespayload);//Print request response payload
  return clientespayload;
}
////////////////////////////////LOGIN//////////////////////////////////////////////////////////////////////////
////////////////////////////////LOGIN//////////////////////////////////////////////////////////////////////////
String botslogin(String BOTSUSER, String BOTSPASS){
  WiFiClient espClient=phpserver.available();
  if(espClient.connect(espserver,80)){
    Serial.println("Conexión PHP Exitosa");
    HTTPClient http;//Declare object of class HTTPClient
    http.begin("http://casabots.000webhostapp.com/login.php");//Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");//Specify content-type header
    String data = "user="+BOTSUSER+"&pass="+BOTSPASS+"";
    int httpCode = http.POST(data);//Send the request
    phppayload = http.getString();//Get the response payload
    Serial.print("LoginHttpCode: ");Serial.println(httpCode);//Print HTTP return code
    http.end();//Close connection
    if(phppayload=="1"){
      phppayload="Login OK";
      estado=202;
      writeString(BOTSUSER,BOTSDIR);
      writeString(BOTSPASS,BOTSDIR+DATA_OFFSET+BOTSUSER.length());
      clientes();
      }else{estado=201;}//0:UNSET-200:WIFISTARTEDOK-201:PHPSERVEROK-202:BOTSUSEROK-400:WIFIERROR-401:PHPSERVERERROR
  }else{
    phppayload="Error de Conexion con Servidor PHP. Contacte Proveedor.";
    estado=401;   
  }
  return phppayload;
}
