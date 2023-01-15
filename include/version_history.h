#ifndef _VERSION_HISTORY_H_
#define _VERSION_HISTORY_H_

// ----------------------------------------------------
// >>> Version History      ---------------------------
// ----------------------------------------------------
/*
 *  xx.xx.xxxx - vx.x stable system build
 *  28.03.2019 - v1.0 implement MQTT Mesh protocol
 *  29.03.2019 - v1.0 code cleaning and test
 *  21.06.2019 - v1.1 correct bug
 *    implement #include guard
 *    implement web browser OTA firmware Update
 *    defaule address: http://esp8266-webupdate.local/update
 *  10.10.2019 - v1.2 implement SPIFFS handle
 *    implement html logo
 *    implement FTP server (username: esp8266, password: esp8266)
 *  11.10.2019 - v1.3 implement config json file
 *    implement change meas color with alarm
      http://hostname.local  (viualizzazione misure)
      http://hostname.local/update (aggiornamento firmware via OTA)
      http://hostname.local/readconfig (rilegge la configurazione, la rende attiva e ti rimanda alla visualizzazione misure)
 *  14.10.2019 - v1.3 code cleaning
 *  20.10.2019 - v1.3.1 Implement settings via embedded web page
      http://hostname.local/settings (configurazione sonda termometrica)
      restart ESP when good data saved and redirect to new HOSTNAME address
 *   28.10.2019 - v1.3.2 Implement detachable OLED Display to show node configuration - BETA
 *   04.11.2019 - v1.3.2 Implement multitasking computation, code cleaning
 *   11.11.2019 - v1.3.3 Implement html session control and login with username and password to:
 *        settings.html (admin - admin)
 *        update.html (embedded in libraries)
 *   15.01.2023 - v1.3.3 Implement GitHub repository with version control
 */
// ----------------------------------------------------
// <<< Version History      ---------------------------
// ----------------------------------------------------

#endif //_VERSION_HISTORY_H_