#ifndef _MAIN_H_
#define _MAIN_H_
    #include "libraries.h"

    // wifi node configuration options
    #define AS_WIFI_STATION       0
    #define AS_WIFI_ACCESS_POINT  1
    #define AS_WIFI_MQTTMESH_NODE 2

    #define UID_HOSTNAME          0
    #define USER_HOSTNAME         1

    // force wifi configuration if userconfig wifi wrong
    #ifndef NODE_WIFI_SETTINGS
      #define NODE_WIFI_SETTINGS AS_WIFI_ACCESS_POINT
    #endif

    struct _conf MystdConfig;
    
    //multitask enable
    Tasker tasker;
    
    //String WIFI_AP_NAME = "";
    String HOSTNAME = "";
    String WIFI_CHIP_ID = WiFi.hostname();
    String HOST = "";

    int durationTemp = 0;                                 //The frequency of temperature measurement
    long lastTemp;                                        //The last measurement

    #define SCREEN_WIDTH 128 // OLED display width, in pixels
    #define SCREEN_HEIGHT 64 // OLED display height, in pixels

    // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
    #define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    #define NUMFLAKES     10 // Number of snowflakes in the animation example

    #define LOGO_HEIGHT   16
    #define LOGO_WIDTH    16

    static const unsigned char PROGMEM logo_bmp[] =
    { B00000000, B11000000,
      B00000001, B11000000,
      B00000001, B11000000,
      B00000011, B11100000,
      B11110011, B11100000,
      B11111110, B11111000,
      B01111110, B11111111,
      B00110011, B10011111,
      B00011111, B11111100,
      B00001101, B01110000,
      B00011011, B10100000,
      B00111111, B11100000,
      B00111111, B11110000,
      B01111100, B11110000,
      B01110000, B01110000,
      B00000000, B00110000 };

    // ----------------------------------------------------
    //      WEB OTA
    // ----------------------------------------------------
    ESP8266WebServer server(80);
    ESP8266HTTPUpdateServer httpUpdater;
    // ----------------------------------------------------
    //      WEB OTA
    // ----------------------------------------------------
    
    FtpServer ftpSrv;

    // ----------------------------------------------------
    // >>> Variable definition/prototipization for MQTTMesh
    // ----------------------------------------------------
    #if NODE_WIFI_SETTINGS == AS_WIFI_MQTTMESH_NODE
      #include <ESP8266MQTTMesh.h>
      #define FIRMWARE_ID  MystdConfig.MQTT_FIRMWARE_ID
      #define FIRMWARE_VER MystdConfig.MQTT_FIRMWARE_VER
        
      wifi_conn  networks[] = NETWORK_LIST;
      const char* mesh_password = MystdConfig.MESH_PASSWORD.c_str();
      const char* mqtt_server = MystdConfig.MQTT_SERVER.c_str();
      const int mqtt_port = MystdConfig.MQTT_SERVER_TCP_PORT;

      const char* in_topic = MystdConfig.MQTT_TOPIC_IN.c_str();
      const char* out_topic = MystdConfig.MQTT_TOPIC_OUT.c_str();

      #if ASYNC_TCP_SSL_ENABLED
        const uint8_t *mqtt_fingerprint = MQTT_FINGERPRINT;
        bool mqtt_secure = MQTT_SECURE;
        #if MESH_SECURE
          #include "ssl_cert.h"
        #endif
      #endif

      #ifdef ESP32
        String ID  = String((unsigned long)ESP.getEfuseMac());
      #else
        String ID  = String(ESP.getChipId());
      #endif

      // Note: All of the '.set' options below are optional.  The default values can be
      // found in ESP8266MQTTMeshBuilder.h
      ESP8266MQTTMesh mesh = ESP8266MQTTMesh::Builder(networks, mqtt_server, mqtt_port)
                                            .setVersion(FIRMWARE_VER, FIRMWARE_ID)
                                            .setMeshPassword(mesh_password)
                                            .setTopic(in_topic, out_topic)
      #if ASYNC_TCP_SSL_ENABLED
                                            .setMqttSSL(mqtt_secure, mqtt_fingerprint)
        #if MESH_SECURE
                                            .setMeshSSL(ssl_cert, ssl_cert_len, ssl_key, ssl_key_len, ssl_fingerprint)
        #endif //MESH_SECURE
      #endif //ASYNC_TCP_SSL_ENABLED
                                            .build();

      void callback(const char *topic, const char *msg);

      unsigned long previousMillis = 0;
      const long interval = 5000;
      int cnt = 0;
    #endif
    // ----------------------------------------------------
    // <<< Variable definition/prototipization for MQTTMesh
    // ----------------------------------------------------

    // ----------------------------------------------------
    // >>> Variable definition/prototipization for DS18B20
    // ----------------------------------------------------
    #define ONE_WIRE_BUS D2                              //Pin to which is attached a temperature sensor
    #define ONE_WIRE_MAX_DEV 64                          //The maximum number of devices

    DeviceAddress devAddr[ONE_WIRE_MAX_DEV];              //An array device temperature sensors
    float tempDevRaw[ONE_WIRE_MAX_DEV];                   //Saving the last measurement of temperature
    float tempDevLastRaw[ONE_WIRE_MAX_DEV];               //Previous temperature measurement
    int tempDevPositionRaw[ONE_WIRE_MAX_DEV];            //Store temperature in-line physical position

    DeviceAddress tempDevAddr[ONE_WIRE_MAX_DEV];          //An array device temperature sensors
    float tempDev[ONE_WIRE_MAX_DEV];                      //Saving the last measurement of temperature
    float tempDevLast[ONE_WIRE_MAX_DEV];                  //Previous temperature measurement
    int tempDevPosition[ONE_WIRE_MAX_DEV];               //Store temperature in-line physical position
    
    int numberOfDevices;                                  //Number of temperature devices found
    char temperatureString[6];
    String sensorName = "";

    //DS18B20 1Wire bus inizialization ---------
    OneWire oneWire(ONE_WIRE_BUS);
    DallasTemperature DS18B20(&oneWire);
    // ----------------------------------------------------
    // <<< Variable definition/prototipization for DS18B20
    // ----------------------------------------------------

    // ----------------------------------------------------
    // >>> Prototyoing functions
    // ----------------------------------------------------
    void SetupDS18B20();
    String GetAddressToString(DeviceAddress deviceAddress);
    void TempLoop2(long now);
    void TempLoop3();
    int swapBytes(int x);
    void getSpiff_file(String path, String TyPe);
    void buildWebsite();
    void buildXML();
    void buildJavascript();
    void handleWebsite();
    void handleFormSettings();
    void handleXML();

    String getContentType(String filename);                 // convert the file extension to the MIME type
    bool handleFileRead(String path);                       // send the right file to the client (if it exists)

    void ReadConfig();
    void HandleNotFound();
    bool loadConfig();
    bool saveConfig();
    void restartESP();

    void testdrawbitmap(void);
    // ----------------------------------------------------
    // <<< Prototyoing functions
    // ----------------------------------------------------

#endif //_MAIN_PROGRAM_H_
