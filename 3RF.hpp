#include <RCSwitch.h>
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength);
/*static const uint8_t D5 = 14;*/
/*static const uint8_t D2 = 4;*/
RCSwitch RFDev = RCSwitch();
RCSwitch RFRecep = RCSwitch();
void RFSetup(){
  RFDev.enableTransmit(D2);
  RFDev.setProtocol(6);
  RFDev.setPulseLength(533);
  RFDev.setRepeatTransmit(15);
  RFRecep.enableReceive(D5);
  Serial.println("EMISOR RF D2 / Receptor RF D5"); 
}
int RecepRF(){
    if (RFRecep.available()){
      int value = RFRecep.getReceivedValue();   
      if (value == 0) {Serial.print("Unknown encoding");}
      else{
        Serial.print("Codigo RF: ");Serial.print(RFRecep.getReceivedValue());
        Serial.print(" / ");Serial.print(RFRecep.getReceivedBitlength());Serial.print("bit ");
        Serial.print("Protocol: ");Serial.print(RFRecep.getReceivedProtocol());
        Serial.print(" / PulseLength: ");Serial.println(RFRecep.getReceivedDelay());
        const char* b = dec2binWzerofill(RFRecep.getReceivedValue(), RFRecep.getReceivedBitlength());
        Serial.print("Binary: ");Serial.println(b);
      }
      RFRecep.resetAvailable();
    }
  return 1;
} 
int EmisRF(String code){
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.print("EmisionRF: ");Serial.println(code);
  delay(400);
  RFDev.send("1000100000000110010000100101");
  delay(2000);
  digitalWrite(LED_BUILTIN,LOW);
  return 1;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
  static char bin[64];unsigned int i=0;
  while (Dec > 0){bin[32+i++]=((Dec & 1)>0)?'1':'0';Dec=Dec>>1;}
  for (unsigned int j = 0; j< bitLength; j++){if (j >= bitLength - i){bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];}else {bin[j] = '0';}}
  bin[bitLength] = '\0';
  return bin;
}
