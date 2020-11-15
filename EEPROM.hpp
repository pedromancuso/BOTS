#define DATA_OFFSET 10
EEPROM_Rotate EEPROMr;
////////////////////////////////EEPROMSETUP//////////////////////////////////////////////////////////////////////////
void eepromsetup(){EEPROMr.size(4);EEPROMr.begin(512);}
////////////////////////////////WRITESTRING//////////////////////////////////////////////////////////////////////////
void writeString(String str, int pos){
  for (int i=0; i<str.length(); i++){
    int s = str[i];
    EEPROMr.write(i+pos+DATA_OFFSET, s);
  }
  EEPROMr.write(str.length()+pos+DATA_OFFSET,'\0');
  EEPROMr.commit();
}
////////////////////////////////READSTRING//////////////////////////////////////////////////////////////////////////
String readString(int pos){
  char data[100]; // Max 100 Bytes
  int len = 0;
  unsigned char k;
  while(k != '\0' && len <= 100){
    k = char(EEPROMr.read(len+pos+DATA_OFFSET));
    data[len] = k;
    len++;
  }
  return String(data);
}
