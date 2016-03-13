The following setup guide is either directly contributed to, or adapted from : https://github.com/esp8266/Arduino

### Installing with Boards Manager ###

- Install [Arduino 1.6.5r5] (https://www.arduino.cc/en/Main/OldSoftwareReleases#previous) from the [Arduino website](http://www.arduino.cc/en/main/software)
- Start Arduino and open Preferences window
- Enter ```http://arduino.esp8266.com/stable/package_esp8266com_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas
- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation)

The best place to ask questions related to this core is ESP8266 community forum: http://www.esp8266.com/arduino.
If you find this forum or the ESP8266 Boards Manager package useful, please consider supporting www.esp8266.com with a donation. <br />
[![Donate](https://img.shields.io/badge/paypal-donate-yellow.svg)](https://www.paypal.com/webscr?cmd=_s-xclick&hosted_button_id=4M56YCWV6PX66)
</br>

### Hookup ###

*be careful!*
- esp8266 3.3V only! It is not in any way 5V tolerant
- max pin current is 12mA
- 

![esp-12e to FTDI hookup](ESP8266_12E-FTDI_bb.png?raw=true)

### Arduino OTA ###

Three things:
- You may or may not need to open up port 8266 (the default - it can be changed) in your firewall settings
- The port list works fine on the Arduino IDE, but it doesnt clear OTA ports in the visual micro add on for Atmel Studio / Visual Studio
- I run Win 10, and found that i need to start a telnet server for OTA to be visible - shown below

```cpp
//...
WiFiServer TelnetServer(8266); // ADDED

void setup()
{
	TelnetServer.begin(); // ADDED
	//...
}
```
