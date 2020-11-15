#define DATA_OFFSET 10
EEPROM_Rotate EEPROMr;
////////////////////////////////EEPROMSETUP//////////////////////////////////////////////////////////////////////////
void eepromsetup(){EEPROMr.size(4);EEPROMr.begin(512);Serial.println("EEPROM INICIADA 512 Kb");}
////////////////////////////////WRITESTRING//////////////////////////////////////////////////////////////////////////
void writeString(String str, int pos){
  for (int i=0; i<str.length(); i++){int s = str[i];EEPROMr.write(i+pos+DATA_OFFSET, s);}
  EEPROMr.write(str.length()+pos+DATA_OFFSET,'\0');
  EEPROMr.commit();
}
////////////////////////////////READCHAR//////////////////////////////////////////////////////////////////////////
char * readChar(int pos){
  char * data=new char[1000];int len = 0;unsigned char k;
  while(k != '\0' && len <= 1000){k=char(EEPROMr.read(len+pos+DATA_OFFSET));data[len]=k;len++;}
  return data;
}
