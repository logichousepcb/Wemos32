#define USER_SSID                 "YOUR SSID"
#define USER_PASSWORD             "YOUR PASSWORD"
#define USER_MQTT_SERVER          "192.168.1.130"    // YOUR MQTT SERVER
#define USER_MQTT_PORT            1883
#define USER_MQTT_USERNAME        "YOUR MQTT USERNAME"
#define USER_MQTT_PASSWORD        "YOUR MQTT USERNAME"
#define USER_MQTT_CLIENT_NAME     "Rack64"         // Used to define MQTT topics, MQTT Client ID, and ArduinoOTA
#define TEMP_PRESENT               1  // 1 for ds19b20 present, 0 for none
#define BME_PRESENT                0  //1 for present on i2c, 0 if not
/******************** CHIP CONFIG SECTION *********************************/

#define MCP0_PRESENT               1            // 1 FOR PRESENT AND 0 FOR NOT
#define MCP1_PRESENT               1
#define MCP2_PRESENT               0
#define MCP3_PRESENT               1
#define MCP4_PRESENT               0

#define MCP0_ADDRESS                           // LEAVE BLANK FOR ADDRESS 0X020  
#define MCP1_ADDRESS               1   
#define MCP2_ADDRESS               2
#define MCP3_ADDRESS               4
#define MCP4_ADDRESS               5

#define MCP0_FUNCTION              INPUT       // USE AS INPUT OR OUTPUT 
#define MCP1_FUNCTION              INPUT
#define MCP2_FUNCTION              INPUT
#define MCP3_FUNCTION              INPUT
#define MCP4_FUNCTION              INPUT

/* THERE ARE 5 MCP CHIPS YOU CAN HAVE, FIRST DEFINE IF IT'S PRESENT, THE DEFINE THE ADDRESS (LEAVE BLANK FOR ADDRESS 0 
 * LASTLY DEFINE IT AS INPUT OR OUTPUT
USE THIS KEY TO DETERMINE ADDRESS TO USE - THEN DEFINE THE FUNCTION OF THE CHIP (INPUT OT OUTPUT)
addr 0 = A2 low , A1 low , A0 low  000 0x20 - for ) leave that value blank
addr 1 = A2 low , A1 low , A0 high 001 0x21
addr 2 = A2 low , A1 high , A0 low  010 0x22
addr 3 = A2 low , A1 high , A0 high  011 0x23
addr 4 = A2 high , A1 low , A0 low  100 0x24
addr 5 = A2 high , A1 low , A0 high  101 0x25
addr 6 = A2 high , A1 high , A0 low  110 0x26
addr 7 = A2 high, A1 high, A0 high 111 0x27

*/
