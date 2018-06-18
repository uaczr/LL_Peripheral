#include "Arduino.h"

#define USE_OTA
#define USE_UDP
#define USE_MQTT
#define USE_WS2811FX

#ifdef USE_OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const char* ssid = "llbrian1";
const char* password = "lichtundlaessig!";
#endif

#ifdef USE_WS2811FX
#include <WS2812FX.h>
#define LED_PIN   D5
#define LED_COUNT 300
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

#ifdef USE_MQTT

#endif

uint32_t state = 1;
uint32_t pattern = 1;
uint32_t color = 1;
uint32_t dimm = 127;
uint32_t speed = 1000;


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");


#ifdef USE_OTA
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
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
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif

#ifdef USE_WS2811FX
  ws2812fx.init();
  ws2812fx.setBrightness(127);
  ws2812fx.setSegment(0, 0, 50, 	FX_MODE_BLINK, (const uint32_t[]) {0xff0000, 0x000000, 0x000000}, 1000, false);
  ws2812fx.setSegment(1, 51, 101, 	FX_MODE_BLINK, (const uint32_t[]) {0xff0000, 0x000000, 0x000000}, 1000, false);
  ws2812fx.setSegment(2, 102, 152, 	FX_MODE_BLINK, (const uint32_t[]) {0xff0000, 0x000000, 0x000000}, 1000, false);
  ws2812fx.setSegment(3, 153, 203, 	FX_MODE_BLINK, (const uint32_t[]) {0xff0000, 0x000000, 0x000000}, 1000, false);
  ws2812fx.setSegment(4, 204, 254, 	FX_MODE_BLINK, (const uint32_t[]) {0xff0000, 0x000000, 0x000000}, 1000, false);
  ws2812fx.setSegment(5, 255, 305, 	FX_MODE_BLINK, (const uint32_t[]) {0xff0000, 0x000000, 0x000000}, 1000, false);
  ws2812fx.start();
#endif
}

void loop() {
#ifdef USE_OTA
  ArduinoOTA.handle();
#endif

#ifdef USE_WS2811FX
  ws2812fx.service();
#endif
}
