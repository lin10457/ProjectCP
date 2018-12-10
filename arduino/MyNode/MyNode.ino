#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <aREST.h>
#define DHTPIN 2 //PORT:D4
float temperature;
float humidity;

int photoresistor;

void setup()
{
  pinMode(2, OUTPUT);
  // Start Serial
  Serial.begin(115200);

  setupDHT();

  setupARest();
}

void loop()
{

  loopDHT();
  //  loopDHT_Fake();
  loopPS();

  loopARest();
  uploadData();
  //  loopLED();

  if (temperature < 30 && humidity < 60)
  {
    for (int i = 0; i < 5; i++)
    {
      digitalWrite(2, HIGH);
      delay(500);
      digitalWrite(2, LOW);
      delay(500);
    }
  }
  else if (temperature > 30 && humidity < 60)
  {
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(2, HIGH);
      delay(250);
      digitalWrite(2, LOW);
      delay(250);
    }
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
  }
  else if (temperature < 30 && humidity > 60)
  {
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(2, HIGH);
      delay(500);
      digitalWrite(2, LOW);
      delay(150);
    }
  }
  else
  {
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(2, HIGH);
      delay(125);
      digitalWrite(2, LOW);
      delay(125);
    }
  }
}
