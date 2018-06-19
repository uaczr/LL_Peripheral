#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-06-19 20:56:37

#include "Arduino.h"
#include "Arduino.h"
#include <string.h>
#define USE_OTA
#define USE_MQTT
#define USE_WS2811FX
#define USE_LLPuppet
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "colors.h"
#include <WS2812FX.h>
#define LED_PIN   D5
#define LED_COUNT 300
#define LED_PIN   D6
#define LED_COUNT 100
#define LED_PIN   D5
#define LED_PIN2  D4
#define LED_COUNT 300
#define PWMRANGE 255
#include <PubSubClient.h>
#include <ESP8266TrueRandom.h>
#include "Protocol.h"
void callback(char* topic, byte* payload, unsigned int length) ;
void reconnect() ;
void setup() ;
void loop() ;


#include "main.ino"

#endif
