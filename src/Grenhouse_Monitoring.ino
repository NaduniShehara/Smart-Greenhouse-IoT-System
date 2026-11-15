// -------- BLYNK SETTINGS (MUST BE AT TOP) --------
#define BLYNK_TEMPLATE_ID "TMPL6m0kJ5Y_F"
#define BLYNK_TEMPLATE_NAME "Greenhouse Monitoring"
#define BLYNK_AUTH_TOKEN "TOdBHzAncJb_TNpJOPphqhBg0mwBuI3R"

// -------- LIBRARIES --------
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

// -------- WIFI --------
char ssid[] = "Dialog 4G 995";
char pass[] = "A104E2f3";

// -------- PIN SETUP --------
#define DHTPIN 5        // D1
#define DHTTYPE DHT11
#define SOIL_PIN A0
#define LDR_PIN 4       // D2

// LEDs
const int blueLED = 14;   // D5
const int whiteLED = 12;  // D6
const int yellowLED = 13; // D7
const int greenLED = 15;  // D8

// -------- OBJECTS --------
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// -------- MAIN FUNCTION --------
void sendSensorData() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT Error!");
    return;
  }

  // Soil averaging
  long sum = 0;
  for(int i=0; i<10; i++) {
    sum += analogRead(SOIL_PIN);
    delay(5);
  }
  int soilAvg = sum / 10;

  int soilPercent = map(soilAvg, 1024, 250, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  int lightStatus = digitalRead(LDR_PIN);

  // -------- LED LOGIC --------
  digitalWrite(blueLED, soilPercent < 30);   // Low Soil
  digitalWrite(whiteLED, h < 40);           // Low Humidity
  digitalWrite(yellowLED, h > 80);          // High Humidity
  digitalWrite(greenLED, lightStatus == HIGH); // Dark

  // -------- BLYNK --------
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, soilPercent);
  Blynk.virtualWrite(V3, lightStatus);

  // Serial monitor
  Serial.print("Temp: "); Serial.print(t);
  Serial.print(" | Hum: "); Serial.print(h);
  Serial.print(" | Soil: "); Serial.print(soilPercent);
  Serial.print("% | Light: "); Serial.println(lightStatus);
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(LDR_PIN, INPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(3000L, sendSensorData);
}

// -------- LOOP --------
void loop() {
  Blynk.run();
  timer.run();
}