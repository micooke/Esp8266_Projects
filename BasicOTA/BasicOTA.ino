#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "C:\networks.h"

bool connected = false;

WiFiServer TelnetServer(8266);

#define BLINKY

#ifdef BLINKY
uint32_t t0;
uint8_t LED_PIN = 5;
bool LED_STATE = true;
#endif

void setup()
{
#ifdef BLINKY
	t0 = millis();
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LED_STATE);
#endif
	TelnetServer.begin();

	Serial.begin(115200);
	Serial.println("Booting");
	WiFi.mode(WIFI_STA);

	for (uint8_t i = 0; i < NETWORK_NODES; ++i)
	{
		WiFi.begin(ssid[i], password[i]);
		Serial.print(ssid[i]);
		Serial.print(" : ");

		if (WiFi.waitForConnectResult() == WL_CONNECTED)
		{
			Serial.println("Success");
			connected = true;
			break;
		}
		else
		{
			Serial.println("Failed");
		}
	}

	if (connected == false)
	{
		Serial.println("All networks attempted\nRebooting in 5s");
		delay(5000);
		ESP.restart();
	}
	// Port defaults to 8266
	//ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	// ArduinoOTA.setHostname("myesp8266");

	// No authentication by default
	// ArduinoOTA.setPassword((const char *)"123");

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
}

void loop()
{
	ArduinoOTA.handle();
#ifdef BLINKY
	if (millis() - t0 > 1000)
	{
		t0 = millis();
		LED_STATE = !LED_STATE;
		digitalWrite(LED_PIN, LED_STATE);
	}
#endif
}
