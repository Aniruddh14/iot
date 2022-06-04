#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "LED"
#define WIFI_PASS "ONOFFONOFF"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "Aniruddh_Juyal"
#define MQTT_PASS "2cf1d1f814fd43739f31c89ce3192013"

int tv = D4,ac = D0;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/ONOFF");
Adafruit_MQTT_Publish TVStatus = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/TVSTATUS");
Adafruit_MQTT_Subscribe onac = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/ONAC");
Adafruit_MQTT_Publish ACStatus = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/acStatus");


void setup()
{
  Serial.begin(9600);
  


  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(">");
    delay(50);
  }

  Serial.println("OK!");

  //Subscribe to the onoff topic
  mqtt.subscribe(&onoff);
  mqtt.subscribe(&onac);
  pinMode(ac, OUTPUT);
  digitalWrite(ac, HIGH);
  pinMode(tv, OUTPUT);

  digitalWrite(tv, HIGH);

}

void loop()
{
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &onoff)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) onoff.lastread);

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) onoff.lastread, "ON"))
      {
        //active low logic
        digitalWrite(tv, LOW);
        TVStatus.publish("/OFF");
      }
      else if (!strcmp((char*) onoff.lastread, "OFF"))
      {
        digitalWrite(tv, HIGH);
        TVStatus.publish("/ON");

      }
      else
      {
        TVStatus.publish("ERROR");
      }
    }
  
 
  
    //If we're in here, a subscription updated...
    else if (subscription == &onac)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) onac.lastread);

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) onac.lastread, "ON"))
      {
        //active low logic
        digitalWrite(ac, LOW);
        ACStatus.publish("/OFF");
      }
      else if (!strcmp((char*) onac.lastread, "OFF"))
      {
        digitalWrite(ac, HIGH);
        ACStatus.publish("/ON");

      }
      else
      {
        ACStatus.publish("ERROR");
      }
    }
  }
 
}


void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    //    mqtt.disconnect();
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
  }
  Serial.println("MQTT Connected!");
}
