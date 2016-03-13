#define BLINKY
#define NEO_PIXEL

#ifdef NEO_PIXEL
#include <Adafruit_NeoPixel.h>

#define PIN            4
#define NUMPIXELS      50
#define BG 1

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define delayval 50
uint32_t tNext;
uint8_t R, G, B;
uint8_t idx;
bool chaseUp;

void chase1(const uint8_t R, const uint8_t G, const uint8_t B, uint8_t &i, uint32_t &tNext, const bool chaseUp = true, const bool clear = true);
void chase3(const uint8_t R, const uint8_t G, const uint8_t B, uint8_t &i, uint32_t &tNext, const bool chaseUp = true, const bool clear = true);

int color;
int center = 0;
int step = -1;
int maxSteps = 16;
float fadeRate = 0.6;
int diff;

//background color
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;

void ripple();
#endif

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "esp12e_AP";
const char* password = "";

WiFiServer TelnetServer(8266);

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

	WiFi.begin(ssid, password);
	while (WiFi.waitForConnectResult() != WL_CONNECTED)
	{
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}

	ArduinoOTA.setPort(8266); // Default
	ArduinoOTA.setHostname("esp8266_NeoPixel");

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

#ifdef NEO_PIXEL
	R = 0; // G
	G = 133; // R
	B = 133; // B
	idx = 0;

	chaseUp = true;
	pixels.setBrightness(25);
	pixels.begin(); // This initializes the NeoPixel library
#endif
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

#ifdef NEO_PIXEL
	ripple();

	/*
	chase1(R, G, B, idx, tNext, chaseUp, true);
	if ((idx == 0) || (idx == NUMPIXELS - 1))
	{
	chaseUp = !(chaseUp);
	pixels.clear();
	}
	
	chase3(R, G, B, idx, tNext, chaseUp, true);
	if ((idx == 1) || (idx == NUMPIXELS - 2))
	{
		chaseUp = !(chaseUp);
		pixels.clear();
	}
	*/
#endif
}
#ifdef NEO_PIXEL
void chase1(const uint8_t R, const uint8_t G, const uint8_t B, uint8_t &i, uint32_t &tNext, const bool chaseUp, const bool clear)
{
	if (millis() > tNext)
	{
		if (clear)
		{
			pixels.setPixelColor(i, pixels.Color(0, 0, 0));
		}
		tNext = millis() + delayval;
		if (chaseUp)
		{
			if (++i == NUMPIXELS)
			{
				i = 0;
			}
		}
		else
		{
			i = (i == 0) ? NUMPIXELS - 1 : i - 1;
		}
		pixels.setPixelColor(i, pixels.Color(R, G, B));
		pixels.show();
	}
}
void chase3(const uint8_t R, const uint8_t G, const uint8_t B, uint8_t &i, uint32_t &tNext, const bool chaseUp, const bool clear)
{
	if (millis() > tNext)
	{
		if (clear)
		{
			pixels.setPixelColor(i - 1, pixels.Color(0, 0, 0));
			pixels.setPixelColor(i, pixels.Color(0, 0, 0));
			pixels.setPixelColor(i + 1, pixels.Color(0, 0, 0));
		}
		tNext = millis() + delayval;
		if (chaseUp)
		{
			if (++i == NUMPIXELS - 1)
			{
				i = 1;
			}
		}
		else
		{
			i = (i == 1) ? NUMPIXELS - 2 : i - 1;
		}
		pixels.setPixelColor(i - 1, pixels.Color(R, G, B));
		pixels.setPixelColor(i, pixels.Color(R, G, B));
		pixels.setPixelColor(i + 1, pixels.Color(R, G, B));
		pixels.show();
	}
}
void ripple()
{
	if (BG)
	{
		if (currentBg == nextBg)
		{
			nextBg = random(256);
		}
		else if (nextBg > currentBg)
		{
			currentBg++;
		}
		else
		{
			currentBg--;
		}
		for (uint16_t l = 0; l < NUMPIXELS; l++)
		{
			pixels.setPixelColor(l, Wheel(currentBg, 0.1));
		}
	}
	else
	{
		for (uint16_t l = 0; l < NUMPIXELS; l++)
		{
			pixels.setPixelColor(l, 0, 0, 0);
		}
	}

	if (step == -1)
	{
		center = random(NUMPIXELS);
		color = random(256);
		step = 0;
	}

	if (step == 0)
	{
		pixels.setPixelColor(center, Wheel(color, 1));
		step++;
	}
	else
	{
		if (step < maxSteps)
		{
			pixels.setPixelColor(wrap(center + step), Wheel(color, pow(fadeRate, step)));
			pixels.setPixelColor(wrap(center - step), Wheel(color, pow(fadeRate, step)));
			if (step > 3)
			{
				pixels.setPixelColor(wrap(center + step - 3), Wheel(color, pow(fadeRate, step - 2)));
				pixels.setPixelColor(wrap(center - step + 3), Wheel(color, pow(fadeRate, step - 2)));
			}
			step++;
		}
		else
		{
			step = -1;
		}
	}

	pixels.show();
	delay(500);
}

int wrap(int step)
{
	if (step < 0)
	{
		return NUMPIXELS + step;
	}
	if (step > NUMPIXELS - 1)
	{
		return step - NUMPIXELS;
	}
	return step;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, float opacity)
{
	if (WheelPos < 85)
	{
		return pixels.Color((WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity, 0);
	}
	else if (WheelPos < 170)
	{
		WheelPos -= 85;
		return pixels.Color((255 - WheelPos * 3) * opacity, 0, (WheelPos * 3) * opacity);
	}
	else
	{
		WheelPos -= 170;
		return pixels.Color(0, (WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity);
	}
}
#endif