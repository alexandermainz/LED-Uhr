# LED-Uhr

PlatformIO project for ESP8266 based LED ring clock.

Shows a simplified clock an a 12 LED circle:
- hour is marked by white LED
- minute is marked by yellow LED
- second is marked by lightblue LED

The minute and second display is set forward if the corresponding 5-minute value is reached.
This means, if the white LED shows 1 o'clock and the yellow LED is on "9 o'clock", the time
is between 1.45 and 1.49.
If the minute "hits" the hour LED, it will turn to orange color.


#### Hardware:

* Wemos D1 Mini or similar with ESP8266
* 12-LED circle with WS2812 LEDs

#### Software:

* PlatformIO IDE with installed ESP8266 support
* Adafruit Neopixel library (https://github.com/adafruit/Adafruit_NeoPixel)
* this repository

### Pin assignment

| LED Pin |	ESP8266 Pin |
| ------- | ----------- |
| VCC |	5V |
| GND |	GND |
| DIN |	D4 |

### Software configuration

* set your WiFi access point SSID and password in your system environment variables `WIFI_SSID` and `WIFI_PASS` or change corresponding entries
in `platformio.ini` lines 24 & 25
* supports ArduinoOTA (over-the-air update) to flash the controller. Set your OTA password in `platformio.ini`
line 26. To remove that support, comment out lines 18-21 in `platformio.ini`
 
### Instructions for use with Arduino IDE

* install "Adafruit Neopixel" via library manager
* rename `main.cpp` to `LED-Uhr.ino`
* add the following lines to `LED-Uhr.ino`:
    * `#define STASSID "your-wifi-ssid"`
    * `#define STAPSK  "your-wifi-password"`
    * `#define OTAPWD  "your-ota-password"` (if you like to use ArduinoOTA)
* remove `main.h` and `platformio.ini`, it's not needed on Arduino


![alt text](https://github.com/alexandermainz/LED-Uhr/blob/master/LED-Uhr-2.jpeg "LED circle clock")
