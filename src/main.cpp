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
union {
   float dFloat[2];            // 4 bytes
   uint32_t dBytes[2];   //  mapped onto the same storage as myFloat
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

  // update gps information
  smartDelay(10);
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

  // prepare packet to be delivered
  posicao.dFloat[0] = gps.location.lat();
  posicao.dFloat[1] = gps.location.lng();

  uint8_t payload[12];

  //Latitude do Car
  // for (int i = 0; i < 4; i++)
  // {
  //   payload[i] = ((posicao.dBytes[0]) >> (24 - ( 8 * i )) & 0xff);
  // }

  //Longitude Car
  // for (int i = 4; i < 8; i++)
  // {
  //   payload[i] = ((posicao.dBytes[1]) >> (24 - ( 8 * (i - 4) )) & 0xff);
  // }

  // Position
  for (int j = 0; j < 2; j++)
  {
    for (int i = 0; i < 4; i++)
    { 
        payload[i + (j * 4)] = ((posicao.dBytes[j]) >> (24 - ( 8 * (i + (j * 4)) )) & 0xff); 
    }
  }

  //packet sent
  for (int i = 8; i < 12; i++)
  {
    payload[i] = (pkt_sent >> (24 - ( 8 * (i - 8) )) & 0xff);
  }

  rf95.send(payload, sizeof(payload));
  rf95.waitPacketSent();
  pkt_sent+=1;
}