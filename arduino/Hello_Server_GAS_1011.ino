#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <aREST.h>

char* device_id="test1a2b3";
const char* ssid = "IOTAP";
const char* password = "raspberry";

ESP8266WebServer server(80);
// Create aREST instance
WiFiClient espClient;
PubSubClient client(espClient);

// Create aREST instance
aREST rest = aREST(client);

const int led = 2;
int GAS = A0;
int swPin = 5;
int led2 = 4;
String swtt;

void gastt() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  //server.send(200, "text/plain", String("gas:") + analogRead(A0));
  server.send(200, "text/json", String("{\"gas\":") + analogRead(A0) + "}");
}

void updateGas() {
  int vag = analogRead(GAS);
  Serial.print("GAS:");
  Serial.println(vag);
  //delay(2000);
  if (vag > 300)
  {
    digitalWrite(led, LOW);
    //Serial.println("GAS:");
  }
  else
    digitalWrite(led, HIGH);
}

void swPintt() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  //server.send(200, "text/plain", String("sw:") + digitalRead(5));
  server.send(200, "text/json", String("{\"status\":") + digitalRead(5) + "}");
}
void swPin1() {
  boolean val;
  val = digitalRead(swPin);
  Serial.print("status:");

  if (val)
  {
    digitalWrite(4, HIGH);
    Serial.println("開門警報");
    swtt="開門警報";
  }
  else {
    digitalWrite(4, LOW);
    Serial.println("關門");
    swtt="關門";
  }
}



void handleNotFound() {
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(5, INPUT);
  pinMode(4, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  pinMode(GAS, INPUT);
  pinMode(led, OUTPUT);
 

  // Wait for connection
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
  rest.variable("sw",&swtt);
  client.setCallback(callback);
//  if (MDNS.begin("esp8266")) {
//    Serial.println("MDNS responder started");
//} 
  server.on("/ga", gastt);
  server.on("/sw", swPintt);



  

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  //server.onNotFound(handleNotFound);

 server.begin();
 
  Serial.println("HTTP server started");
}

void loop(void) {
  //server.handleClient();
  rest.handle(client);
  //updateGas();
  swPin1();

}
void callback(char* topic, byte* payload, unsigned int length) {

  rest.handle_callback(client, topic, payload, length);

}
