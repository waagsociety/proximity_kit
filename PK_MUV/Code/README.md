The code is written in Arduino IDE platform, for the Adafruit Feather HUZZAH esp8266 board.
To upload the code PK_MUV_main_.ino to your kit, you need the SiLabs CP2104 Driver and the ESP8266 Board Package in Arduino IDE: [tutorial](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide).

The sketch includes the following libraries:
* esp8266 core: https://github.com/esp8266/Arduino
* WiFiManager library: https://github.com/tzapu/WiFiManager
* ArduinoJson library: https://github.com/bblanchon/ArduinoJson
* PubSubClient library: https://github.com/knolleary/pubsubclient
* Adafruit_NeoPixel library:
* SPI library:
* MFRC522 library:


#### First upload
This is a recommendation, the First Upload is not mandatory but it formats the System files in the esp8266 and reset previously saved WiFi settings.
You you want to exectues the mentioned action you have to define the variable RESET_SETTINGS  to 1, in the Arduino sketch: <br>
'#define RESET_SETTINGS 1'.
Upload the code.

#### Second and further uploads
In the Arduino sketch define the variable RESET_SETTINGS 0:<br>
'#define RESET_SETTINGS 0'.

Be sure that the information to access the MUV server are correct:
char mqtt_server[41] = "<my_server>\0";<br>
char mqtt_portStr[21] = "<my_port>\0";<br>
char mqtt_username[21] = "<my_username>\0";<br>
char mqtt_password[21] = "<my_password>\0";<br>
char mqtt_topic[21] = "<my_topic>\0";<br>

and the MUV code is also filled in:
byte key[] = {};    //MUV password

To get the MUV credentials and MUV password, please [contact](https://github.com/emmapa/proximity_kit/tree/master/PK_MUV#Contacts).

#### Debug
You can use DEBUG variable to print on Serial Monitor debugging messages.

## To Be Fixed
- resetSettings: doesn't delete WiFi credentials.?.
