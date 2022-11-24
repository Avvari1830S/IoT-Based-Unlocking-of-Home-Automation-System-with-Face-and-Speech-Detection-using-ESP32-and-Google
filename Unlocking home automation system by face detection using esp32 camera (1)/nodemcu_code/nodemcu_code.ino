#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define load1 D1
#define load2 D2
#define WLAN_SSID       "project1"
#define WLAN_PASS       "123456789"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "KManikanta"
#define AIO_KEY         "aio_nooQ10PcKATHnpQeSLYs389LvDWW"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light");
Adafruit_MQTT_Subscribe fan = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/fan");

void MQTT_connect();

int k;

void setup() {
  Serial.begin(115200);

  pinMode(D0, INPUT);
  pinMode(load1, OUTPUT);
  pinMode(load2, OUTPUT);
  digitalWrite(load1, LOW);
  digitalWrite(load2, LOW);

   Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&light);
  mqtt.subscribe(&fan);
  
  MQTT_connect();

}

void loop() {
  k = digitalRead(D0);
  Serial.println(k);
  if(k == 1){
    MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &light) {
      Serial.print(F("Got: "));
      Serial.println((char *)light.lastread);
       if(!strcmp((char*) light.lastread, "ON")){
        Serial.println("light On");
        digitalWrite(load1, HIGH);
      }
      else if(!strcmp((char*) light.lastread, "OFF")){
        Serial.println("light Off");  
        digitalWrite(load1, LOW);      
      }
    }
    if (subscription == &fan) {
      Serial.print(F("Got: "));
      Serial.println((char *)fan.lastread);
       if(!strcmp((char*) fan.lastread, "ON")){
        Serial.println("fan On");
        digitalWrite(load2, HIGH);
      }
      else if(!strcmp((char*) fan.lastread, "OFF")){
        Serial.println("fan Off"); 
        digitalWrite(load2, LOW);       
      }
    }
  }
  }
  delay(1000);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
