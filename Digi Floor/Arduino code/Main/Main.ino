#include <WiFi.h>
#include <esp_now.h>
#include <WebServer.h>  
#include "SuperMon.h"   // .h file that stores your html page code

//#define USE_INTRANET //comment this out if you want it to turn into a AP

// "Login" for esp32 to connect to a wifi
#define LOCAL_SSID "ZION"
#define LOCAL_PASS "largeocean922"

// "Login" ofr devices to connect to the Esp32
#define AP_SSID "System Command"
#define AP_PASS "password123"

std::string f1_data = "None";
std::string f2_data = "None";
std::string f3_data = "None";
std::string T_data = "None";

bool new_message = false;
std::string main_message = "";
std::string M1_order = "";
String Main_order = "";
bool computer = false;
bool operations = true;
int order = 0;
int s_message1 = 0;
int s_message2 = 0;
int t_message = 0;
std::string converted_order = "";
std::string M_order = "";

char XML[2048];
char buf[32];

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress Actual_IP;

// definitions of your desired intranet created by the ESP32
IPAddress PageIP(192, 168, 1, 1); //IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

// gotta create a server
WebServer server(80);

void calculate()
{
  order = s_message1 + s_message2 + t_message;
  converted_order = std::to_string(order);
  if (order > 0)
  {
    M_order = "1" + converted_order;
    M1_order = "01-04-04-" + M_order + "-14";
  }
  if (order < 0)
  {
    order = order * (-1);
    M_order = "0" + converted_order;
    M1_order = "01-04-04-" + M_order + "-04";
  }
  Main_order = M1_order.c_str();
  broadcast(Main_order);
  operations = true;
}

void compute() 
{
  if (new_message)
  {
    if (computer)
    {
      new_message = false;
      if (operations)
      {
        operations = false;
        main_data_assign();
        calculate();
      }
    }
  }
}

bool Find(const std::string& text, const std::string& search, int start, int length) {
    if (start >= text.length() || length <= 0) {
        return false; // Invalid parameters
    }
    // Ensure length does not exceed the remaining text length
    length = std::min(length, static_cast<int>(text.length()) - start);
    
    size_t found = text.find(search, start);
    if (found != std::string::npos && found + search.length() <= start + length) {
        //Serial.print("Found '"); Serial.print(search.c_str()); Serial.print("' at position "); Serial.println(found);
        return true;
    } else {
        //Serial.print("Did not find '"); Serial.print(search.c_str()); Serial.print("' within range starting at "); Serial.println(start);
        return false;
    }
}


void sensor_data_assign()
{
  if (Find(main_message, "01", 6, 2))
  {
    if (Find(main_message, "1", 10, 1))
    {
      s_message1 = 1;
    }
    if (Find(main_message, "0", 10, 1))
    {
      s_message1 = 0;
    }
  }
  if (Find(main_message, "02", 6, 2)) 
  {
    if (Find(main_message, "1", 10, 1))
    {
      s_message2 = -1;
    }
    if (Find(main_message, "0", 10, 1))
    {
      s_message2 = 0;
    }
  }
}

void variable_assign(std::string &status, const std::string &message)
{
  if (Find(message, "11", 9, 2))
  {
    status = "Up";
  }
  if (Find(message, "01", 9, 2))
  {
    status = "Down";
  }
  if (Find(message, "00", 9, 2))
  {
    status = "None";
  }
}

void time_data_assign()
{
  if (Find(main_message, "1", 9, 1))
  {
    if (Find(main_message, "1", 10, 1))
    {
      t_message = 1;
    }
  }
  if (Find(main_message, "0", 9, 1))
  {
    if (Find(main_message, "1", 10, 1))
    {
      t_message = -1;
    }
  }

  variable_assign(T_data, main_message);
}

void light_data_assign()
{
  //0x-03-00-xx-x0
  if (Find(main_message, "01", 0, 2))
  {
    Serial.println("F1");
    variable_assign(f1_data, main_message);
  }
  if (Find(main_message, "02", 0, 2))
  {
    Serial.println("F2");
    variable_assign(f2_data, main_message);
  }
  if (Find(main_message, "03", 0, 2))
  {
    Serial.println("F3");
    variable_assign(f3_data, main_message);
  }
}

void operator_command()
{
  M1_order = main_message;
  Main_order = M1_order.c_str();
  broadcast(Main_order);
  //exp: 01-04-01-01-01
  if (Find(main_message, "01", 6, 2))
  {
    if (Find(main_message, "00", 9, 2))
    {
      computer = false;
    }
    if (Find(main_message, "01", 9, 2))
    {
      computer = true;
    }
  }
}

void main_data_assign()
{
  Serial.println("Main data assign");
  if (Find(main_message, "02", 3, 2))
  {
    Serial.println("2");
    time_data_assign();
  }
  if (Find(main_message, "01", 3, 2)) 
  {
    Serial.println("1");
    sensor_data_assign();
  }
  if (Find(main_message, "03", 3, 2))
  {
    Serial.println("3");
    light_data_assign();
  }
  else if (Find(main_message, "04", 3, 2))
  {
    operator_command();
  }
}

void message_verification(std::string message) //in a scenario if anyone tried to infiltrate and tamper with the system
{
  //message example: 01-01-02-00-10
  //floor(01, 02...)-type(01- sensors, 02- time, 03-lights)-arrangment(01- down, 02- up)-message(x1= x1)-class of data(0- for negative result, 1- for positive results)
  //the last digit in the number is just a dummy
  //[0 in the left side if message would indicate negative numbers(0x= -x, 1x= x)]
  bool class_of_data = false; 
  if ((Find(message, "1", 12, 1) && Find(message, "1", 9, 1)) || (Find(message, "0", 12, 1) && Find(message, "0", 9, 1)))
  {  
    new_message = true;
    main_message = message;
    Serial.println("Data Verified");
    main_data_assign();
  }
  else 
  {
    Serial.println("Not Verified");
  }
}

//Formatting Mac Address
void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

//Function for when a message is recieved from the broadcast channel
void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen)
{
  // only allow a maximum of 250 characters in the message + a null terminating byte
  char buffer[ESP_NOW_MAX_DATA_LEN + 1];
  int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);  
  strncpy(buffer, (const char *)data, msgLen);
  // make sure we are null terminated
  buffer[msgLen] = 0;
  // format the mac address
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  // debug log the message to the serial port
  //Serial.printf("Received message from: %s - %s\n", macStr, buffer);
  // what are our instructions
  Serial.println(buffer);

  //Verify data
  message_verification(buffer);
}

// callback when data is sent to check if data has been recieved
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//function to when you send message to the broadcast channel
void broadcast(const String &message)
{
  // this will broadcast a message to everyone in range
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress))
  {
    esp_now_add_peer(&peerInfo);
  }
  esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t *)message.c_str(), message.length());
  // and this will send a message to a specific device
  /*uint8_t peerAddress[] = {0x3C, 0x71, 0xBF, 0x47, 0xA5, 0xC0};
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, peerAddress, 6);
  if (!esp_now_is_peer_exist(peerAddress))
  {
    esp_now_add_peer(&peerInfo);
  }
  esp_err_t result = esp_now_send(peerAddress, (const uint8_t *)message.c_str(), message.length());*/
  if (result == ESP_OK)
  {
    Serial.println("Broadcast message success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)
  {
    Serial.println("ESPNOW not Init.");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    Serial.println("Invalid Argument");
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    Serial.println("Internal Error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    Serial.println("Peer not found.");
  }
  else
  {
    Serial.println("Unknown error");
  }
}

void setup() {
  //Serial initialization
  Serial.begin(115200);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);

  //print Startup data
  Serial.println("ESPNow Example");
  // Output my MAC address - useful for later
  Serial.print("My MAC Address is: ");
  Serial.println(WiFi.macAddress());

  // shut down wifi
  WiFi.disconnect();

  // startup ESP Now
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    delay(3000);
    ESP.restart();

  // if you have this #define USE_INTRANET,  you will connect to your home intranet, again makes debugging easier
#ifdef USE_INTRANET
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  Actual_IP = WiFi.localIP();
#endif

  // if you don't have #define USE_INTRANET, here's where you will creat and access point
  // an intranet with no internet connection. But Clients can connect to your intranet and see
  // the web page you are about to serve up
#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("IP address: "); Serial.println(Actual_IP);
#endif

  printWifiStatus();


  }

  // if your web page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  disableCore0WDT();

  // maybe disable watch dog timer 1 if needed
  //  disableCore1WDT();

  // just an update to progress

  Serial.println("starting server");

  // if you have this #define USE_INTRANET,  you will connect to your home intranet, again makes debugging easier
#ifdef USE_INTRANET
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  Actual_IP = WiFi.localIP();
#endif

  // if you don't have #define USE_INTRANET, here's where you will creat and access point
  // an intranet with no internet connection. But Clients can connect to your intranet and see
  // the web page you are about to serve up
#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("IP address: "); Serial.println(Actual_IP);
#endif

  printWifiStatus();


  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);

  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page
  server.on("/xml", SendXML);

  server.on("/Command1", Command1);
  server.on("/Command2", Command2);
  server.on("/Command3", Command3);
  server.on("/Timer", Timer);
  // finally begin the server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

  compute();
}

// I think I got this code from the wifi example
void printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:a
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Open http://");
  Serial.println(ip);
}


// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {

  Serial.println("sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);

}

void Timer() {
  // Get the value of the "VALUE" parameter from the request
  String Cdata = server.arg("VALUE");

  std::string Command;

  if (Find(Cdata.c_str(), "0", 0, 1)) 
  {
    Command = "01-04-02-" + std::string(Cdata.c_str()) + "-04";
  }
  else if (Find(Cdata.c_str(), "1", 0, 1)) 
  {
    Command = "01-04-02-" + std::string(Cdata.c_str()) + "-14";
  }

  // Broadcast the Command value
  broadcast(String(Command.c_str()));

  // Send an empty response to the client
  server.send(200, "text/plain", "");
}


void Command1() {
  // Get the value of the "VALUE" parameter from the request
  String Cdata = server.arg("VALUE");

  std::string Command;

  if (Find(Cdata.c_str(), "0", 0, 1)) 
  {
    Command = "01-04-04-" + std::string(Cdata.c_str()) + "-04";
  }
  else if (Find(Cdata.c_str(), "1", 0, 1)) 
  {
    Command = "01-04-04-" + std::string(Cdata.c_str()) + "-14";
  }

  // Broadcast the Command value
  broadcast(String(Command.c_str()));

  // Send an empty response to the client
  server.send(200, "text/plain", "");
}

void Command2() {
  // Get the value of the "VALUE" parameter from the request
  String Cdata = server.arg("VALUE");

  std::string Command;

  if (Find(Cdata.c_str(), "0", 0, 1)) 
  {
    Command = "02-04-04-" + std::string(Cdata.c_str()) + "-04";
  }
  else if (Find(Cdata.c_str(), "1", 0, 1)) 
  {
    Command = "02-04-04-" + std::string(Cdata.c_str()) + "-14";
  }

  // Broadcast the Command value
  broadcast(String(Command.c_str()));

  // Send an empty response to the client
  server.send(200, "text/plain", "");
}

void Command3() {
  // Get the value of the "VALUE" parameter from the request
  String Cdata = server.arg("VALUE");

  std::string Command;

  if (Find(Cdata.c_str(), "0", 0, 1)) 
  {
    Command = "03-04-04-" + std::string(Cdata.c_str()) + "-04";
  }
  else if (Find(Cdata.c_str(), "1", 0, 1)) 
  {
    Command = "03-04-04-" + std::string(Cdata.c_str()) + "-14";
  }

  // Broadcast the Command value
  broadcast(String(Command.c_str()));

  // Send an empty response to the client
  server.send(200, "text/plain", "");
}

// code to send the main web page
void SendXML() {

  // Serial.println("sending xml");

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  sprintf(buf, "<f1>%s</f1>\n", f1_data.c_str());
  strcat(XML, buf);

  sprintf(buf, "<f2>%s</f2>\n", f2_data.c_str());
  strcat(XML, buf);

  sprintf(buf, "<f3>%s</f3>\n", f3_data.c_str());
  strcat(XML, buf);

  sprintf(buf, "<fT>%s</fT>\n", T_data.c_str());
  strcat(XML, buf);

  strcat(XML, "</Data>\n");
  
  //print the result above
  Serial.println(XML);

  server.send(200, "text/xml", XML);
}


// end of code