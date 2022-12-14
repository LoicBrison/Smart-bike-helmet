//ESP32 - Guidon
#include <Arduino.h>
#include <WiFi.h>

//WIFI parameters
const String name = "ESP32-Guidon";
const char* ssid = "Iphone du bled";
const char* password = "23032000";
WiFiClient master;
IPAddress server(192, 168, 119, 167);
String command;
unsigned long previousRequest = 0;
////

const int buttonPin = 15;
const int joystickButtonPin = 23;
const int joystickPinY = 35;
const int buzzerPin = 32;

const int rightLimit = 3500;
const int leftLimit = 1500;

int yValue = 0;

void joystickPress();
void joystickRelease();
void buttonPress();
void buttonRelease();
void wifiConfig();
void requestMaster();

void setup() {
  Serial.begin(115200);
  pinMode(joystickButtonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(joystickButtonPin), joystickPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, FALLING);

  wifiConfig();
}

void loop() {
  yValue = analogRead(joystickPinY);
  if(yValue > rightLimit){
    Serial.println("Right");
  }else if(yValue < leftLimit){
    Serial.println("Left");
  }
  requestMaster();  
}

void joystickPress(){
  detachInterrupt(digitalPinToInterrupt(joystickButtonPin));
  Serial.println("Button Pressed");
  digitalWrite(buzzerPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(joystickButtonPin), joystickRelease, RISING);
}

void joystickRelease(){
  detachInterrupt(digitalPinToInterrupt(joystickButtonPin));
  Serial.println("Button Released");
  digitalWrite(buzzerPin, LOW);
  attachInterrupt(digitalPinToInterrupt(joystickButtonPin), joystickPress, FALLING);
}

void buttonPress(){
  detachInterrupt(digitalPinToInterrupt(buttonPin));
  Serial.println("FREEEEEEIN");
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonRelease, RISING);
}

void buttonRelease(){
  detachInterrupt(digitalPinToInterrupt(buttonPin));
  Serial.println("FREIN plus");
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, FALLING);
}

void wifiConfig(){
 WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.print(name);
  Serial.print(F(" connected to Wifi! IP address : ")); Serial.println(WiFi.localIP());
}

void requestMaster(){
  if(millis() - previousRequest > 1000){
    previousRequest = millis();
    if(master.connect(server, 80)){
      master.println(name + ": lightOn"+"\r");
      String answer = master.readStringUntil('\r');
      master.flush();
    }
  }
}