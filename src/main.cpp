#include "main.h"

void setup() {
  
  //Setup Serial port speed
  Serial.begin(115200);
  delay(1000); //This is only here to make it easier to catch the startup messages.  It isn't required
  Serial.println("");
  
  WiFi.begin("FRITZ!Box 7590 XO","07532074360330867860");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  //if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  //  Serial.println(F("SSD1306 allocation failed"));
  //  for(;;); // Don't proceed, loop forever
  //}
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...
  
  testdrawbitmap();    // Draw a small bitmap image

  SPIFFS.begin();
  Serial.println(F("Whaiting SPIFFS.begin()"));
  while (!SPIFFS.begin()) {
  // statement(s)
    delay(200);
    Serial.print(".");
  }

  //Config json file read and write
  if (SPIFFS.exists("/config.json")){
    ReadConfig();
    //MAIN CONFIGURATION
    if (HOSTNAME_TYPE == UID_HOSTNAME){
      HOSTNAME = MystdConfig.HostNameSuffix + WIFI_CHIP_ID;
    }else if (HOSTNAME_TYPE == USER_HOSTNAME){
      HOSTNAME = MystdConfig.UserHostName;
    }
    if (!saveConfig()) {
      Serial.println(F("Failed to save config"));
    } else {
      Serial.println(F("Config saved"));
    }
  } else {
    if (!saveConfig()) {
      Serial.println(F("Failed to save config"));
    } else {
      Serial.println(F("Config saved"));
      ReadConfig();
      Serial.println(F("Config re-loaded"));
    }
  }

  //MAIN CONFIGURATION
  if (HOSTNAME_TYPE == UID_HOSTNAME){
    HOSTNAME = MystdConfig.HostNameSuffix + WIFI_CHIP_ID;
  }else if (HOSTNAME_TYPE == USER_HOSTNAME){
    HOSTNAME = MystdConfig.UserHostName;
  }

  HOST = HOSTNAME;
  
  const char* host = HOST.c_str();
  Serial.println(host);

  #if NODE_WIFI_SETTINGS == AS_WIFI_MQTTMESH_NODE
    //Setup WIFI MQTTMesh network stack
    delay(500);
    mesh.setCallback(callback);
    Serial.println(F("Setting mesh.setCallback()"));
    mesh.begin();
    Serial.println(F("Start MQTTMESH network mesh.begin()"));
  #endif

  #if NODE_WIFI_SETTINGS == AS_WIFI_STATION
    //Setup WIFI
    Serial.println("");
    WiFiManager wifiManager;
    //wifiManager.setCountry("US");
    //wifiManager.setWiFiAPChannel(13);
    wifiManager.setRemoveDuplicateAPs(true);
    wifiManager.setTimeout(60);
    Serial.println(F("WifiManager timeout: 180sec"));
    
    Serial.println(F("Whaiting WM connection"));
    wifiManager.setDebugOutput(true); 
    while(!wifiManager.autoConnect(host, "abcd12345")) {
      //Serial.print(".");
      //delay(200);
      ESP.restart();
      delay(5000);
    }
    
    Serial.println("ChipID: " + WiFi.hostname());
  #endif

  #if NODE_WIFI_SETTINGS == AS_WIFI_ACCESS_POINT
    /* Set these to your desired credentials. */
    const char *ssid = host;
    const char *password = MystdConfig.WIFI_AP_PASSWORD.c_str();

    Serial.println(F("Configuring access point..."));
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);
    //WiFi.setOutputPower(82);
    
    delay(1000);
    WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(ssid, password, 6);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("");
    Serial.print(F("AP IP address: "));
    Serial.println(myIP);
    Serial.println(ssid);
    Serial.println(password);

    WiFi.printDiag(Serial);
    //Serial.println("wifi RSSI: " + WiFi.RSSI());
    //Serial.println("Hostname: " + WiFi.hostname());
  #endif

  #if (NODE_WIFI_SETTINGS == AS_WIFI_STATION) | (NODE_WIFI_SETTINGS == AS_WIFI_ACCESS_POINT)
    
    MDNS.begin(host);
    MDNS.addService("http", "tcp", 80);
    delay(5000);
    server.begin();
    // web OTA Update portal
    httpUpdater.setup(&server); 
    httpUpdater.setup(&server, update_username, update_password);
    
    delay(500);
    Serial.println("HTTP server started at ip " + WiFi.localIP().toString());

    //SPIFFS web settings
    server.on("/submit", HTTP_POST, handleFormSettings);    //recieve form settings data
    server.on("/",handleWebsite);                           //request without page file
    server.on("/xml",handleXML);                            //request without page file
    server.on("/readconfig",ReadConfig);                    //request without page file

    server.onNotFound([]() {                                // If the client requests any URI
    if (!handleFileRead(server.uri()))                      // send it if it exists
      server.send(404, "text/plain", "404: Not Found");     // otherwise, respond with a 404 (Not Found) error
  });

  #endif
  
  //set sample time
  if (MystdConfig.SAMPLE < 30){                                       //sample time min 30s
    durationTemp = (1*1000)*MystdConfig.SAMPLE;                       //The frequency of temperature measurement
  }else{
    MystdConfig.SAMPLE = 30;
    durationTemp = (1*1000)*MystdConfig.SAMPLE;                       //The frequency of temperature measurement
  };

  //Setup FTP server
  Serial.println(F("FTP server started!"));
  ftpSrv.begin(MystdConfig.FTP_USERNAME, MystdConfig.FTP_PASSWORD);   // username, password for ftp. Set ports in ESP8266FtpServer.h (default 21, 50009 for PASV)
  
  //Setup DS18b20 temperature sensor
  SetupDS18B20();
  delay(500);

  // create multitasking process to reading temperature
  tasker.setInterval(TempLoop3, durationTemp);

  // Pin mapping
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
}

void loop() {
  //multitask for ESPrestart
  tasker.loop();

  #if (NODE_WIFI_SETTINGS == AS_WIFI_STATION) | (NODE_WIFI_SETTINGS == AS_WIFI_ACCESS_POINT)
    server.handleClient();    //multitasking
    MDNS.update();            //multitasking
    ftpSrv.handleFTP();       //multitasking
  #endif

  #if NODE_WIFI_SETTINGS == AS_WIFI_MQTTMESH_NODE

    if (! mesh.connected())
          return;

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {

      String cntStr = String(cnt);
      String msg = ""; //build_json();

      for(int i=0;i<numberOfDevices;i++){

        dtostrf(tempDev[i], 2, 2, temperatureString);
        sensorName = String(ID) + "/sensor_" + String(i);
        msg = temperatureString;
        mesh.publish(sensorName.c_str(), msg.c_str());
        yield();
      }
          previousMillis = currentMillis;
          cnt++;
    }
    #endif
}

#if NODE_WIFI_SETTINGS == AS_WIFI_MQTTMESH_NODE
  void callback(const char *topic, const char *msg) {
    if (0 == strcmp(topic, (const char*) ID.c_str())) {
      if(String(msg) == "0") {
        digitalWrite(LED1, HIGH);
      }else{
        digitalWrite(LED1, LOW);
      }
    }
  }
#endif

// -----------------------------------------------------------
// ------------------------ Functions ------------------------
// -----------------------------------------------------------
//Setting the temperature sensor
void SetupDS18B20(){
  DS18B20.begin();
  
  Serial.print(F("Parasite power is: "));
  if( DS18B20.isParasitePowerMode() ){
    Serial.println(F("ON"));
  }else{
    Serial.println(F("OFF"));
  }
  
  numberOfDevices = DS18B20.getDeviceCount();
  Serial.print(F( "Device count: " ));
  Serial.println( numberOfDevices );

  lastTemp = millis();
  DS18B20.requestTemperatures();

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if( DS18B20.getAddress(devAddr[i], i) ){
       GetAddressToString(devAddr[i]);
    }else{
      Serial.print(F("Found ghost device at "));
      Serial.print(i, DEC);
      Serial.print(F(" but could not detect address. Check power and cabling"));
    }

    //Read physical in-line position of DS18b20
    tempDevPositionRaw[i] = swapBytes(DS18B20.getUserDataByIndex(i))-1;
    yield();
  }
}

//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress){
  String str = "";
  for (uint8_t i = 0; i < 8; i++){
    if( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
    yield();
  }
  return str;
}

void TempLoop3(){
  int index = 0;
  
  DS18B20.setWaitForConversion(false);                      //No waiting for measurement
  DS18B20.requestTemperatures();

  for(int i=0; i<numberOfDevices; i++){
    yield();
    DS18B20.getAddress(devAddr[i], i);                      // read device address from ds18b20
    tempDevRaw[i] = DS18B20.getTempCByIndex(i);
    tempDevPositionRaw[i] = swapBytes(DS18B20.getUserDataByIndex(i))-1;
 
    index = tempDevPositionRaw[i];                          // read userdata (scratchpad+2 = in-line physical position)
    tempDevPosition[index] = index;                         // create in-line physical position index
    DS18B20.getAddress(tempDevAddr[index], i);              // read device address from ds18b20 in-line physical position index
    tempDev[index] = tempDevRaw[i];                         // read temperature from index and store in in-line physical position index
  }
}

int swapBytes(int x){
  byte hi, lo;
  hi = x >> 8;
  lo = x;
    return ( hi | lo);
}

void getSpiff_file(String path, String TyPe) { 
 if(SPIFFS.exists(path)){ 
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, TyPe);
    file.close();
  }
}

//Website
String javaScript, webSite, XML;

void buildWebsite(){
  char temperatureString[6];

  buildJavascript();
  webSite =  F("<!DOCTYPE HTML>\n");
  webSite += F("<html lang=\"en-US\">\n");
  webSite += F("<head>\n");
  webSite += F("<title>STP-WIFI-D1PRO visualizer </title>\n");
  webSite += F("<meta charset=\"utf-8\">\n");
  webSite += F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n");
  webSite += F("<meta name=\"Description\" content=\"Multipoint temperature sensor visualizer\">\n");
  webSite += F("<meta name=\"author\" content=\"Alessandro Migliorini\">\n");
  webSite += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"buttons.css\">\n");
  webSite += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"styles.css\">\n");
   webSite += F("<script type=\"text/javascript\" src=\"functions.js\"></script>\n");
  webSite += F("</head>\n");
  webSite += javaScript;

  webSite += F("<BODY onload='process()'>\n");
//add logo image to html page
  webSite += F("<table width=\"80%\" align=center>\n"
                "<tr>\n"
  	              "<td style='text-align:center;vertical-align:middle'>\n"
                    "<div class=\"header\">\n"
                      "<img src=\"/logo.png\" alt=\"logo\" />\n"
                      "</div>\n"
                      "<h1>Sonda termometrica ");
  webSite +=          WiFi.hostname();
  webSite +=          F("</h1>\n");
  webSite +=          F("<h2>TAG:  ");
  webSite +=            MystdConfig.UserDescriptions;
  webSite +=          F("</h2>\n");
  webSite +=          F("<h3>Numero di punti: ");
  webSite +=            numberOfDevices;
  webSite +=          F("</h3>\n"                    
                  "</td>\n"
                "</tr>\n"
              "</table>\n");
  webSite += F("<center><table class=blueTable>\n");
  webSite += F("<thead>\n");
  webSite += F("<tr>\n");
  webSite += F("<th>Punto</th>\n");
  webSite += F("<th>Misura</th>\n");
  webSite += F("</tr>\n");
  webSite += F("</thead>\n");
  webSite += F("<tbody>\n");

  for(int i=0;i<numberOfDevices;i++){
    dtostrf(tempDev[i], 2, 2, temperatureString);
    webSite += F("<tr>\n");
    webSite += "<td id=r" + String(i+1) + "c1" + ">\n";
    webSite += tempDevPosition[i]+1;
    webSite += "</td>\n";
    webSite += "<td id=r" + String(i+1) + "c2" + ">\n";
    webSite += temperatureString;
    webSite += "</td>\n";
    webSite += "</tr>\n";
    yield();
  }
  webSite += F("</tbody>\n"
              "</table>\n");
  webSite += F("<table width=\"80%\" align=center>\n"
                "<tr>\n"
  	              "<td style='text-align:left;vertical-align:middle'>\n"
                    //"<button class=\"btn success\" onclick=\"location.href='settings.html'\">Settings</button>\n"
                    "<button class=\"btn success\" onclick=\"openForm()\">Settings</button>\n"
                    "<button class=\"btn warning\" onclick=\"location.href='update'\">OTA Update FW</button>\n"  
                  "</td>\n"
                "</tr>\n"
              "</table>\n");
                // popup login
  webSite += F("<div id=\"loginPopup\">\n"
                  "<div class=\"form-popup\" id=\"popupForm\">\n"
                    "<form class=\"form-container\">\n"
                      "<h2>Please Log in</h2>"
                      "<label for=\"email\">\n"
                        "<strong>Username</strong>\n"
                      "</label>\n"
                        "<input type=\"text\" id=\"username\" placeholder=\"username\" name=\"username\" autocomplete=\"off\" required>\n"
                      "<label for=\"password\">\n"
                        "<strong>Password</strong>\n"
                      "</label>\n"
                      "<input type=\"password\" id=\"password\" placeholder=\"Your Password\" name=\"password\" autocomplete=\"off\" required>\n"
                      "<button type=\"button\" class=\"btn\" onclick='validate()'>Log in</button>\n"
                      "<button type=\"button\" class=\"btn cancel\" onclick=\"closeForm()\">Close</button>\n"
                      "</form>\n"
                    "</div>\n"
                  "</div>\n");                
  webSite += F("</BODY>\n");
  webSite += F("</html>\n");
}

void buildXML(){
  char temperatureString[6];

  XML="<?xml version='1.0'?>";
  XML+="<cord>";
    for(int i=0;i<numberOfDevices;i++){
      dtostrf(tempDev[i], 2, 2, temperatureString);
      XML += "<sensor>";
      XML += "<pos>";
      XML += (tempDevPosition[i]+1);
      XML += "</pos>";
      XML += "<meas>";
      XML += temperatureString;
      XML += "</meas>";
      XML += "</sensor>";
      yield();
    }
  XML+="</cord>";
}

void buildJavascript(){
  javaScript =  "<SCRIPT>\n";
  javaScript += "var xmlHttp=createXmlHttpObject();\n";

  javaScript += "function createXmlHttpObject(){\n";
  javaScript += " if(window.XMLHttpRequest){\n";
  javaScript += "    xmlHttp=new XMLHttpRequest();\n";
  javaScript += " }else{\n";
  javaScript += "    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript += " }\n";
  javaScript += " return xmlHttp;\n";
  javaScript += "}\n";

  javaScript += "function process(){\n";
  javaScript += " if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript += "   xmlHttp.open('PUT','xml',true);\n";
  javaScript += "   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript += "   xmlHttp.send(null);\n";
  javaScript += " }\n";
  javaScript += " setTimeout('process()',";
  javaScript +=   durationTemp;
  javaScript += " );\n";
  javaScript += "}\n";

  javaScript += "function handleServerResponse(){\n";
  javaScript += " if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript += "   xmlResponse=xmlHttp.responseXML;\n";
  javaScript += "   x = xmlResponse.getElementsByTagName('meas');\n";
  javaScript += "   for (i = 0; i < x.length ;i++) {\n";
  javaScript += "     var pointer;\n";
  javaScript += "     var temp;\n";
  javaScript += "     pointer = i + 1;\n";
  javaScript += "     temp = 'r' + pointer + 'c1';\n";
  javaScript += "     document.getElementById(temp).innerHTML = pointer;\n";
  javaScript += "     temp = 'r' + pointer + 'c2';\n";
  javaScript += "     document.getElementById(temp).innerHTML = x[i].childNodes[0].nodeValue;\n";
  javaScript += "     //document.getElementById('r1c1').innerHTML = temp;\n";
  javaScript += "     if (x[i].childNodes[0].nodeValue > " + String(MystdConfig.Tmax) + ") {\n"
                "       document.getElementById(temp).style.color = \"red\"\n"
                "     }\n"
                "     else if (x[i].childNodes[0].nodeValue < " + MystdConfig.Tmin + ") {\n"
                "       document.getElementById(temp).style.color = \"blue\"\n"
                "     }\n"
                "     else {\n"
                "       document.getElementById(temp).style.color = \"black\"\n"
                "     }\n";
  javaScript += "   }\n";
  javaScript += " }\n";
  javaScript += "}\n";
  javaScript += "</SCRIPT>\n";
}

void handleWebsite(){
  buildWebsite();
  server.send(200,"text/html",webSite);
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}

void ReadConfig(){
  if (!loadConfig()) {
    Serial.println(F("Failed to load config"));
  } else {
    Serial.println(F("Config loaded"));
  }
  server.sendHeader("Location", "/",true); //Redirect to our html web page 
  server.send(302, "text/plane",""); 
}

void HandleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/html", message);
}

//----------------------------------------
String getContentType(String filename) {                    // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".jpg")) return "image/jpg";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";  
  return "text/plain";
}

bool handleFileRead(String path) {                          // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";             // If a folder is requested, send the index file
  if (path.endsWith("/submit")) handleFormSettings();
  String contentType = getContentType(path);                // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {   // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                        // Use the compressed verion
    File file = SPIFFS.open(path, "r");                     // Open the file
    size_t sent = server.streamFile(file, contentType);     // Send it to the client
    file.close();                                           // Close the file again
    return true;
  }
  return false;
}
//----------------------------------------

void handleFormSettings(){
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "hostname_suffix") {
         MystdConfig.HostNameSuffix = server.arg(i).c_str(); 
      }
      if (server.argName(i) == "user_hostname") {
         MystdConfig.UserHostName = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "wifi_ap_password") {
         MystdConfig.WIFI_AP_PASSWORD = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "ftp_username") {
         MystdConfig.FTP_USERNAME = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "ftp_password") {
         MystdConfig.FTP_PASSWORD = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "sample") {
         MystdConfig.SAMPLE = server.arg(i).toInt(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "tmax") {
         MystdConfig.Tmax = server.arg(i).toFloat(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "tmin") {
         MystdConfig.Tmin = server.arg(i).toFloat(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "userdescriptions") {
         MystdConfig.UserDescriptions = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "mqtt_wifi_password") {
         MystdConfig.NETWORK_PASSWORD = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "mqtt_mesh_password") {
         MystdConfig.MESH_PASSWORD = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "mqtt_server") {
         MystdConfig.MQTT_SERVER = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "mqtt_server_tcp_port") {
         MystdConfig.MQTT_SERVER_TCP_PORT = server.arg(i).toInt(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "mqtt_topic_in") {
         MystdConfig.MQTT_TOPIC_IN = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "mqtt_topic_out") {
         MystdConfig.MQTT_TOPIC_OUT = server.arg(i).c_str(); // do something here with value from server.arg(i);
      }
      if (server.argName(i) == "as_mesh_secure") {
         MystdConfig.MESH_SECURE = server.arg(i).toInt(); // do something here with value from server.arg(i);

      }
      if (server.argName(i) == "as_mqtt_secure") {
         MystdConfig.MQTT_SECURE = server.arg(i).toInt(); // do something here with value from server.arg(i);
      }
    }
  }
  //MAIN CONFIGURATION
  if (HOSTNAME_TYPE == UID_HOSTNAME){
    HOSTNAME = MystdConfig.HostNameSuffix + WIFI_CHIP_ID;
  }else if (HOSTNAME_TYPE == USER_HOSTNAME){
    HOSTNAME = MystdConfig.UserHostName;
  }

  //save configuration in conf.json file
  if(saveConfig()){
    String redirect = "/redirect.html?param1=http://" + HOSTNAME;
    server.sendHeader("Location", redirect, true); //Redirect to our html web page 
    server.send(302, "text/plane","");
    tasker.setTimeout(restartESP, 6000);    
  };
}

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println(F("Failed to open config file"));
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println(F("Config file size is too large"));
    // Close the file
    configFile.close();
    return false;
  }

  const size_t capacity = JSON_OBJECT_SIZE(128) + 890;
  DynamicJsonDocument doc(capacity);

  //auto error = deserializeJson(doc, buf.get());
  auto error = deserializeJson(doc, configFile);
  if (error) {
    Serial.println(F("Failed to parse config file"));
    // Close the file
    configFile.close();
    return false;
  }

  MystdConfig.HostNameSuffix = doc["HOSTNAME_SUFFIX"].as<String>();       // "STD-WIFI-"
  MystdConfig.UserHostName = doc["USER_HOSTNAME"].as<String>();           // "esp8266-webupdate"
  MystdConfig.ChipID = doc["CHIP_ID"].as<String>();                       // 
  MystdConfig.WIFI_AP_PASSWORD = doc["WIFI_AP_PASSWORD"].as<String>();    // "abcd12345"

  MystdConfig.FTP_USERNAME = doc["FTP_USERNAME"].as<String>();            // "esp8266"
  MystdConfig.FTP_PASSWORD = doc["FTP_PASSWORD"].as<String>();            // "esp8266"
  MystdConfig.SAMPLE = doc["SAMPLE"]; // 10
  MystdConfig.Tmax = doc["TMAX"].as<String>();                            // "35.0"
  MystdConfig.Tmin = doc["TMIN"].as<String>();                            // "10.0"
  MystdConfig.UserDescriptions = doc["USERDESCRIPTIONS"].as<String>();    // "sonda silos 1 - grano"

  MystdConfig.NETWORK_PASSWORD = doc["MQTT_WIFI_PASSWORD"].as<String>();  // "network password"
  MystdConfig.MESH_PASSWORD = doc["MQTT_MESH_PASSWORD"].as<String>();     // "esp8266_sensor_mesh"
  MystdConfig.MQTT_SERVER = doc["MQTT_SERVER"].as<String>();              // "192.168.137.1"
  MystdConfig.MQTT_SERVER_TCP_PORT = doc["MQTT_SERVER_TCP_PORT"];         // 1883
  MystdConfig.MQTT_TOPIC_IN = doc["MQTT_TOPIC_IN"].as<String>();          // "STP-WIFI-in/"
  MystdConfig.MQTT_TOPIC_OUT = doc["MQTT_TOPIC_OUT"].as<String>();        // "STP-WIFI-out/"
  MystdConfig.MESH_SECURE = doc["AS_MESH_SECURE"].as<boolean>();          // true
  MystdConfig.MQTT_SECURE = doc["AS_MQTT_SECURE"].as<boolean>();          // false

  // Close the file
  configFile.close();

return true;
}

bool saveConfig() {
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }
  MystdConfig.ChipID = WIFI_CHIP_ID;
  const size_t capacity = JSON_OBJECT_SIZE(128);
  DynamicJsonDocument doc(capacity);
  
  doc["NODE_WIFI_SETTINGS"] =  MystdConfig.node_wifi_settings;      //readonly
  doc["NODE_WIFI_TYPE"] =  MystdConfig.hostname_type;               //readonly
  doc["CHIP_ID"] = MystdConfig.ChipID;                              //readonly
  doc["HOSTNAME_SUFFIX"] = MystdConfig.HostNameSuffix;
  doc["USER_HOSTNAME"] = MystdConfig.UserHostName;
  doc["HOSTNAME"] = HOSTNAME;
  doc["WIFI_AP_PASSWORD"] = MystdConfig.WIFI_AP_PASSWORD;

  doc["FTP_SERVER"] = "ftp://" + HOSTNAME + "/";                    //derived
  doc["FTP_SERVER_TCP_PORT"] = "21";                                //readonly
  doc["FTP_USERNAME"] = MystdConfig.FTP_USERNAME;
  doc["FTP_PASSWORD"] = MystdConfig.FTP_PASSWORD;

  doc["SAMPLE"] = MystdConfig.SAMPLE;
  doc["TMAX"] = MystdConfig.Tmax;                                   //value TAH >> red color
  doc["TMIN"] = MystdConfig.Tmin;                                   //value value TAL >> blue color
  doc["USERDESCRIPTIONS"] = MystdConfig.UserDescriptions;           //value descriptionr

  doc["DEFAULT_IP_ADDRESS"] = DEFAULT_IP_ADDRESS;                   //readonly
  doc["URI"] = "http://" + HOSTNAME + "/";                          //derived
  doc["URI_OTA"] = "http://" + HOSTNAME + "/update/";               //derived
  doc["URI_SETTINGS"] = "http://" + HOSTNAME + "/setings/";         //derived
  doc["URI_READCONFIG"] = "http://" + HOSTNAME + "/readconfig/";    //derived

  doc["MQTT_WIFI_PASSWORD"] = MystdConfig.NETWORK_PASSWORD;
  doc["MQTT_MESH_PASSWORD"] = MystdConfig.MESH_PASSWORD;
  doc["MQTT_SERVER"] = MystdConfig.MQTT_SERVER;
  doc["MQTT_SERVER_TCP_PORT"] = MystdConfig.MQTT_SERVER_TCP_PORT;
  doc["MQTT_TOPIC_IN"] = MystdConfig.MQTT_TOPIC_IN;
  doc["MQTT_TOPIC_OUT"] = MystdConfig.MQTT_TOPIC_OUT;
  doc["AS_MESH_SECURE"] = MystdConfig.MESH_SECURE;
  doc["AS_MQTT_SECURE"] = MystdConfig.MQTT_SECURE;

  doc["MQTT_FIRMWARE_ID"] = MystdConfig.MQTT_FIRMWARE_ID;           //readonly
  doc["MQTT_FIRMWARE_VER"] = MystdConfig.MQTT_FIRMWARE_VER;         //readonly

  serializeJson(doc, configFile);

  // Close the file
  configFile.close();
  return true;
}

void restartESP()
{
    Serial.println(F("ESP Restart in 2sec!!"));
    delay(2000);
    ESP.restart();
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}