# STD-WIFI-ESP8266_D1PRO
## _WIFI Temperature sensor (DS18B20) with MQTT MESH capability_
### Features
- WIFI Captive portal for new WIFI connection configuration
- MQTT MESH protocol
- OTA Firmware update
- FTP Server
- Detachable OLED Display to show configuration
- HTML session control and data encryption

### Version History

 - xx.xx.xxxx - vx.x stable system build
 - 28.03.2019 - v1.0 implement MQTT Mesh protocol
 - 29.03.2019 - v1.0 code cleaning and test
 - 21.06.2019 - v1.1 correct bug
    - implement #include guard
    - implement web browser OTA firmware Update
    - defaule address: http://esp8266-webupdate.local/update
 - 10.10.2019 - v1.2 implement SPIFFS handle
    - implement html logo
    - implement FTP server (username: esp8266, password: esp8266)
 - 11.10.2019 - v1.3 implement config json file
 - implement change meas color with alarm
     - http://hostname.local  (viualizzazione misure)
     - http://hostname.local/update (aggiornamento firmware via OTA)
     - http://hostname.local/readconfig (rilegge la configurazione, la rende attiva e ti rimanda alla visualizzazione misure)
 - 14.10.2019 - v1.3 code cleaning
 - 20.10.2019 - v1.3.1 Implement settings via embedded web page
    - http://hostname.local/settings (configurazione sonda termometrica)
    - restart ESP when good data saved and redirect to new HOSTNAME address
 - 28.10.2019 - v1.3.2 Implement detachable OLED Display to show node configuration - BETA
 - 04.11.2019 - v1.3.2 Implement multitasking computation, code cleaning
 - 11.11.2019 - v1.3.3 Implement html session control and login with username and password to:
    - settings.html (admin - admin)
    - update.html (embedded in libraries)

## Code status
[![.github/workflows/main.yml](https://github.com/kattivik76/STD-WIFI-ESP8266_D1PRO/actions/workflows/main.yml/badge.svg?branch=main&event=release)](https://github.com/kattivik76/STD-WIFI-ESP8266_D1PRO/actions/workflows/main.yml)

## Make a donation
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/donate/?hosted_button_id=38LNBHDPTZUU6)


