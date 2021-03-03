/* Here is example yaml to add to Home Assistant to read the address and state of the sensors

   
binary_sensor:

  - platform: mqtt
    name: "test front door"
    payload_on: "ON"
    payload_off: "OFF"
    device_class: door
    state_topic: "Rack64/sensorout/0x215"
    value_template: '{{ value_json.state }}'  
  - platform: mqtt
    name: "Panel Status"
    payload_on: "Online"
    payload_off: "Offline"
    device_class: connectivity
    state_topic: "Rack64/checkIn"
    value_template: '{{ value_json.state }}' 
    qos: 1
    retain: true   
 sensor:
  - platform: mqtt
    name: "Alarm Temperature"
    state_topic: "Rack64/temp"
    unit_of_measurement: '°F'
    value_template: "{{ value_json }}"
  
**********************************************

03022021- Added temp sensor support
          Added LWT support (Online/Offline)
03032021- Added RESET to cmnd topic and ability to add on outputs 
*/
#include <SimpleTimer.h>    //https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <ESP8266WiFi.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <ESP8266mDNS.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <PubSubClient.h>   //https://github.com/knolleary/pubsubclient
#include <ArduinoOTA.h>     //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include "logix_secret.h"
#include "Adafruit_MCP23017.h"
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
/*****************  START USER CONFIG SECTION *********************************/
/*****************  START USER CONFIG SECTION *********************************/
// Data wire is plugged into GPIO4 on the Arduino 
#define ONE_WIRE_BUS 14 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
//Version number
#define CURR_VERS                 "1.31w6"
// NETWORK
// this data is contained in logix_secret.h

// LCD
// LiquidCrystal_I2C lcd(0x3F, 16, 2);

// OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// BME Sensor support
Adafruit_BME280 bme; // I2C
float temperature = 0;
float humidity = 0;

// MCP23017
/* 

addr 0 = A2 low , A1 low , A0 low  000 0x20
addr 1 = A2 low , A1 low , A0 high 001 0x21
addr 2 = A2 low , A1 high , A0 low  010 0x22
addr 3 = A2 low , A1 high , A0 high  011 0x23
addr 4 = A2 high , A1 low , A0 low  100 0x24
addr 5 = A2 high , A1 low , A0 high  101 0x25
addr 6 = A2 high , A1 high , A0 low  110 0x26
addr 7 = A2 high, A1 high, A0 high 111 0x27
*/
Adafruit_MCP23017 mcp0;
Adafruit_MCP23017 mcp1;
Adafruit_MCP23017 mcp2;
Adafruit_MCP23017 mcp3;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Redefine the I2C bus
// 17, 16 for esp32mx-e  or 4, 5 for Wemos
#define I2C_SDA 4
#define I2C_SCL 5


/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/

WiFiClient espClient;
PubSubClient client(espClient);
SimpleTimer timer;

//Global Variables
bool boot = true;
char sensorPublish[50];
char charPayload[50];
char * portName[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
char * stateme[2] = {"OFF", "ON"};  // CHANGE TO OPEN OR CLOSE IF YOU LIKE
int pina;
int mcp0buff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int mcp1buff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int mcp2buff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int mcp3buff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


const char* ssid = USER_SSID ; 
const char* password = USER_PASSWORD ;
const char* mqtt_server = USER_MQTT_SERVER ;
const int mqtt_port = USER_MQTT_PORT ;
const char *mqtt_user = USER_MQTT_USERNAME ;
const char *mqtt_pass = USER_MQTT_PASSWORD ;
const char *mqtt_client_name = USER_MQTT_CLIENT_NAME ; 
const int bouncedelay = 150;  // THIS IS A DELAY TO TRY AND PREVENT DOUBLE PRESS OR SENSOR BOUNCE - PROBABLY A BETTER WAY 
byte willQoS = 0;
const char* willTopic = USER_MQTT_CLIENT_NAME"/checkIn";
const char* willMessage = "Offline";
boolean willRetain = true;

/***************************** for recieved msg ***************************/
char* reportTopic = USER_MQTT_CLIENT_NAME"/lastcmnd";
char* reedTopic = USER_MQTT_CLIENT_NAME"/cmnd";

int i;


//Functions
void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.println("Connecting");
    display.setCursor(10, 30);
    display.println(USER_SSID );
    display.display(); 
    delay(500);
    Serial.print(".");
  }
  display.clearDisplay();
  display.setTextSize(1.75);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.println("WiFi CONNECTED");
  display.println(WiFi.localIP());
  display.display(); 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// TEMPERTURE SETUP
void setup_bme(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    // optional have this define whether BME present or not    
    while (1);
  }    
}

void setup_display() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.display(); 
}

void mcp_renew() {
   for (i = 0; i<16; i++) {
    mcp0.pinMode(i, INPUT);
    mcp0.pullUp(i, HIGH);    
    mcp1.pinMode(i, INPUT);
    mcp1.pullUp(i, HIGH);   
    mcp2.pinMode(i, INPUT);
    mcp2.pullUp(i, HIGH);    
    mcp3.pinMode(i, INPUT);
    mcp3.pullUp(i, HIGH);
   }
   for (pina=0;pina<16; pina++) {    // FILL BUFFER WITH CURRENT VALUES OF PINS
    mcp0buff[pina] = mcp0.digitalRead(pina);
    mcp1buff[pina] = mcp1.digitalRead(pina);
    mcp2buff[pina] = mcp0.digitalRead(pina);
    mcp3buff[pina] = mcp3.digitalRead(pina);
   }  
}

void setup_mcp() { 
  Wire.begin(I2C_SDA,I2C_SCL);  
  mcp1.begin(1);
  mcp2.begin(2);
  mcp3.begin(4);   // INTERNAL HUB ON BOARD DISABLED....UNCOMMENT 4 LINES IN THIS SECTION AND CHANGE PORT TO ENABLE
  mcp0.begin();
  for (i = 0; i<16; i++) {
    mcp0.pinMode(i, INPUT);
    mcp0.pullUp(i, HIGH);    
    mcp1.pinMode(i, INPUT);
    mcp1.pullUp(i, HIGH);   
    mcp2.pinMode(i, INPUT);
    mcp2.pullUp(i, HIGH);   
    mcp3.pinMode(i, INPUT);  // INTERNAL HUB ON BOARD
    mcp3.pullUp(i, HIGH);   
  }
   for (pina=0;pina<16; pina++) {    // FILL BUFFER WITH CURRENT VALUES OF PINS
    mcp0buff[pina] = mcp0.digitalRead(pina);
    mcp1buff[pina] = mcp1.digitalRead(pina);
    mcp2buff[pina] = mcp2.digitalRead(pina);
    mcp3buff[pina] = mcp3.digitalRead(pina);   // ACTIVATE THESE LINE FOR INTERNAL HUB

   }
//   report_all_states();
}
  
void reconnect() 
{
  int retries = 0;
  while (!client.connected()) {
    if(retries < 150)
    {
      Serial.print("Attempting MQTT connection...");
      display.setTextSize(1.75);
      display.println("Connecting MQTT...");
      display.display();
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass, willTopic, willQoS, willRetain, willMessage)) 
      {
        Serial.println("connected");
        display.println("connected!");
        display.display();
        delay (5000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 8);
        display.print(" LH ");
        display.println(CURR_VERS);
        display.display();
        if(boot == false)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Reconnected"); 
          client.publish(reportTopic,"Reconnected"); 
        }
        if(boot == true)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Online");
        }
        // ... and resubscribe
        client.subscribe(USER_MQTT_CLIENT_NAME"/sensorin");
        client.subscribe(reportTopic);
        client.subscribe(reedTopic);
      } 
      else 
      {
        display.println("failed, trying again in 5 seconds...");
        display.display();
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    if(retries > 149)
    {
    ESP.restart();
    }
  }
}

void report_to_display(char *imtelling,char *sttop,int d) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  display.print(" LH ");
  display.println(CURR_VERS);
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0,30);
  if (d == 0) {
  display.println("Sending states-MQTT");   
  } else {
    display.println(sttop); //passes the topic
    display.setCursor(0,50); // forces down a line in case topic is long
  }
  display.println(imtelling); 
  display.display(); 
}


// HANDLING MQTT INPUT NOT YET ADDRESSED BUT THIS SECTION IS WHERE TO START TO DO IT
void callback(char* reedTopic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(reedTopic);
 Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
 }
  Serial.println();
  payload[length] = '\0';
  String message = (char*)payload;

  if(message == "RESET"){
    Serial.println("Resetting ESP");
    client.publish(reportTopic,"Reset ESP"); 
    ESP.restart();
    while(message != "RESET"){
      break;
    }
  } 
  if(message == "RESCAN"){
    Serial.println("Rescanning MCP Bus");
    client.publish(reportTopic,"Rescanning MCP Bus"); 
    mcp_renew();
    while(message != "RESCAN"){
      break;
    }
  }
  if(message == "OFF"){
    client.publish(reportTopic, "Off recieved");
    delay(2000);
    while(message != "OFF"){
      break;
    }
  }
}

void checkIn()
{
  client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Online"); 
}

void sensorPub(char *chipname,int n,int v,int k)

// BUILD CHAR ARRAY TO PUBLISH

{
  StaticJsonDocument<256> doc;
  char addbuf[6];
  char addresstopic[50];

  strcpy(addresstopic,USER_MQTT_CLIENT_NAME);
  strcat(addresstopic,"/sensorout/");
  strcpy(addbuf,chipname);
  strcat(addbuf, portName[n]);
  strcat(addresstopic,addbuf); //make a topic with the address name
  doc["chip"] = chipname;
  doc["port"] = n;
  doc["address"] = addbuf;
  doc["state"] = stateme[v];

  
  char mybuffer[256];
  serializeJson(doc, mybuffer);
  client.publish(addresstopic, mybuffer);// THIS IS WHERE TO PUBLISH THE SENSOR DATA
  char buf[50];
  const char *second = " - ";
  strcpy(buf,chipname);
  strcat(buf,second);
  strcat(buf, portName[n]);
  strcat(buf,second);
  strcat(buf, stateme[v]); 
  report_to_display(buf,addresstopic,k); // THIS IS WHERE TO PRINT TO SCREEN TOPIC AND CHANGE
  Serial.println(buf);  // THIS IS SHOW IN SERIAL THE SENSOR DATA
 

}

int pini;
int pinread[16];

void pincompare() {
//  Serial.println("Reading pins.");
  for (pini=0;pini<16; pini++) {
    pinread[pini] = mcp2.digitalRead(pini);  // DEBOUNCE HERE
    if(pinread[pini] != mcp2buff[pini]){
      delay (bouncedelay);
      pinread[pini] = mcp2.digitalRead(pini);
       if(pinread[pini] != mcp2buff[pini]){
         mcp2buff[pini] = pinread[pini];  // ONLY PUBLISH AND SAVE TO BUFFER IF CHANGE EXISTS FOR bouncedelay TIME
         sensorPub ("0x22",pini,mcp2buff[pini],1);
       }
    } 
    pinread[pini] = mcp0.digitalRead(pini);
    if(pinread[pini] != mcp0buff[pini]){
     delay (bouncedelay);
     pinread[pini] = mcp0.digitalRead(pini);
       if(pinread[pini] != mcp0buff[pini]){
         mcp0buff[pini] = pinread[pini];
         sensorPub ("0x20",pini,mcp0buff[pini],1);
       }
    }
    pinread[pini] = mcp1.digitalRead(pini);
    if(pinread[pini] != mcp1buff[pini]){
     delay (bouncedelay);
     pinread[pini] = mcp1.digitalRead(pini);
       if(pinread[pini] != mcp1buff[pini]){
         mcp1buff[pini] = pinread[pini];
         sensorPub ("0x21",pini,mcp1buff[pini],1);
       }      
    }
    pinread[pini] = mcp3.digitalRead(pini);
    if(pinread[pini] != mcp3buff[pini]){
     delay (bouncedelay);
     pinread[pini] = mcp3.digitalRead(pini);
       if(pinread[pini] != mcp3buff[pini]){
         mcp3buff[pini] = pinread[pini];
         sensorPub ("0x24",pini,mcp3buff[pini],1);
      }    
    }
  }
  delay (10);
}


//Run once setup
void setup() {
  Serial.begin(115200);
  setup_mcp();                           
  pinMode(5, OUTPUT);             
  if (BME_PRESENT == 1) {
    setup_bme();
  } 
  setup_display();
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  if (!client.connected()) 
  {
    reconnect();
  }
  report_all_states();
  display.setCursor(10,50);
  display.setTextSize(2);
  display.println("READY...");
  display.display();
  Serial.println ("States read...ready for action!");
  ArduinoOTA.setHostname(USER_MQTT_CLIENT_NAME);
  ArduinoOTA.begin(); 
  delay(10);
}

void report_all_states(){
   
   for (pina=0;pina<16; pina++) {    // THIS SECTION REPORTS TO MQTT ALL STATES OF ALL PINS SO HA IS CORRECT ON REBOOT
                                     // PLAN TO ADD A REFRESH BUTTON TO RUN THIS IS HA SERVER RESTARTS TO RETRIEVE THIS DATA ALSO
     mcp0buff[pina] = mcp0.digitalRead(pina);
     sensorPub ("0x20",pina,mcp0buff[pina],0);
     delay (80);                     //DELAY MIGHT NOT BE NEEDED, I WANTED TO PREVENT HAMMERING THE MQTT SERVER 
     mcp1buff[pina] = mcp1.digitalRead(pina);
     sensorPub ("0x21",pina,mcp1buff[pina],0);
     delay (80);
     mcp2buff[pina] = mcp2.digitalRead(pina);
     sensorPub ("0x22",pina,mcp2buff[pina],0);
     delay (80);
     mcp3buff[pina] = mcp3.digitalRead(pina);   // ACTIVATE THESE LINE FOR INTERNAL HUB
     sensorPub ("0x24",pina,mcp3buff[pina],0);
     delay (80);
   }
}
long lastMsgb = 0;
void BME_temp() {
  long now = millis();
  if (now - lastMsgb > 90000) {
    lastMsgb = now;
    
    // Temperature in Celsius
    // temperature = bme.readTemperature();   
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
    
    // Convert the value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish(USER_MQTT_CLIENT_NAME"/temp", tempString);

    humidity = bme.readHumidity();
    
    // Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish(USER_MQTT_CLIENT_NAME"/humidity", humString);
    client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Online");
  }
}

long lastMsg = 0;
void rack_temp(){
  long now = millis();
  if (now - lastMsg > 90000) {
    lastMsg = now;
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  Serial.print("Temperature is: "); 
  Serial.print(sensors.getTempCByIndex(0)); 
  char tempString[8];
  dtostrf(sensors.getTempCByIndex(0), 1, 2, tempString);
  client.publish(USER_MQTT_CLIENT_NAME"/temp",tempString);
  client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Online");
  
  }      
    // Convert the value to a char array
 //   char tempString[8];
 //   dtostrf(temperature, 1, 2, tempString);
 //   Serial.print("Temperature: ");
 //   Serial.println(tempString);
 //   client.publish("esp32/temperature", tempString);
  
}
void loop() 

{
  if (!client.connected()) 
  {
    reconnect();
  }
// HERE NEEDS TO GO A ROUTINE FOR CHECKING SENSOR DATA AGAINST LAST STATE
  client.loop();
  
  pincompare();
  if (TEMP_PRESENT == 1) {
    rack_temp();
  }
  if (BME_PRESENT == 1) {
    BME_temp();
 } 
//  repeatedloop();  // THIS JUST FOR TESTING DISPLAYS BINARY VALUE OF PINS ON 0X20
  ArduinoOTA.handle();
  timer.run();
}
