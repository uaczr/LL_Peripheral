#include "Arduino.h"
#include <string.h>

#define USE_OTA
#define USE_UDP
#define USE_MQTT
#define USE_WS2811FX
#define USE_LLSHIELD

uint32_t state = 1;
uint32_t pattern = 1;
uint32_t color = 1;
uint32_t dimm = 127;
uint32_t speed = 1000;
uint32_t device_id = 0;

#ifdef USE_OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const char* ssid = "llbrain1";
const char* password = "lichtundlaessig!";
#endif

#ifdef USE_WS2811FX
#include <WS2812FX.h>
#define LED_PIN   D5
#define LED_COUNT 300
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

#ifdef USE_MQTT
#include <PubSubClient.h>
#include <ESP8266TrueRandom.h>
WiFiClient espClient;
PubSubClient client(espClient);
const char* mqtt_server = "192.168.0.2";

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
}

void reconnect() {
	char device_name[20];
	char temp[10];
#ifdef USE_LLSHIELD
	strcpy(device_name, "LLShield");
	sprintf(temp, "%i", device_id);
	strcat(device_name, temp);
#endif

	if (!client.connected()) {

		if (client.connect(device_name)) {
			Serial.println("Connected MQTT:");
			Serial.printf("Server IP: %s\n", mqtt_server);
			Serial.printf("Device ID: %i\n", device_id);
			client.subscribe("LLShield/+");
		}
	}
}
#endif

void setup() {
	Serial.begin(115200);


#ifdef USE_OTA
	Serial.println("Connecting to WIFI");
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
	Serial.println("Initialising WS2811FX.");
	ws2812fx.init();
	ws2812fx.setBrightness(127);
	ws2812fx.setSegment(0, 0, 50, FX_MODE_BLINK, (const uint32_t[] ) { 0xff0000,
					0x000000, 0x000000 }, 1000, false);
	ws2812fx.setSegment(1, 51, 101, FX_MODE_BLINK, (const uint32_t[] ) {
					0xff0000, 0x000000, 0x000000 }, 1000, false);
	ws2812fx.setSegment(2, 102, 152, FX_MODE_BLINK, (const uint32_t[] ) {
					0xff0000, 0x000000, 0x000000 }, 1000, false);
	ws2812fx.setSegment(3, 153, 203, FX_MODE_BLINK, (const uint32_t[] ) {
					0xff0000, 0x000000, 0x000000 }, 1000, false);
	ws2812fx.setSegment(4, 204, 254, FX_MODE_BLINK, (const uint32_t[] ) {
					0xff0000, 0x000000, 0x000000 }, 1000, false);
	ws2812fx.setSegment(5, 255, 305, FX_MODE_BLINK, (const uint32_t[] ) {
					0xff0000, 0x000000, 0x000000 }, 1000, false);
	ws2812fx.start();
#endif

#ifdef USE_MQTT
	Serial.println("Initialising MQTT");
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
	device_id = ESP8266TrueRandom.random(0, 10000);
#endif
}

void loop() {
#ifdef USE_OTA
	ArduinoOTA.handle();
#endif

#ifdef USE_WS2811FX
	ws2812fx.service();
#endif

#ifdef USE_MQTT
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
#endif
}
