#define DATA_OFFSET 10
#define WIFIDIR 0
#define BOTSDIR 50
#define ACTUADORESDIR 150
#define PROGRAMASDIR 200
extern bool BOTSCONNECTED;
extern String HORAACTUAL;
/*----------------------------------------------------------------------------------------------------------PROGRAMASCLIENTES------------------------------------*/
String programas[][15]={{},{}};
extern String actuadores[][15];
int qp=0;
int qa=0;
//prgid&nprg&state&days&HHINICIO&HHFIN&dev
void ProgramasHandler(){
  for (int i=0;i<qp;i++){
    if(programas[i][2]=="1" || programas[i][2]=="true"){
      if(programas[i][3].indexOf(DIAACTUAL, 0)!=-1){
        if(programas[i][4]==HORAACTUAL){
          if(programas[i][7]=="0"){
            programas[i][7]="1";
            String * args2 = substr(programas[i][6],"%");
            int j=0;
            Serial.print("Iniciando Programa: ");Serial.println(programas[i][1]);
            while(args2[j]!=""){mqttpublish(args2[j],"1");j++;}
          }
        }
        if(programas[i][5]==HORAACTUAL){
          if(programas[i][7]=="1"){
            programas[i][7]="0";
            String * args2 = substr(programas[i][6],"%");
            int j=0;
            Serial.print("Finalizando Programa: ");Serial.println(programas[i][1]);
            while(args2[j]!=""){mqttpublish(args2[j],"0");j++;}
          }
        }   
      }
    }else{
      if(programas[i][7]=="1"){
        programas[i][7]="0";
        String * args2 = substr(programas[i][6],"%");
        int j=0;
        Serial.print("Finalizando Programa: ");Serial.println(programas[i][1]);
        while(args2[j]!=""){mqttpublish(args2[j],"0");j++;}
      }
    }
  }
}

void nuevoactuador(String nuevoactuador){
  String * args3 = split(nuevoactuador,'&','=');
  int i=0;
  while(args3[i]!=""){
    actuadores[qa][i]=args3[i];
    i++;    
  }
  Serial.print("Nuevo Actuador "+String(qa)+": ");Serial.println(actuadores[qa][1]);
  qa=qa+1;
}


void nuevoprograma(String nuevoprograma){
  String * args3 = split(nuevoprograma,'&','=');
  int i=0;
  while(args3[i]!=""){
    programas[qp][i]=args3[i];
    i++;    
  }
  programas[qp][i]="0";
  Serial.print("Nuevo Programa "+String(qp)+": ");Serial.println(programas[qp][1]);
  qp=qp+1;
}

void eliminarprograma(int IDeliminar){
  if(IDeliminar<999){
    for(int IDeliminar;IDeliminar<qp;IDeliminar++){
      int k=0;
      while(programas[IDeliminar][k]!=""){
        if(IDeliminar!=qp){programas[IDeliminar][k]=programas[IDeliminar+1][k];}
        else{programas[IDeliminar][k]="";}
        k++;
      }
    }
    qp--;
  }else{
    for(int n=0;n<qp;n++){
      int k=0;
      while(programas[n][k]!=""){
        programas[n][k]="";
        k++;
      }
    }qp=0;
  }
}

void bots(String serveruri,String data,char * response){
  if(serveruri=="/changepass.php" || serveruri=="/BOTSlogin.php"){
    if(strcmp(response,"1")==0){
      String * args2 = split(data,'&','=');
      writeString(args2[0],BOTSDIR);
      writeString(args2[1],BOTSDIR+DATA_OFFSET+args2[0].length());  
      BOTSCONNECTED=true;
      Serial.println("Credenciales BOTS guardadas en EEPROM.");
    }
  
  }else if(serveruri=="/cli.php"){
    writeString(response,ACTUADORESDIR);
    String * args2 = substr(response,"$");
    int i=0;
    while(args2[i]!=""){nuevoactuador(args2[i]);i++;}
  
  
  }else if(serveruri=="/prgcli.php"){
    writeString(response,PROGRAMASDIR);
    String * args2 = substr(response,"$");
    eliminarprograma(1000);
    int i=0;
    while(args2[i]!=""){nuevoprograma(args2[i]);i++;}
  
  
  
  }else if(serveruri=="/newprg.php"){
    String * args2 = substr(data,"$");
    Serial.println("Nuevo Programa: ");Serial.println(args2[0]);
    nuevoprograma(args2[0]);


    
  }else if(serveruri=="/prgdel.php"){
    String * args2 = split(data,'&','=');
    for(int j=0;j<=qp;j++){Serial.print("Programa Viejos "+String(qp)+": ");Serial.println(programas[qp][1]);}
    for(int i=0;i<=qp;i++){
      if(args2[0]==programas[i][0]){
        Serial.print("Eliminar ID: ");Serial.println(programas[i][0]);
        eliminarprograma(i);
        }
    }
    for(int j=0;j<=qp;j++){Serial.print("Programa Actuales "+String(qp)+": ");Serial.println(programas[qp][1]);}
  
    
    
    
  
  
  
  }else if(serveruri=="/editstateprg.php"){
    String * args2 = split(data,'&','=');
    for(int i=0;i<=qp;i++){
      if(args2[0]==programas[i][0]){
        Serial.print("VIEJO ESTADO ID ");Serial.print(programas[i][0]);Serial.print(": ");Serial.println(programas[i][2]);
        programas[i][2]=args2[1];
        Serial.print("NUEVO ESTADO ID ");Serial.print(programas[i][0]);Serial.print(": ");Serial.println(programas[i][2]);
        }
    }
  }
}
