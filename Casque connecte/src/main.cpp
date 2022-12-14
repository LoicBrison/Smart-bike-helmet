//ESP32 - Casque
#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>

//WI-FI parameters
const String name = "ESP32-Casque";
const char* ssid = "Iphone du bled";
const char* password = "23032000";
WiFiServer server(80);
WiFiClient browser;
IPAddress ip(192, 168, 119, 167);
IPAddress gateway(192, 168, 119, 147);
IPAddress subnet(255, 255, 255, 0);
/////

const int shockPin = 23;

void shockLoop();
void wifiConfig();
void clientRequest();
void handleRequest(String request);

void setup() {
  Serial.begin(115200);
  pinMode(shockPin, INPUT);

  wifiConfig();
}

void loop() {
  // shockLoop();
  clientRequest();
}

void shockLoop(){
  if(digitalRead(shockPin) == HIGH){
    Serial.println("Shock detected");
  }
  else{
    Serial.println("No shock");
  }
}

void clientRequest() {
  WiFiClient client = server.available();
  client.setTimeout(50);

  if(client){
    if(client.connected()){
      String request = client.readStringUntil('\r');
      if(request.indexOf("ESP32-Guidon") == 0){
        handleRequest(request);
        client.stop();   
      }
    }
  }
}

void handleRequest(String request) {
  if (request.indexOf("lightOn") > 0)  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void wifiConfig(){
//WIFI
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  server.begin();
  Serial.print(name);
  Serial.print(F(" connected to Wifi! IP address : http://")); Serial.println(WiFi.localIP());
}