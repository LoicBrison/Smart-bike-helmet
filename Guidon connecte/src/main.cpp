//ESP32 - Guidon
#include <Arduino.h>
#include <WiFi.h>

//WIFI parameters
const String nom = "ESP32-Guidon";
const char* ssid = "Iphone du bled";
const char* password = "23032000";
WiFiClient master;
IPAddress server(192, 168, 119, 167);
String command;
unsigned long previousRequest = 0;
////

const int buttonPin = 5;
const int joystickButtonPin = 23;
const int joystickPinY = 35;
const int buzzerPin = 32;

const int rightLimit = 3500;
const int leftLimit = 1500;

bool buttonState = false;

int yValue = 0;

void joystickPress();
void joystickRelease();
void buttonPress();
void buttonRelease();
void wifiConfig();
void requestMaster(String command);

void setup() {
  Serial.begin(115200);
  pinMode(joystickButtonPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(joystickButtonPin), joystickPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, FALLING);

  wifiConfig();
}

void loop() {
  yValue = analogRead(joystickPinY);
  if(yValue > rightLimit){
    requestMaster("Right");
    Serial.print("Right");

  }else if(yValue < leftLimit){
    requestMaster("Left");
    Serial.print("Left");
  }
  if(buttonState){
    requestMaster("Stop");
    Serial.print("Stop");
    buttonState = false;
  }
}

void joystickPress(){
  detachInterrupt(digitalPinToInterrupt(joystickButtonPin));
  digitalWrite(buzzerPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(joystickButtonPin), joystickRelease, RISING);
}

void joystickRelease(){
  detachInterrupt(digitalPinToInterrupt(joystickButtonPin));
  digitalWrite(buzzerPin, LOW);
  attachInterrupt(digitalPinToInterrupt(joystickButtonPin), joystickPress, FALLING);
}

void buttonPress(){
  detachInterrupt(digitalPinToInterrupt(buttonPin));
  buttonState = true;
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonRelease, RISING);
}

void buttonRelease(){
  detachInterrupt(digitalPinToInterrupt(buttonPin));
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, FALLING);
}

void wifiConfig(){
 WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.print(nom);
  Serial.print(F(" connected to Wifi! IP address : ")); Serial.println(WiFi.localIP());
}

void requestMaster(String command){
  Serial.println("Request: "+command);
  if(master.connect(server, 80)){
    Serial.println("try send request");
    master.println(nom + " : "+command+" : "+"\r");
    Serial.println("Request sent");
    String answer = master.readStringUntil('\r');
    master.flush();
  }
}