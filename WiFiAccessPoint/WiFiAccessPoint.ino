/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "c:\networks.h"

bool connected = false;

#define BLINKY

#ifdef BLINKY
uint32_t t0;
uint8_t LED_PIN = 5;
bool LED_STATE = true;
#endif

namespace OTA
{
	WiFiServer TelnetServer(8266);
	void setup()
	{
		// Port defaults to 8266
		//ArduinoOTA.setPort(8266);

		// Hostname defaults to esp8266-[ChipID]
		// ArduinoOTA.setHostname("myesp8266");
		char hostname[17];
		sprintf(hostname, "esp12e_AP-%06x", ESP.getChipId());
		ArduinoOTA.setHostname(hostname);

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
	}

	void start()
	{
		OTA::TelnetServer.begin();
		ArduinoOTA.begin();
	}

	void step()
	{
		ArduinoOTA.handle();
	}
};

namespace esp12e
{
	ESP8266WebServer server(80);

	int8_t num_wifi_networks = 0;
	String _ssid[5];

	void handleRoot()
	{
		esp12e::server.send(200, "text/html", "<h1>You are connected</h1>");
	}

	void setup_AP()
	{
		Serial.println("Configuring access point");

		WiFi.softAP("esp12e_AP");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		esp12e::server.on("/", handleRoot);
	}

	void start_AP()
	{
		esp12e::server.begin();
		Serial.println("HTTP server started");
	}

	void step_AP()
	{
		server.handleClient();
	}
};

void setup()
{
#ifdef BLINKY
	t0 = millis();
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LED_STATE);
#endif
	
	Serial.begin(115200);
	Serial.println("Booting");

	// Start the Access Point
	esp12e::setup_AP();
	esp12e::start_AP();

	// Setup Arduino OTA
	OTA::setup();
	OTA::start();
}

void loop()
{
	OTA::step();

	esp12e::step_AP();

#ifdef BLINKY
	if (millis() - t0 > 1000)
	{
		t0 = millis();
		LED_STATE = !LED_STATE;
		digitalWrite(LED_PIN, LED_STATE);
	}
#endif
}
