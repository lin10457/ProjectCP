#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <aREST.h>

char* device_id = "qwa5sz";
const char* ssid = "IOTAP";
const char* password = "raspberry";
WiFiClient espClient;
PubSubClient client(espClient);
aREST rest = aREST(client);
const byte GAS = A0;
const byte swPin = 5;       //D1
const byte infrared_VR_Pin = 4;//D2
const byte gas_LED = 14;        //D5
const byte sw_LED = 12;         //D6
const byte infrared_VR_LED = 13;//D7
const byte BZ_Pin = 15;         //D8

boolean infrared_var, door_var;//紅外&門窗狀態
boolean guard_Flag;       //紅外&門窗觸發

String infrared_val;    //紅外線arest輸出
String swtt, gatt; //門窗&瓦斯(數位)arest輸出
int gas_val;      //瓦斯(類比)arest輸出
unsigned long Time01, Time02;
unsigned long BZ_Time01, BZ_Time02;
int Time_Return = 2000;
int BZ_Time_Return1 = 1000;
int BZ_Time_Return2 = 2000;

void gas()
{
  gas_val = analogRead(GAS);
  if (gas_val < 300)
  {
    gatt = "Safety!!";
    digitalWrite(gas_LED, HIGH);
  }
  else
  {
    gatt = "abnormal";
    digitalWrite(gas_LED, LOW);
  }
}
void door()
{
  if (guard_Flag)
  {
    door_var = digitalRead(swPin);
    if (door_var)
    {
      swtt = "abnormal";
      digitalWrite(sw_LED, LOW);



    }
    else
    {
      swtt = "Safety!!";
      digitalWrite(sw_LED, HIGH);
    }
  }
  else
  {
    swtt = "off";
  }
}
void infrared()
{
  if (guard_Flag)
  {
    infrared_var = digitalRead(infrared_VR_Pin);
    if (infrared_var)
    {
      infrared_val = "abnormal";
      digitalWrite(infrared_VR_LED, LOW);
    }
    else
    {
      infrared_val = "Safety!!";
      digitalWrite(infrared_VR_LED, HIGH);
    }
  }
  else
  {
    infrared_val = "off";
  }
}
int guard(String z)
{
  guard_Flag = !guard_Flag;
  return guard_Flag;
}
void Alarm() //bz副程式
{
  BZ_Time02 = millis();
  if (BZ_Time02 - BZ_Time01 > 0)
  {
    //檢測時間差是否到達
    if (BZ_Time02 - BZ_Time01 >= BZ_Time_Return1)
    {
      tone(BZ_Pin, 2000);
    }
    else if (BZ_Time02 - BZ_Time01 >= BZ_Time_Return2)
    {
      tone(BZ_Pin, 1000);
      BZ_Time01 = millis();
    }
  }
  //Time02爆滿歸0處理
  else if (BZ_Time02 - BZ_Time01 <= 0)
  {
    BZ_Time01 = millis();
  }
}
void setup()
{
  pinMode(GAS, INPUT);
  pinMode(swPin, INPUT);
  pinMode(infrared_VR_Pin, INPUT);
  pinMode(gas_LED, OUTPUT);
  pinMode(sw_LED, OUTPUT);
  pinMode(infrared_VR_LED, OUTPUT);
  pinMode(BZ_Pin, OUTPUT);
  Time01 = millis();
  BZ_Time01 = millis();
  // put your setup code here, to run once:
  Serial.begin(115200);
  client.setCallback(callback);
  guard_Flag = false;
  rest.set_id(device_id);
  rest.set_name("esp8266");
  rest.variable("sw", &swtt);
  rest.variable("infrared", &infrared_val);
  rest.variable("gasdigital", &gatt);
  rest.variable("gasanalog", &gas_val);
  rest.variable("guardvar", &guard_Flag);
  rest.function("guard", guard);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  door();
  infrared();
  gas();
  delay(2000);
  // Set output topic
  char* out_topic = rest.get_topic();
}

void loop() {
  // put your main code here, to run repeatedly:
  Time02 = millis();
  //固定時間回傳
  //檢測 Time02 是否爆滿歸零
  if (Time02 - Time01 > 0)
  {
    //檢測時間差是否到達
    if (Time02 - Time01 >= Time_Return)
    {
      door();
      infrared();
      gas();
      Serial.print("瓦斯:");
      Serial.println(gatt);
      Serial.print("門窗:");
      Serial.println(swtt);
      Serial.print("紅外:");
      Serial.println(infrared_val);
      Time01 = millis();
    }
  }
  //Time02爆滿歸0處理
  else if (Time02 - Time01 <= 0)
  {
    Time01 = millis();
  }
  if (guard_Flag)
  {
    if (gatt == "abnormal" || swtt == "abnormal" || infrared_val == "abnormal")
    {
      Alarm();
    }
    else if (gatt == "Safety!!" && swtt == "Safety!!" && infrared_val == "Safety!!")
    {
      noTone(BZ_Pin);
    }
  }
  rest.handle(client);
}
void callback(char* topic, byte* payload, unsigned int length) {

  rest.handle_callback(client, topic, payload, length);
}
