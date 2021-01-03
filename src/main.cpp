/*
 * LED-Uhr
 * 
 * Hardware: Wemos D1 Mini, WS2812 12-LED circle
 * Software: https://github.com/adafruit/Adafruit_NeoPixel
 * 
 */

#include <Arduino.h>  // remove this line, if you use the program with Arduino IDE
#include <Adafruit_Neopixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "main.h"

#define LED_COUNT 12
#define LED_PIN D4

const char* ssid = STASSID;
const char* password = STAPSK;

Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t hour = 0;
uint32_t minute = 0;
uint32_t second = 0;
uint32_t updateTime = 0;
uint32_t colorHour = leds.Color(255,255,255);     //white LED for the hour
uint32_t colorMinute = leds.Color(255,255,0);     //yellow LED for the minute
uint32_t colorHourMinute = leds.Color(255,160,0); //orange LED if minute hits hour
uint32_t colorSecond = leds.Color(10,10,15);      //lightblue LED for second

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();

  leds.begin();
  leds.clear();
  leds.show();
  leds.setBrightness(50);

  // connect WiFi
  initWiFi();
  Serial.println("WiFi connected!");
  IPAddress ip = WiFi.localIP();
  char buf[20];
  sprintf(buf, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  Serial.println(buf);
  
  // Init time from NTP-Server
  configTime(1 * 3600, 0, "fritz.box", "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for NTP time sync...");
  delay(1000);
  Serial.print("Time initialized: ");
  time_t now = time(nullptr);
  Serial.print(ctime(&now));

  // Init mDNS & set device name
  if (MDNS.begin("esp-leduhr")) {
    Serial.println("MDNS responder started");
  }

  // Init OTA
  initOTA();
  Serial.println("ArduinoOTA started");
}

void loop() {
  ArduinoOTA.handle();
  MDNS.update();

  if (millis() - updateTime > 1000) {
    leds.clear();
    time_t now = time(nullptr);
    tm *jetzt = localtime(&now);
    second = jetzt->tm_sec / 5;
    minute = jetzt->tm_min / 5;
    hour = jetzt->tm_hour % 12;
    if (hour != minute) {
      leds.setPixelColor(hour, colorHour);
      leds.setPixelColor(minute, colorMinute);
    }
    else
    {
      leds.setPixelColor(minute, colorHourMinute);
    }
    
    if (second != hour && second != minute)
      leds.setPixelColor(second, colorSecond);
    
    leds.show();

    updateTime = millis();
  }
}

void initWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  int tryout = 0;
  while (WiFi.status() != WL_CONNECTED && tryout++ < 3) {
    Serial.print(".");
    Serial.println(WiFi.begin(ssid, password));
    int wait = 0;
    while (WiFi.status() != WL_CONNECTED && wait++ < 12) {
      delay(500);
      Serial.print(".");
    }
    
    delay(5000);
  }
  if (WiFi.status() != WL_CONNECTED)  // reboot if WiFi cannot be connected
    ESP.restart();
    
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());  
  Serial.println(WiFi.localIP());
}

// Init OTA (over-the-air-update)
void initOTA() {
  ArduinoOTA.setHostname("esp-leduhr-OTA");
  ArduinoOTA.setPassword(OTAPWD);
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  ArduinoOTA.begin();
}
