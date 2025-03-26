#include <WiFi.h>
#include <MQTT.h>
WiFiClient clientwifi;
MQTTClient clientmqtt;
const char clientID[] = "POPO123";
const char ssid[] = "wadudu";
const char pass[] = "bgstt123";
const char* brokerserver =  "prickleshift857.cloud.shiftr.io";
const char* userbrokermqtt = "prickleshift857";
const char* passbrokermqtt = "kelasiotcoba12";

String topic1 = "LED/#";
String topic_masuk_baru;
String Data_payload_baru;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int lebar = 128;
const int tinggi = 64;
const int reset = 4; 
Adafruit_SSD1306 oledz(lebar,tinggi,&Wire, reset);

#include <ESP32Servo.h>
int servopins1 = 33, servopins2 = 5;  
Servo servo1ku;
Servo servo2ku;
int led[] = {12,14,27};
int ledyellow = 25, ledblue = 13;
unsigned long wktuskrg = 0;
unsigned long wktuskrg2 = 0;

#define poten1 32
#define poten2 35
int potenread1, potenread2;
int old_pot1 = 0, old_pot2 = 0;

#include <NewPing.h>
#define TRIGGER_PIN 26
#define ECHO_PIN 26
NewPing sonar(TRIGGER_PIN, ECHO_PIN, 400);
int jarak, old_jarak = 0;

void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 3; i++){
    pinMode(led[i], OUTPUT);
  }
  pinMode(ledblue, OUTPUT);
  pinMode(ledyellow, OUTPUT);
  servo1ku.attach(servopins1); 
  servo2ku.attach(servopins2);
  oledz.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oledz.clearDisplay();
  rgbled(0,0,0);

  WiFi.begin(ssid, pass);
  clientmqtt.begin(brokerserver, clientwifi);
  clientmqtt.onMessage(pesansubscribemasuk);
  reconnect();
}

void loop() {
  clientmqtt.loop();
  if(!clientmqtt.connected()){
    reconnect();
  }
  pesanclient_doit();
  publish_poten();
  publish_ultras();
}

void reconnect(){
  rgbled(0,1,0);
  while (WiFi.status() != WL_CONNECTED){
    lcdkecil(1,1,"WifiSTAT = ---");
    lcdkecil(1,10,"MQTTSTAT = ---");
    oledz.clearDisplay();
    delay(500);
    rgbled(1,1,1);
  }
  rgbled(1,0,0);
  lcdkecil(1,2,"WifiConnected");
  oledz.clearDisplay();
  clientmqtt.setWill("kelasiot/POPO123/Status", "offline", true, 1);
  while(!clientmqtt.connect(clientID, userbrokermqtt, passbrokermqtt)){
    lcdkecil(1,1,"WifiSTAT = Connected");
    lcdkecil(1,10,"MQTTSTAT = ---");
    oledz.clearDisplay();
    delay(500);
    rgbled(1,1,1);
  }
  rgbled(0,0,1);
  lcdkecil(1,1,"WifiSTAT = Connected");
  lcdkecil(1,10,"MQTTSTAT = Connected");
  clientmqtt.publish("kelasiot/POPO123/Status", "online", true, 1);
  clientmqtt.subscribe(topic1, 1);
}

void pesansubscribemasuk(String &topicmasuk, String &payloadmasuk){
  topic_masuk_baru = topicmasuk;
  Data_payload_baru = payloadmasuk;
}

void pesanclient_doit(){
  if(topic_masuk_baru == "LED/led1"){
    if(Data_payload_baru == "on"){
      digitalWrite(ledblue, HIGH);
    }
    else if(Data_payload_baru == "off"){
      digitalWrite(ledblue, LOW);
    }
  }
  if(topic_masuk_baru == "LED/led2"){
    if(Data_payload_baru == "on"){
      digitalWrite(ledyellow, HIGH);
    }
    else if(Data_payload_baru == "off"){
      digitalWrite(ledyellow, LOW);
    }
  } 
  if(topic_masuk_baru == "LED/servoxxx"){
    int servoposition =  Data_payload_baru.toInt();
    servo1ku.write(servoposition);
    Serial.println("Servo 1 Position: " + String(servoposition));
  }
  if(topic_masuk_baru == "LED/srvvzz"){
    int servoposition2 =  Data_payload_baru.toInt();
    servo2ku.attach(servopins2); 
    servo2ku.write(servoposition2);
    Serial.println("Servo 2 Position: " + String(servoposition2));
  }
  lcdkecil(1, 20, "Topic: " + topic_masuk_baru);
  lcdkecil(1, 30, "Data: " + String(Data_payload_baru));
}

void lcdkecil(int koor1, int koor2, String katakata){
  oledz.fillRect(koor1, koor2, 128, 10, BLACK);
  oledz.setTextSize(1); 
  oledz.setTextColor(WHITE);
  oledz.setCursor(koor1, koor2);
  oledz.println(katakata);
  oledz.display();
}

void rgbled(int pin1, int pin2, int pin3){
    digitalWrite(led[0], pin1);
    digitalWrite(led[1], pin2);
    digitalWrite(led[2], pin3);
}

void publish_poten(){
  potenread1 = analogRead(poten1);
  potenread2 = analogRead(poten2);
  if(millis()-wktuskrg > 100){
    wktuskrg = millis();
    if(potenread1 != old_pot1){
      clientmqtt.publish("kelasiot/pot1", String(potenread1).c_str(), true, 1);
      old_pot1 = potenread1;
    }
    if(potenread2 != old_pot2){
      clientmqtt.publish("kelasiot/pot2", String(potenread2).c_str(), true, 1);
      old_pot2 = potenread2;
    }
    lcdkecil(1, 40, "POT1: " + String(potenread1));
    lcdkecil(1, 50, "POT2: " + String(potenread2));
  }
}

void publish_ultras(){
  jarak = sonar.ping_cm();
  if(millis() - wktuskrg2 > 500){
    wktuskrg2 = millis();
    if(jarak != old_jarak){
      clientmqtt.publish("kelasiot/ultras",  String(jarak).c_str(), true, 1);
      old_jarak = jarak;
    }
  }
}
