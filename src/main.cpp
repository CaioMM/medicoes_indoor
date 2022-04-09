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
  // put your main code here, to run repeatedly:
}