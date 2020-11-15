int HoraServidor=165;int MinutosServidor=165;int DiaServidor;int MesServidor;int YearServidor;
String HORAACTUAL;String DIAACTUAL;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
void NTPUpdate(){
  timeClient.begin();
  timeClient.setTimeOffset(-10800);// GMT -3 = -10800
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();  
  HoraServidor  = timeClient.getHours();
  MinutosServidor  = timeClient.getMinutes();
  struct tm *ptm = gmtime ((time_t *)&epochTime);//Get a time structure
  DiaServidor = ptm->tm_mday;
  MesServidor = ptm->tm_mon+1;
  YearServidor = ptm->tm_year+1900;
  //rtc.adjust(DateTime(YearServidor, MesServidor, DiaServidor, HoraServidor, MinutosServidor, 00));
  //Serial.print("Reloj Interno Actualizado por NTP: ");
  //Serial.println(String(HoraServidor)+":"+String(MinutosServidor)+" "+String(DiaServidor)+"/"+String(MesServidor)+"/"+String(YearServidor));
  if(HoraServidor<10){HORAACTUAL = "0"+String(HoraServidor);}else{HORAACTUAL =String(HoraServidor);}
  if(MinutosServidor<10){HORAACTUAL=HORAACTUAL+":0"+String(MinutosServidor);}else{HORAACTUAL=HORAACTUAL+":"+String(MinutosServidor);}
  DIAACTUAL = String(DiaServidor);
}
/*-----------------------------SPLIT---------------------------------------------------------------*/
String * split(String fullstring,char div1,char div2) {
  int pos[20];int pos2[20]; int wexit = 0; String next[20]; String * postarg = new String[20];int q= 0; String sub[20];
  next[0] = fullstring;
  do{
    pos[q] = next[q].indexOf(div1, 0);
    pos2[q] = next[q].indexOf(div1, pos[q]+1);if(pos2[q]==-1){pos2[q]=next[q].length();}
    if (pos[q] != -1) {
      sub[q] = next[q].substring(pos2[q], pos[q]);
      if ((sub[q].indexOf(div2, 0)) > 0) {
        postarg[q] = sub[q].substring(sub[q].indexOf(div2, 0) + 1, sub[q].length());}
        else{postarg[q] = sub[q];}
      next[q + 1] = next[q].substring(pos[q] + 1, next[q].length());
      //Serial.print("postarg["+String(q)+"]: ");Serial.println(postarg[q]);
      q++;
    }else{wexit = 1;}
  }while(wexit == 0);
  return postarg;
}

String * substr(String InStr,char s[2]) {
   String * postarg = new String[20];
   char str[1000];
   char *token;
   int i=0;
   InStr.toCharArray(str,InStr.length()+1);
   token = strtok(str, s);/* get the first token */
   while( token != NULL ) {/* walk through other tokens */
      postarg[i]=token;
      token = strtok(NULL, s);
      i++;
   }
   return postarg;
}
