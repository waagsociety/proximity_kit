The code is written in Arduino IDE platform, for the Adafruit Feather HUZZAH esp8266 board.
To upload the code PK_MUV_main_.ino to your kit, you need the SiLabs CP2104 Driver and the ESP8266 Board Package in Arduino IDE: [tutorial](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide).

The sketch includes the following libraries:
* esp8266 core: https://github.com/esp8266/Arduino
* WiFiManager library: https://github.com/tzapu/WiFiManager
* ArduinoJson library: https://github.com/bblanchon/ArduinoJson
* PubSubClient library: https://github.com/knolleary/pubsubclient
* Adafruit_NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
* MFRC522 library: https://github.com/miguelbalboa/rfid


#### First upload
This is a recommendation, the First Upload is not mandatory but it formats the System files in the esp8266 and reset previously saved WiFi settings.
If you want to execute this step you have to define the variable RESET_SETTINGS  to 1, in the Arduino sketch: <br>
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

#### Uplinks
Depending by the triggered routines, the PK_MUV send three different messages to the MUV server (MQTT protocol). The topic is prox and the payloads, Jason format, are the following:

- Start-up routine:<br> {"message":"startup","id":1756474,"location":"Nieuwmarkt 4, Amsterdam","rssi":-52, "batt":50}<br>
- Tag routine:<br> {"id":1756474,"uid":"04:23:AB:32:EC:4C:81”}<br>
- Check routine:<br> {"message":"check","id":1756474,"location":"Nieuwmarkt 4, Amsterdam","rssi":-59,"batt”:50}<br>

message: this is the routine that has been triggered, start-up or check <br>
id: chipID or PK_MUV ID <br>
uid: Tag UID <br>
location: location of the kit, to be inserted during configuration mode <br>
rssi: rssi <br>
batt: battery level in % when the devices is using the battery or charging the battery. If the kit is powered by the usb cable, the value "usb"<br>. In the start-up message the batt will be always the value in %.

#### Debug
You can use DEBUG variable to print on Serial Monitor debugging messages.

## To Be Fixed/Add
- timestamp
- resetSettings: doesn't delete WiFi credentials.?.
- in start-up mess. => add the usb detection
