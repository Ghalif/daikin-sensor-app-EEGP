
#include "DHT.h"
#define DHTPIN 5     // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

uint8_t sensor1[8] = { 0x28, 0x02, 0x07, 0x76, 0xE0, 0x01, 0x3C, 0x9E };

void setup() {
  // put your setup code here, to run once:  
  sensors.begin();
   Serial.begin(9600);
   dht.begin();

   Serial.println(F("DHT Humidity:\tDHT Temperature:\tDS18B20 Temperature "));
}

void loop() {
  
   delay(2000);

  sensors.requestTemperatures();

    float sen1 = sensors.getTempCByIndex(1);
  float sen2 = sensors.getTempCByIndex(2);
  float sen3 = sensors.getTempCByIndex(3);
  float sen0 = sensors.getTempCByIndex(0);
 
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("\t");
  Serial.print(h);
  Serial.print("\t");
  Serial.println(t);
//  Serial.print("\t");
//  Serial.println(sen0);
}
