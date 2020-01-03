#define _DISABLE_TLS_

#include <ESP8266WiFi.h>
#include <ThingerSmartConfig.h>
#include <Wire.h>
#include <MQ135.h>
#include <dht.h>
#define _DEBUG_

/* PIN D0 = GPIO16 */
static const uint8_t D0   = 16;
static const uint8_t D8 = 15;

//WiFi credentials
#define SSID "NEST UI 2018-powered by ICON+"
#define PASSWORD ""

//thinger.io credentials
#define USERNAME  "mkamal"
#define DEVICE_ID "1111"
#define DEVICE_CREDENTIAL "mkamal"
ThingerSmartConfig thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);


#define RLOAD 22.0
#define RZERO 879.13
float ppmCO2;
void co2Sensor();
int sensorValue;
MQ135 gasSensor = MQ135(A0);

//#define dht_type DHT22
#define dht_pin 2
dht DHT;
//DHT dht(dht_pin, dht_type);
float humidity;
float temperature;
void dhtSensor();
void alert();
const int buzzer = D8;

void setup() {
  //  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(2000);
  while (!Serial) { }
  Serial.println("Device Activated.");
  /* lightSensor */
  Wire.begin();
  DHT.read11(dht_pin);
  analogRead(0); 
  WiFi.begin(SSID, PASSWORD);
  if (WiFi.status() != WL_CONNECTED) {
    delay(10000);
    Serial.print(".");
  }
  Serial.println("Device Connected!");
  thing["AQI"] >> [](pson & out) {
    out["CO2"] = analogRead(0);
    out["temperature"] = DHT.temperature;
    out["humidity"] = DHT.humidity;
  };
  co2Sensor();
  dhtSensor();
  thing.handle();
  thing.write_bucket("1", "AQI");
  delay(15000);
  WiFi.disconnect();
  if (sensorValue > 600 || temperature > 45 || humidity >= 90){
    Serial.println("Danger");
    alert();
  }
  ESP.deepSleep(1800e6);//DeepSleep selama 30 menit
}

void loop() {//Perbocabaan
  ////  if (sensorValue > 510 || temperature >= 50 || humidity >=90)
  ////  {
  ////    Serial.println("Danger");
  ////    alert();
  ////  }
}

void co2Sensor() {
  //  ppmCO2 = gasSensor.getPPM();
  sensorValue = analogRead(0);       // read analog input pin 0
  Serial.print("AirQua=");
  Serial.print(sensorValue, DEC);   // prints the value read
  Serial.println(" PPM");

  // Print for test
  //  Serial.print("CO2 : ");
  //  Serial.println(ppmCO2);
}

void dhtSensor() {
  DHT.read11(dht_pin);
  humidity = DHT.humidity;
  Serial.print("Humidity : ");
  Serial.println(humidity);
  temperature = DHT.temperature;
  Serial.print("Temperature : ");
  Serial.println(temperature);
}

void alert(){
    tone(buzzer, 900);
    delay (1000);
    noTone(buzzer);
    delay (500);
    tone(buzzer, 900);
    delay (1000);
    noTone(buzzer);
  }
