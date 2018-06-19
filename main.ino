#include "Arduino.h"
#include <string.h>

#define USE_OTA
//#define USE_UDP
#define USE_MQTT
#define USE_WS2811FX
//#define USE_LLShield
//#define USE_LLStrobo
#define USE_LLPuppet


uint32_t state = 1;
uint32_t pattern = 3500;
uint32_t color = 50;
uint32_t dimm = 30000;
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
#include "colors.h"
#include <WS2812FX.h>

#ifdef USE_LLShield
#define LED_PIN   D5
#define LED_COUNT 300
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif
#ifdef USE_LLPuppet
#define LED_PIN   D6
#define LED_COUNT 100
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif
#ifdef USE_LLStrobo
#define LED_PIN   D5
#define LED_PIN2  D4
#define LED_COUNT 300
#define PWMRANGE 255
WS2812FX ws2812fx1 = WS2812FX(84, LED_PIN2, NEO_GRB + NEO_KHZ800);
WS2812FX ws2812fx0 = WS2812FX(80, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

#endif

#ifdef USE_MQTT
#include <PubSubClient.h>
#include <ESP8266TrueRandom.h>
WiFiClient espClient;
PubSubClient client(espClient);
const char* mqtt_server = "192.168.0.2";

void callback(char* topic, byte* payload, unsigned int length) {
#ifdef USE_LLShield
	char name [] = "LLShield";
#endif
#ifdef USE_LLStrobo
	char name [] = "LLStrobo";
#endif
#ifdef USE_LLPuppet
	char name [] = "LLPuppet";
#endif

	if(strstr(topic, name) != NULL){
		if(strstr(topic, "Dimm") != NULL){
			char value[20];
			strncpy(value, (char*)payload, length);
			dimm = atoi(value);
			Serial.printf("Dimm %u %u\n", dimm ,get_color(color));
		}
		if(strstr(topic, "Pattern") != NULL){
			char value[20];
			strncpy(value, (char*)payload, length);
			pattern = atoi(value);
			Serial.printf("Pattern %u %u\n", pattern , (uint32_t)(pattern / 65535.0 * 56));
		}
		if(strstr(topic, "Color") != NULL){
			char value[20];
			strncpy(value, (char*)payload, length);
			color = atoi(value);
			Serial.printf("Color %u 0x%X\n", color ,get_color(color));
		}
		if(strstr(topic, "Speed") != NULL){
			char value[20];
			strncpy(value, (char*)payload, length);
			speed = atoi(value);
			Serial.printf("Speed %u %u\n", speed ,get_color(color));
		}

		uint32_t mode = (uint32_t)((float)pattern / 65535.0 * (float)(MODE_COUNT -1));
#ifdef USE_LLStrobo
		ws2812fx0.setSpeed(speed);
		ws2812fx0.setBrightness((uint32_t)(dimm / 65535.0 * 255.0));
		ws2812fx0.setColor(get_color(color));
		ws2812fx0.setMode(mode);
		ws2812fx1.setSpeed(speed);
		ws2812fx1.setBrightness((uint32_t)(dimm / 65535.0 * 255.0));
		ws2812fx1.setColor(get_color(color));
		ws2812fx1.setMode(mode);
#endif
#ifdef USE_LLPuppet
		ws2812fx.setSpeed(speed);
		ws2812fx.setBrightness((uint32_t)(dimm / 65535.0 * 255.0));
		ws2812fx.setColor(get_color(color));
		ws2812fx.setMode(mode);
#endif
#ifdef USE_LLShield
		ws2812fx.setBrightness(dimm / 65535.0 * 255);
		ws2812fx.setSegment(0, 0, 50, mode, get_color(color), speed, false);
		ws2812fx.setSegment(1, 51, 101, mode, get_color(color), speed, false);
		ws2812fx.setSegment(2, 102, 152, mode, get_color(color), speed, false);
		ws2812fx.setSegment(3, 153, 203, mode, get_color(color), speed, false);
		ws2812fx.setSegment(4, 204, 254, mode, get_color(color), speed, false);
		ws2812fx.setSegment(5, 255, 305, mode, get_color(color), speed, false);
#endif
	}
}

void reconnect() {
	char device_name[20];
	char temp[10];
#ifdef USE_LLShield
	strcpy(device_name, "LLShield");
#endif
#ifdef USE_LLStrobo
	strcpy(device_name, "LLStrobo");
#endif
#ifdef USE_LLPuppet
	strcpy(device_name, "LLPuppet");
#endif
	sprintf(temp, "%i", device_id);
	strcat(device_name, temp);


	if (!client.connected()) {

		if (client.connect(device_name)) {
			Serial.println("Connected MQTT:");
			Serial.printf("Server IP: %s\n", mqtt_server);
			Serial.printf("Device ID: %i\n", device_id);
#ifdef USE_LLStrobo
			client.subscribe("LLStrobo/+");
#endif
#ifdef USE_LLShield
			client.subscribe("LLShield/+");
#endif
#ifdef USE_LLPuppet
			client.subscribe("LLPuppet/+");
#endif
		}
	}
}
#endif

#ifdef USE_UDP
#include "Protocol.h"
#define MAX_PACKET_SIZE 32
IPAddress remoteIP;
unsigned int remotePort = 1103;
char recvBuffer[MAX_PACKET_SIZE];
WiFiUDP Udp;
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

#ifdef USE_LLShield
	ArduinoOTA.setHostname("LLShield");
#endif
#ifdef USE_LLStrobo
	ArduinoOTA.setHostname("LLStrobo");
#endif

#ifdef USE_LLPuppet
	ArduinoOTA.setHostname("LLPuppet");
#endif

	ArduinoOTA.begin();
	Serial.println("Ready");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
#endif

#ifdef USE_WS2811FX
	Serial.println("Initialising WS2811FX.");


#ifdef USE_LLShield
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
#ifdef USE_LLStrobo
	ws2812fx1.init();
	ws2812fx1.setSpeed(speed);
	ws2812fx1.setBrightness((uint32_t)(dimm / 65535.0 * 255.0));
	ws2812fx1.setColor(get_color(color));
	ws2812fx1.setMode((uint32_t)(pattern / 65535.0 * (float)(MODE_COUNT -1)));
	ws2812fx1.start();

	ws2812fx0.init();
	ws2812fx0.setSpeed(speed);
	ws2812fx0.setBrightness((uint32_t)(dimm / 65535.0 * 255.0));
	ws2812fx0.setColor(get_color(color));
	ws2812fx0.setMode((uint32_t)(pattern / 65535.0 * (float)(MODE_COUNT -1)));
	ws2812fx0.start();

#endif
#ifdef USE_LLPuppet
	ws2812fx.init();
	ws2812fx.setSpeed(speed);
	ws2812fx.setBrightness((uint32_t)(dimm / 65535.0 * 255.0));
	ws2812fx.setColor(get_color(color));
	ws2812fx.setMode((uint32_t)(pattern / 65535.0 * (float)(MODE_COUNT -1)));
	ws2812fx.start();

#endif
#endif

#ifdef USE_MQTT
	Serial.println("Initialising MQTT");
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
	device_id = ESP8266TrueRandom.random(0, 10000);
#endif
#ifdef USE_UDP
	Udp.begin(remotePort);

		//Receivebuffer
		for (int i = 0; i < MAX_PACKET_SIZE; i++) {
			recvBuffer[i] = '0';
	}
#endif

#ifdef USE_LLStrobo
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D7, OUTPUT);
	pinMode(D6, OUTPUT);
	digitalWrite(D1, HIGH);
	digitalWrite(D2, HIGH);
	digitalWrite(D3, HIGH);
	digitalWrite(D7, HIGH);
	digitalWrite(D6, HIGH);



#endif
}

uint32_t mode_old = 0;
void loop() {
#ifdef USE_OTA
	ArduinoOTA.handle();
#endif

#ifdef USE_WS2811FX

	uint32_t mode = (uint32_t)((float)pattern / 65535.0 * (float)(MODE_COUNT -1));

#ifdef USE_LLShield
	ws2812fx.service();
#endif


#ifdef USE_LLStrobo




	ws2812fx0.service();
	ws2812fx1.service() ;




	//uint8_t red = round(1 - (get_color(color) & 0xFF) / 255.0);
	//uint8_t green = round(1 - (get_color(color) >> 8 & 0xFF) / 255.0);
	//uint8_t blue = round(1 - (get_color(color) >>16 & 0xFF) / 255.0);

	digitalWrite(D1, LOW);
	digitalWrite(D2, LOW);
	digitalWrite(D3, LOW);

#endif
#ifdef USE_LLPuppet
	ws2812fx.service();
#endif
#endif

#ifdef USE_MQTT
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
#endif

#ifdef USE_UDP
	uint32_t packetSize = Udp.parsePacket();
	synchronisingMessage syncMesg;
	remoteIP = Udp.remoteIP();

	if (packetSize > 0) {
	if (packetSize <= MAX_PACKET_SIZE) {

		Udp.read(recvBuffer, packetSize);
		//Serial.print(recvBuffer);
		//Checke ob Synchronisierungspacket
		if (synchronising(recvBuffer, packetSize)) {
			syncMesg.create(recvBuffer, packetSize);
			if (syncMesg.direction == '0') {
				//Serial.println("BEAT");
				ws2812fx.trigger();

			}
		}
	}
	}
#endif



}
