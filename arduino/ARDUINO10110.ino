#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <aREST.h>
char* device_id = "vrtest";
const char* ssid = "IOTAP";
const char* password = "raspberry";

ESP8266WebServer server(80);
// Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Create aREST instance
aREST rest = aREST(client);

//const int led = 2;

int infrared_VR_Pin = 5;
int infrared_LED_Pin = 0;
int BuzzerPin = 4;
boolean infrared_var = LOW;
String vr_val="";
void callback(char* topic, byte* payload, unsigned int length) {

  rest.handle_callback(client, topic, payload, length);

}
void handleNotFound() {
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}

void vrs()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/json", String("{\"VRPinn\":") + digitalRead(5) + "}");
  //server.send(200, "text/plain", String("VRPinn:") + digitalRead(5));


  infrared_var = digitalRead(infrared_VR_Pin);
  Serial.print("入侵警報:");
  //Serial.print(infrared_var);
  if (infrared_var == HIGH)
  {
    digitalWrite(infrared_LED_Pin, HIGH);
    Serial.println("發現入侵者!!!");
    vr_val="發現入侵者!!!";
    
    Alarm();
  }
  else if (infrared_var == LOW)
  {
    digitalWrite(infrared_LED_Pin, LOW);
    Serial.println("安全!!");
    noTone(BuzzerPin);
    vr_val="安全!!";
  }

  delay(1000);
}

void Alarm() //bz副程式
{


  tone(BuzzerPin, 2000);
  delay(250);
  tone(BuzzerPin, 1000);
  delay(250);
}

void setup(void) {
  Serial.begin(115200);
  pinMode(infrared_VR_Pin, INPUT);
  pinMode(infrared_LED_Pin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  client.setCallback(callback);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  rest.set_id(device_id);
  rest.set_name("esp8266");
  rest.variable("vr",&vr_val);
  server.on("/vr", vrs);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
 

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  //server.handleClient();
  rest.handle(client);
  vrs();
}
