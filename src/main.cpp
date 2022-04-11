#include <Arduino.h>

#include <RH_RF95.h>
#include <TinyGPS++.h> 
#include <SoftwareSerial.h>

#define SF 7
#define BANDWIDTH 125000
#define FREQUENCY 915
#define TXPOWER 20
#define CR4 5

TinyGPSPlus gps;
RH_RF95 rf95;
SoftwareSerial gps_ss(11,12); // arduino's (Rx, Tx)

uint32_t pkt_sent = 0;
uint32_t pkt_received = 0;
union {
   float dFloat[4];            // 4 bytes
   uint32_t dBytes[4];   //  mapped onto the same storage as myFloat
} posicao;

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gps_ss.available())
      gps.encode(gps_ss.read());
  } while (millis() - start < ms);
}

void setup() {
  Serial.begin(9600);
  gps_ss.begin(9600);

  if (!rf95.init())
    Serial.println("init failed");
  Serial.println("Start LoRa Client");

  rf95.setFrequency(FREQUENCY);
  rf95.setTxPower(TXPOWER);
  rf95.setSpreadingFactor(SF);
  rf95.setSignalBandwidth(BANDWIDTH);
  rf95.setCodingRate4(CR4);

}

void loop() {
  if (rf95.available())
  {
   uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
   uint8_t len = sizeof(buf);
   if (rf95.recv(buf, &len))
   {
     pkt_received += 1;
     Serial.print("got request: ");

     posicao.dBytes[0] = (((uint32_t)(buf[0] & 0xff)) << 24) | (((uint32_t)(buf[1] & 0xff)) << 16) | (((uint32_t)(buf[2] & 0xff)) << 8) | (((uint32_t)(buf[3] & 0xff)));
     posicao.dBytes[1] = (((uint32_t)(buf[4] & 0xff)) << 24) | (((uint32_t)(buf[5] & 0xff)) << 16) | (((uint32_t)(buf[6] & 0xff)) << 8) | (((uint32_t)(buf[7] & 0xff)));
     pkt_sent = (((uint32_t)(buf[8] & 0xff)) << 24) | (((uint32_t)(buf[9] & 0xff)) << 16) | (((uint32_t)(buf[10] & 0xff)) << 8) | (((uint32_t)(buf[11] & 0xff)));

     posicao.dFloat[2] = gps.location.lat();
     posicao.dFloat[3] = gps.location.lng();

     Serial.print(posicao.dFloat[2],5);
     Serial.print(" ");
     Serial.print(posicao.dFloat[3],5);
     Serial.print(SF);
     Serial.print(" ");
     Serial.print(BANDWIDTH);
     Serial.print(" ");
     Serial.print(rf95.lastRssi());
     Serial.print(" ");
     Serial.print(rf95.lastSNR());
     Serial.print(" ");
     Serial.print(posicao.dFloat[0],5);
     Serial.print(" ");
     Serial.print(posicao.dFloat[1],5);
     Serial.print(" ");
     Serial.print(pkt_sent, HEX);
     Serial.print(" ");
     Serial.print(pkt_received, HEX);

     Serial.println();
   }
   else
   {
     Serial.println("recv failed");
   }
  }

  smartDelay(10);
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}