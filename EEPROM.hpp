#define DATA_OFFSET 10
EEPROM_Rotate EEPROMr;
void eepromsetup(){EEPROMr.size(4);EEPROMr.begin(512);}
void writeString(String str, int pos){
  for (int i=0; i<str.length(); i++){int s = str[i];EEPROMr.write(i+pos+DATA_OFFSET, s);}
  EEPROMr.write(str.length()+pos+DATA_OFFSET,'\0');
  EEPROMr.commit();
}
char * readChar(int pos){
  char * data=new char[100];int len = 0;unsigned char k;
  while(k != '\0' && len <= 100){k=char(EEPROMr.read(len+pos+DATA_OFFSET));data[len]=k;len++;}
  return data;
}
