/* 
 * Code for Adafruit Feather HUZZAH esp8266
 * WiFi module + NFC reader
 * Waag, MUV project
 * author: Emma Pareschi
 * version pk_muv_00 -  6 Dec 2018
 * 
 * MUV server credential are missing
 * MUV key for the MUV NFC tag is missing. 
 * 
 * LICENSE
 * Copyright (c) 2018 Emma Pareschi

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

 * 
 * 
 */
 
#include <FS.h>                   // Make sure ESP library 2.1.0 or higher is installed
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient


// RESET_SETTINGS
// To reset the WiFi settings and format the System file define it as 1
// If you do so the esp8622 will not be able to properly function.
// To have the PK_MUV working, you need to define RESET_SETTING to 0 and upload the code again.
#define RESET_SETTINGS 0

// DEBUG message
#define DEBUG 0

// MUV MQTT server and passwords
char mqtt_server[41] = "<my_server>\0";
char mqtt_portStr[21] = "<my_port>\0";
char mqtt_username[21] = "<my_username>\0";
char mqtt_password[21] = "<my_password>\0";
char mqtt_topic[21] = "<my_topic>\0";
char address[41] = "insert address\0";

byte key[] = {};    //MUV password

String location = "";

int mqtt_port = atoi(mqtt_portStr);

#define BUTTON_PIN 16              // D0, button to enter wifi manager, configuration button
int buttonState = 1;         // current state of the button
int lastButtonState = 1;     // previous state of the button

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

String readStr;
long chipid;
bool shouldSaveConfig = false;    //flag for saving data

// for Neopixel
#include <Adafruit_NeoPixel.h>

#define PINPIXELS            0 //Neopixel pin
#define NUMPIXELS      16 //number of neopixels

#define LedConfig 2
#define LedBatt 1
#define LedTag 0

byte brightness = 50;

byte j =0;            //index to print zzzzzz
byte index_light =0;  // index to blink the battery Led

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PINPIXELS, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.Color(255, 0, 0);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t orange = pixels.Color(255,100,0);
uint32_t off = pixels.Color(0,0,0);
uint32_t aqua = pixels.Color(0,255,255);
uint32_t blue = pixels.Color(0,0,255);
uint32_t magenta = pixels.Color(255, 0, 255);
  
//for NFC
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN        4         // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
byte buffer[18];
byte key_tag[sizeof(key)];

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

boolean tag_true = 0;
boolean checkin_true = 0;
boolean sleep_true = 1;
byte batt_threshold_high = 96;
byte batt_threshold_low = 20;
byte batt_threshold_low_low = 10;
boolean reconnect_true = 0;

//PIEZO
#define SPEAK_PIN 15

//BATTERY
#define BATTERY_PIN A0

byte aux_batt = 0;          // batt level
int diff_batt = 0;          // batttery levels differences between two cycles
boolean usb_true = 0; 
int aux_batt_previous = 0;  // previous battery level
byte i_batt =0;             // index for the usb check loop
byte batt_cycles = 20;      // amount of cycles in the check loop


//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void saveConfigJson() {
  //save the custom parameters to FS
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_portStr;
  json["mqtt_username"] = mqtt_username;
  json["mqtt_password"] = mqtt_password;
  json["mqtt_topic"] = mqtt_topic;
  json["address"] = address;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  json.printTo(Serial);
  Serial.println("\n");
  json.printTo(configFile);
  configFile.close();
  //end save
}


void setup() {
 
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT);

  pixels.begin();
  pixels.setBrightness(brightness);
  delay(10);
  for (int i = 1; i<NUMPIXELS; i++){
    pixels.setPixelColor(i,off);
  }
  pixels.setPixelColor(LedConfig, off);
  pixels.show();

 if(RESET_SETTINGS){
  //Format FS, reset Wifi settings, for testing  
  Serial.print("Reset WiFi settings...");
    wifiManager.resetSettings();  //****
    delay(100);

  Serial.print("Formatting FS...");
    SPIFFS.format();
    delay(100);
    Serial.println("Done.");
  
    pixels.setPixelColor(LedConfig, magenta);
    pixels.show();
    Serial.println("Done.");
  
   while(1) {
      delay(1000);
      Serial.println("loop..."); //i block the code
  }
 } else {}

  pinMode(SPEAK_PIN, OUTPUT);
  noTone(SPEAK_PIN);

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);  //print the content of json object on serial.
        if (json.success()) {
          Serial.println("\nparsed json, from setup");

          strcpy(mqtt_server, json["mqtt_server"]); //save the block "mqtt_server" of json object in mqtt_server variable
          strcpy(mqtt_portStr, json["mqtt_port"]);
          mqtt_port = atoi(mqtt_portStr);
          strcpy(mqtt_username, json["mqtt_username"]);
          strcpy(mqtt_password, json["mqtt_password"]);
          strcpy(mqtt_topic, json["mqtt_topic"]);
          strcpy(address, json["address"]);
          

        } else {
          Serial.println("failed to load json config");
        }
      }
    } else {
      Serial.println("/config.json does not exist, creating");
      saveConfigJson(); // saving the hardcoded default values
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read


  wifiManager.setSaveConfigCallback(saveConfigCallback);
    
  boolean startConfigPortal = false;
  
  if ( digitalRead(BUTTON_PIN) == LOW ) {
    startConfigPortal = true;
  }

  WiFi.mode(WIFI_STA);
  if (WiFi.SSID()) {
    Serial.println("Using saved credentials");
    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    WiFi.begin();
  } else {
    Serial.println("No saved credentials");
    startConfigPortal = true;
  }

  WiFi.waitForConnectResult();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Failed to connect Wifi");
    pixels.setPixelColor(LedConfig, blue);
    pixels.show();
    startConfigPortal = true;
  } else { 
  }

  if (startConfigPortal) {

    pixels.setPixelColor(LedConfig, blue);
    pixels.show();

    WiFiManagerParameter custom_address("address", "address", "insert address", 41);
    wifiManager.addParameter(&custom_address);

    wifiManager.startConfigPortal("MUV");
    
    if (shouldSaveConfig) {

      mqtt_port = atoi(mqtt_portStr);
      strcpy(address, custom_address.getValue());
      location = custom_address.getValue();

      saveConfigJson();
      shouldSaveConfig = false;
    }
  }
  
  WiFi.waitForConnectResult();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Failed to connect Wifi");
    pixels.setPixelColor(LedConfig, blue);
    pixels.show();
    while(1){
    Serial.println("Reset the device.");
    delay(500);
    }
  } else { 
    Serial.println("Wifi connected...");
    pixels.setPixelColor(LedConfig, green);
    pixels.show();
  }

  mqttClient.setServer(mqtt_server, mqtt_port);
  chipid = ESP.getChipId();

  startup_message();

  pixels.setPixelColor(LedConfig, off);
  pixels.show();
  
  //for NFC reader
  SPI.begin();        // Init SPI bus

  
  mfrc522.PCD_Init(SS_PIN, RST_PIN); // Init each MFRC522 card
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("NFC ready. Scan PICC to see UID, SAK, type, and data blocks..."));

  aux_batt_previous = battery_level();
  
}


//function that send the startup message
void startup_message() {
  char mqtt_clientid[15];
  //snprintf (mqtt_clientid, 14, "ESP%u", chipid);

  if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected.");
      
      //blink led config three times green and stays green
      blinks(3, LedConfig, green, off);
      //--end
      
      long rssi = WiFi.RSSI();

      // send proper JSON startup message
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["message"] = "startup";
      json["id"] = chipid;
      json["location"] = address;
      json["rssi"] = rssi;
      json["batt"] = battery_level();
      char buf[110];
      json.printTo(buf, sizeof(buf));

      Serial.print("Publish message: ");
      Serial.println(buf);

      char topic_buf[50];
      sprintf(topic_buf, mqtt_topic, chipid);
      mqttClient.publish(topic_buf, buf);
      reconnect_true = 0;
  
  } else {   
    pixels.setPixelColor(LedConfig, aqua);
    pixels.show();
    reconnect_true = 1;
  }
}


//**********************LOOP
void loop() {

//BATTERY routine
   i_batt++;
   
   delay(1);
          
    #if DEBUG
      Serial.println("___________");
      Serial.print("indice per controllo batteria"); Serial.println(i_batt);
      Serial.println("___________"); 
    #endif
    
    aux_batt = battery_level();
    diff_batt += abs(aux_batt_previous - aux_batt);
    aux_batt_previous = aux_batt;

    if (i_batt >= batt_cycles) {
      i_batt = 0;

      if(diff_batt > batt_cycles -1){
        usb_true = 1;
      } else {
        usb_true = 0;
      }

    #if DEBUG
      Serial.println("___________");
      Serial.print("differenze batteria"); Serial.println(diff_batt);
      Serial.println("___________"); 
    #endif
      
      diff_batt = 0;
    }

    
   #if DEBUG
    Serial.println("___________");
    Serial.print("USB FLAG"); Serial.println(usb_true);
    Serial.println("___________"); 
   #endif

    if (usb_true == 1){

    pixels.setPixelColor(LedBatt, off);
    pixels.show();
   
   } else {

    if (aux_batt >= batt_threshold_high){
      pixels.setPixelColor(LedBatt, green);
      pixels.show();
    }

   if(aux_batt < batt_threshold_high && aux_batt > batt_threshold_low){
      pixels.setPixelColor(LedBatt, off);
      pixels.show();
   }

   if(aux_batt <= batt_threshold_low && aux_batt > batt_threshold_low_low){
        index_light++;  
          if(index_light<10) {
            battery_light();
          } else {
            pixels.setPixelColor(LedBatt, off);
            pixels.show();
    } 

   if (index_light > 200) index_light = 0; 
     }

     if (aux_batt <= batt_threshold_low_low){
      pixels.setPixelColor(LedBatt, red);
      pixels.show();
  }
   }
//end battery routine

//CHECK ROUTINE
  //if the config sw is press => send check message
  byte send_check = 0;
  buttonState = digitalRead(BUTTON_PIN);
  //compare the buttonState to its previous state
  if (buttonState != lastButtonState && buttonState == HIGH) {    
      // if the state has changed
      send_check = 1;
      // Delay a little bit to avoid bouncing
      delay(50);
  }

  lastButtonState = buttonState;

  if (send_check) check();  //if send_check is true => send the check message
//end CHECK
  
//SLEEP Routine
  if (sleep_true) {
    pixels.setPixelColor(LedTag, off);
    pixels.show();
    j++;
    Serial.print(".");
    if (j==10) {
      j= 0;
      Serial.println(".zzzzz");
   }}

  #if DEBUG
    Serial.print("checkin_true : "); Serial.println(checkin_true);  
  #endif

//SLEEP
  if(sleep_true){
    WiFi.forceSleepBegin(); //using mode wifi off => the current is 20mA (w/o leds)
    delay(1);
  }
   
//NFC Reader part
  //Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    pixels.setPixelColor(LedTag, off);
    pixels.show();
    return;
  } else {
    blinks(0, LedTag,off,green);
    pixels.show();
    Serial.println("");
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String payload = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        payload +=
        (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(mfrc522.uid.uidByte[i], HEX) + 
      (i != mfrc522.uid.size-1 ? ":" : "");
    }
  
  //GET THE KEY
  MFRC522::StatusCode status;
  byte byteCount;
  byte i;
  
  // the key is store from page 5 (block 1) to page 10 (block 1) 
  for (byte page = 6; page < 10; page +=4) { // Read returns data for 4 pages at a time.
    // Read pages
    byteCount = sizeof(buffer);
    status = mfrc522.MIFARE_Read(page, buffer, &byteCount);

    #ifdef DEBUG
    // Dump data in a matrix
    for (byte offset = 0; offset < 4; offset++) {
      i = page + offset;
      if(i < 10)
        Serial.print(F("  ")); // Pad with spaces
      else
        Serial.print(F(" ")); // Pad with spaces
        Serial.print(i);
        Serial.print(F("  "));
      for (byte index = 0; index < 4; index++) {
        i = 4 * offset + index;
        if(buffer[i] < 0x10)
          Serial.print(F(" 0"));
        else
          Serial.print(F(" "));
        Serial.print(buffer[i], HEX);
      }
      Serial.println();
    }
   Serial.println("...");
   #endif
   
  }

  Serial.println("..."); 
  payload.toUpperCase();
  Serial.print("UID Tag: ");
  Serial.println(payload);
  delay(100);
  Serial.println("...");   
  //end (got the key)

  delay(10);

    //save the key of the tag in the buffer key_tag
    for (byte i = 1; i <sizeof(key)+1; i++){
          key_tag[i-1] = buffer[i];
    }

    #if DEBUG
    Serial.println("KEY IN THE TAG");
    for (byte i = 0; i <sizeof(key_tag); i++){
          //key_tag[i-1] = buffer[i];
          Serial.print(key_tag[i], HEX);     //print the key from the tag
          Serial.print( i == sizeof(key)-1 ? "\n" : " : ");
    }
    
    Serial.println("...");
    Serial.println("KEY");
    for (byte i = 0; i <sizeof(key); i++){
          Serial.print(key[i], HEX);        //print the key
          Serial.print( i == sizeof(key)-1 ? "\n" : " : ");
    }
    
    Serial.println("...");
    #endif
    
    //compare key and key tag
    byte index_key = 0;
    for (byte i = 0; i <sizeof(key); i++){
            if(key_tag[i] == key[i]) {
              index_key++;
            } else {
              Serial.println("Error in the key");
              Serial.print(key_tag[i], HEX); Serial.print(" / "); Serial.println(key[i], HEX);
            }
         }

    //with index_key I check the single byte of the key
    if (index_key == sizeof(key)){
      //tag_true = 1;
      Serial.println("Key is correct.");
      blinks(0, LedTag, off, green);
      checkin_true = 1;
    } else {
      //tag_true = 0;
      blinks(0, LedTag, red, off);
      checkin_true = 0;  
    }

    //reset the key_tag array for the next card
    for (byte i = 0; i<sizeof(key[i]); i++){
      key_tag[i] = 0; 
    }


   // checkin true => connect to wifi and send id, uid
   if(checkin_true){    
    boolean publish_ok = send_data(payload);
    if(publish_ok) audio_good();
    sleep_true = 1;
   }
//end Tag routine

}


// function that wakes-up esp, connect to wfi and server, send complete payload
// complete payload: id chip, uid tag, rssi, bat
boolean send_data(String payload){
    byte publish_ok = 0;
    long rssi = 0;
    
    WiFi.forceSleepWake();
    delay(1);

    
    WiFi.mode(WIFI_STA);
    if (WiFi.SSID()) {
    Serial.println("Using saved credentials");
    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    WiFi.begin();
      } else {
        Serial.println("No saved credentials");
      }

    WiFi.waitForConnectResult();
    if( WiFi.status() == WL_CONNECTED){
        Serial.println("WiFi connected....");
        rssi = WiFi.RSSI();
        blinks(0, LedTag, off, green); 
        delay(10);
      } else { 
        Serial.println("WiFi NOT connected....");
        blinks(0, LedTag, blue, off);  //light debug: green-blue => tag og but no connection
        }

    ///MQTT PART
     mqttClient.setServer(mqtt_server, mqtt_port);
     chipid = ESP.getChipId();
     delay(10);
     mqttClient.loop();
     delay(10);
     char mqtt_clientid[15];
     sprintf(mqtt_clientid, "cliendit");
      
     if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {   
      Serial.println("mqtt server connected");
      delay(10);

      // send proper JSON startup message
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["id"] = chipid;
      json["uid"] = payload;
      json["rssi"] = rssi;
      char buf[110];
      json.printTo(buf, sizeof(buf));

      Serial.print("Publish message: ");
      Serial.println(buf);

      char topic_buf[50];
      sprintf(topic_buf, mqtt_topic, chipid);
      publish_ok = mqttClient.publish(topic_buf, buf);
      
      if (publish_ok) {blinks(0, LedTag, off, green);}
      else {blinks(0, LedTag, off, aqua);} //light debug: green-aqua => tag ok - data didn't get to the server
      
     } else {
      Serial.println("mqtt server not connected");
      blinks(0, LedTag, off, aqua);  //light debug: green-magenta => tag ok - no server connection
     }

    checkin_true = 0;   

    return publish_ok;
}

// BATTERY
//function to monitor the battery level
int battery_level(){
 
  // 10 bit 0-1023 (0V-1V).
  // 5M & 500K voltage divider
  // lipo value of 4.2V -> 0.382V max.
  // min: 292 (3.14V)
  // max: 391 (4.2V).
  int level = analogRead(BATTERY_PIN);

  #if DEBUG
   Serial.print("Battery level: "); Serial.println(level);
  #endif
  
  // convert battery level to percent
  level = map(level, 580, 774, 0, 100);
  level = constrain(level, 0, 100);

  #if DEBUG
   Serial.print("Battery level mapped: "); Serial.print(level); Serial.println("%");
  #endif
  
  return level;
}

//function to set the Led battery
void battery_light(){
  byte range = battery_level();
  if (range >= 80){
     pixels.setPixelColor(LedBatt, pixels.Color(0, 255, 0));
     pixels.show();
  } else if ( range < 80 &&  range >= 60){
     pixels.setPixelColor(LedBatt, pixels.Color(150, 255, 0));      
     pixels.show();
  } else if (range < 60 &&  range >= 40){
     pixels.setPixelColor(LedBatt, pixels.Color(255, 255, 0));      
     pixels.show();    
  } else if (range < 40 &&  range >= 20){
     pixels.setPixelColor(LedBatt, pixels.Color(255, 100, 0));      
     pixels.show();    
  } else if (range < 20){
     pixels.setPixelColor(LedBatt, pixels.Color(255, 0, 0));      
     pixels.show();      
  }
  
}
//end BATTERY

//function to blink the led
void blinks(byte NUMBLINKS, int LED, uint32_t color1, uint32_t color2){
      for (int i = 0; i<NUMBLINKS+1; i++){
      pixels.setPixelColor(LED, color1);
      pixels.show();
      delay(200);
      pixels.setPixelColor(LED, color2);
      pixels.show();
      delay(200);
      }
}


//function for audio feedback
void audio_good(){
    tone(SPEAK_PIN, 1318, 51);
    delay(56);
    delay(56.818125);
    tone(SPEAK_PIN, 1567, 51);
    delay(56);
    delay(56);
    tone(SPEAK_PIN, 2637, 51);
    delay(56);
    delay(56);
    tone(SPEAK_PIN, 2093, 51);
    delay(56);
    delay(56);
    tone(SPEAK_PIN, 2349, 51);
    delay(56);
    delay(56);
    tone(SPEAK_PIN, 3135, 511);
    delay(568);
    noTone(SPEAK_PIN);
}

void audio_bad(){
//so far empty
}


void check(){

    pixels.setPixelColor(LedBatt, off);
    pixels.show();

    if (usb_true == 1){     
      pixels.setPixelColor(LedBatt, off);  //if using usb => no Led Batt
      pixels.show();
    } else {
    battery_light();                        //if using battery => Leb Batt ok
    }
    
    byte publish_ok = 0;
    long rssi = 0;
    WiFi.forceSleepWake();
    delay(1);

    
    WiFi.mode(WIFI_STA);
    if (WiFi.SSID()) {
    Serial.println("Using saved credentials");
    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    WiFi.begin();
      } else {
        Serial.println("No saved credentials");
      }

    WiFi.waitForConnectResult();
    if( WiFi.status() == WL_CONNECTED){
        Serial.println("WiFi connected....");
        rssi = WiFi.RSSI();
        blinks(3, LedConfig, off, green); 
        delay(10);
      } else { 
        Serial.println("WiFi NOT connected....");
        blinks(3, LedConfig, blue, off);  //light debug: green-red => tag og but no connection
        }

    ///MQTT PART
     mqttClient.setServer(mqtt_server, mqtt_port);
     chipid = ESP.getChipId();
     delay(100);
     mqttClient.loop();
     delay(100);
     char mqtt_clientid[15];
     sprintf(mqtt_clientid, "cliendit");
      
     if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_password)) {   
      blinks(3, LedConfig, off, green);  
      Serial.println("mqtt server connected");
      delay(1000);

      // send proper JSON startup message
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["message"] = "check";
      json["id"] = chipid;
      json["location"] = address;
      json["rssi"] = rssi;
      if (usb_true == 1){
        json["batt"] = "usb";
      } else {
        json["batt"] = battery_level();
      }
      char buf[110];
      json.printTo(buf, sizeof(buf));

      Serial.print("Publish message: ");
      Serial.println(buf);

      char topic_buf[50];
      sprintf(topic_buf, mqtt_topic, chipid);
      publish_ok = mqttClient.publish(topic_buf, buf);
      
      if (publish_ok) {blinks(3, LedConfig, green, off);}
      else {blinks(3, LedConfig, off, aqua);} //light debug: green-aqua => tag ok - data didn't get to the server
      
     } else {
      Serial.println("mqtt server not connected");
      blinks(3, LedConfig, off, aqua);  //light debug: green-magenta => tag ok - no server connection
     }
     pixels.setPixelColor(LedBatt, off);  
  }





