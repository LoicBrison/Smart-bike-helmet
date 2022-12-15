//ESP32 - Casque
#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <string.h>
#include <PubSubClient.h>
#include "BluetoothSerial.h"



BluetoothSerial SerialBT; 

//WI-FI parameters
const String name = "ESP32-Casque";
const char* ssid = "Iphone du bled";
const char* password = "23032000";
WiFiServer server(80);
WiFiClient browser;
WiFiClient espClient;
IPAddress ip(192, 168, 20, 167);
IPAddress gateway(192, 168, 20, 176);
IPAddress subnet(255, 255, 255, 0);
const char* mqtt_server = "192.168.20.162";
PubSubClient psClient(espClient);
/////

//LEDs parameters
//Constants
#define NUM_STRIPS 2
#define NUM_LEDS_PER_STRIP 4
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
#define BRIGHTNESS 10
#define COLOR_ORDER BRG
#define LED_TYPE WS2812B
#define FRAMES_PER_SECOND 60
#define COOLING 55
#define SPARKING 120
const int stripLeftPin  = 13;
const int stripRightPin  = 21;
bool gReverseDirection  = false;
CRGB leds[NUM_STRIPS][NUM_LEDS];
////

const int shockPin = 23;

void shockLoop();
void wifiConfig();
void ledsConfig();
void clientRequest();
void handleRequest(String request);
void ledScenarioRight();
void ledScenarioLeft();
void clearLedStrip(); 
void ledScenarioStop();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(115200);
  pinMode(shockPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  wifiConfig();
  ledsConfig();
  SerialBT.begin("ESP32-Casque");
}

void loop() {
  if (!psClient.connected()) {
    reconnect();
  }
  shockLoop();
  clientRequest();
}

void shockLoop(){
  if(digitalRead(shockPin) == LOW){
    Serial.println("Shock detected");
    psClient.publish("esp32/shock", "SHOCK DETECTED");
    SerialBT.print("SHOCK DETECTED \n");
    ledScenarioStop();
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
  if (request.indexOf("Right") > 0) {
    SerialBT.print("TURN RIGHT \n");
    ledScenarioRight();
    clearLedStrip();
  }
  else if(request.indexOf("Left") > 0) {
    SerialBT.print("TURN LEFT \n");
    ledScenarioLeft();
    clearLedStrip();
  }
  else if(request.indexOf("Stop") > 0) {
    SerialBT.print("STOP \n");
    ledScenarioStop();
    clearLedStrip();
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
  digitalWrite(LED_BUILTIN, HIGH);
  psClient.setServer(mqtt_server, 1883);
  psClient.setCallback(callback);
  psClient.connect("ESP32-Casque");
}

void ledsConfig(){
  FastLED.addLeds<LED_TYPE, stripLeftPin, COLOR_ORDER>(leds[0], 0, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LED_TYPE, stripRightPin, COLOR_ORDER>(leds[1], NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(  BRIGHTNESS );
  clearLedStrip();
}

void ledScenarioRight() { /* function ledScenario */
  ////LEDS Strip scenario
  int currentTime = millis();
  int previousTime = millis();
  while(currentTime - previousTime <= 5000){
    currentTime = millis();
    for (int i = NUM_LEDS_PER_STRIP; i < NUM_LEDS_PER_STRIP*2; i++) {
      leds[1][i].setRGB(255, 0, 255);
      leds[1][i + 3].setRGB(255, 0, 255);
      FastLED.show();
      delay(100);
      leds[1][i] = CRGB::Black;
      FastLED.show();
      delay(100);
    }
  }
}

void ledScenarioLeft() {
  int currentTime = millis();
  int previousTime = millis();
  while(currentTime - previousTime <= 5000){
    currentTime = millis();
    for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      leds[0][i].setRGB(255, 0, 255);
      leds[0][i + 3].setRGB(255, 0, 255);
      FastLED.show();
      delay(100);
      leds[0][i] = CRGB::Black;
      FastLED.show();
      delay(100);
    }
  }
}

void ledScenarioStop(){
  Serial.println("Stop");
  int currentTime = millis();
  int previousTime = millis();
  while(currentTime - previousTime <= 5000){
     currentTime = millis();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[0][i].setRGB(255, 0, 0);;
      leds[1][i].setRGB(255, 0, 0);;
    }
    FastLED.show();
    delay(500);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[0][i] = CRGB::Black;
      leds[1][i] = CRGB::Black;
    }
    FastLED.show();
    delay(500);
  }
}

void clearLedStrip(){
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[0][i] = CRGB::Black;
    leds[1][i] = CRGB::Black;
  }
  FastLED.show();
}

void callback(char* topic, byte* message, unsigned int length){
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
}

void reconnect() {
  while (!psClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (psClient.connect("ESP32-Casque")) {
      Serial.println("connected");
      // Subscribe
      psClient.subscribe("esp32/stock");
    } else {
      Serial.print("failed, rc=");
      Serial.print(psClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}