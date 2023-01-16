#ifndef _LIBRARIES_H_
#define _LIBRARIES_H_

    #include <Arduino.h>
    #include "Tasker.h"

    // libraries
    #include <OneWire.h>
    #include <DallasTemperature.h>
    
    // WIFI Autoconnection portal
    #include <ESP8266WiFi.h>
    #include <DNSServer.h>                                  //Local DNS Server used for redirecting all requests to the configuration portal
    #include <ESP8266WebServer.h>                           //Local WebServer used to serve the configuration portal
    #include <WiFiManager.h>                                //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
    
    #include <ESP8266mDNS.h>
    #include <ESP8266HTTPUpdateServer.h>
    //#include <ESP8266httpUpdate.h>

    // FTP server
    #include "ESP8266FtpServer.h"
    #include "FS.h"

    //Json config file over SPIFFS
    #include <ArduinoJson.h>

    // OLED display libraries
    #include <SPI.h>
    #include <Wire.h>
    #include <Adafruit_GFX.h>
    #include <Adafruit_SSD1306.h>
    
    // progect files
    #include "version_history.h"
    #include "settings.h"    
    #include "ssl_cert.h"
    #include "pins.h"
#endif //_LIBRARIES_H_