#define DATA_OFFSET 10

#define EEPROM_SIZE 1
////////////////////////////////EEPROMSETUP//////////////////////////////////////////////////////////////////////////
void eepromsetup(){EEPROM.begin(512);Serial.println("EEPROM INICIADA 512 Kb");}
////////////////////////////////WRITESTRING//////////////////////////////////////////////////////////////////////////
void writeString(String str, int pos){
  for (int i=0; i<str.length(); i++){int s = str[i];EEPROM.write(i+pos+DATA_OFFSET, s);}
  EEPROM.write(str.length()+pos+DATA_OFFSET,'\0');
  EEPROM.commit();
}
////////////////////////////////READCHAR//////////////////////////////////////////////////////////////////////////
char * readChar(int pos){
  char * data=new char[1000];int len = 0;unsigned char k;
  while(k != '\0' && len <= 1000){k=char(EEPROM.read(len+pos+DATA_OFFSET));data[len]=k;len++;}
  return data;
}
