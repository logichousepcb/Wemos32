/******************** NETWORD/MQTT CONFIG SECTION *********************************/
#define USER_SSID                 "ESP"
#define USER_PASSWORD             "doodlehead"
#define USER_MQTT_SERVER          "192.168.1.130"
#define USER_MQTT_PORT            1883
#define USER_MQTT_USERNAME        "esp"
#define USER_MQTT_PASSWORD        "doodlehead"
#define USER_MQTT_CLIENT_NAME     "TestUnit64"         // Used to define MQTT topics, MQTT Client ID, and ArduinoOTA
#define USER_MQTT_DISCOVERY_PREFIX "homeassistant"
// INSIDE YOUR PUBSUB INCREASE BUFFER SIZE
#define USER_DISCOVEY              true                // true or false
/******************** ON BOARD SENSORS *********************************/
#define TEMP_PRESENT               1  // 1 for ds19b20 present, 0 for none
#define BME_PRESENT                0  //1 for present on i2c, 0 if not


/******************** I2C CONFIG SECTION *********************************/
// 17, 16 for esp32mx-e  or 4, 5 for Wemos or 14, 15 doe WT32-ETH01 
// #define I2C_SDA 4
//#define I2C_SCL 5
// THIS IS NOW AT THE TOP OF THE CODE DEFINED BY BOARD TYPE
/******************** ETHERNET CONFIG SECTION *********************************/
#define ETH_ADDR      0      // 1 for WT32-ETH01?, 0 for MXE
#define ETH_POWER_PIN -1    // There is no power pin, use -1 to disable the code (16 foe WT32-ETH01)
#define ETH_MDC_PIN   23
#define ETH_MDIO_PIN  18
#define ETH_TYPE      ETH_PHY_LAN8720
#ifdef ETH_CLK_MODE
#undef ETH_CLK_MODE
#endif
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_OUT  //GPIO0_IN for WT32-ETH01, GPIO0_OUT for MXE

/******************** OLED CONFIG SECTION *********************************/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


/******************** CHIP CONFIG SECTION *********************************/

#define MCP0_PRESENT               true            // true or false
#define MCP1_PRESENT               false
#define MCP2_PRESENT               false
#define MCP3_PRESENT               false
#define MCP4_PRESENT               false

#define MCP0_ADDRESS               1            // LEAVE BLANK FOR ADDRESS 0X020  
#define MCP1_ADDRESS               4   
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
