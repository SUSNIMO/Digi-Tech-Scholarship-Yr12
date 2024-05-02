#include <WiFi.h>
#include <esp_now.h>
#include <algorithm>
#include <string>
#include <iostream>

std::string buffer;
int order = 0;
std::string converted_order = "";
std::string M_order = "";
std::string M1_order = "";
String Main_order = "";
int s_message1 = 0;
int s_message2 = 0;
int t_message = 0;
bool operations = true;
bool new_message = false;
bool computer = false;
std::string main_message = "";
int m_4_1;
int m_4_0;
int ID_1;
int ID_2_1;
int ID_2_2;
int ID_2_3;
int ID_2_4;
int ID_3_1;
int ID_3_2;
int ID_4_1;
int ID_4_0;
int ID_4_i1;
int ID_4_i0;
int ID_5_1;
int ID_5_0;

void Find(std::string text, int& ID, std::string search, int start, int length)
{
  ID = text.find(search.c_str(), start, length);
  if (ID != std::string::npos) {
        ID = 1; 
    } else {
        ID = 0; 
    }
}

void calculate()
{
  order = s_message1 + s_message2 + t_message;
  converted_order = std::to_string(order);
  if (order > 0)
  {
    M_order = "1" + converted_order;
  }
  if (order < 0)
  {
    order = order * (-1);
    M_order = "0" + converted_order;
  }
  M1_order = "01-04-04-" + M_order + "-04";
  Main_order = M1_order.c_str();
  broadcast(Main_order);
  operations = true;
}

void sensor_data_assign()
{
  Find(main_message, ID_3_1, "01", 6, 2);
  Find(main_message, ID_3_2, "02", 6, 2);
  Find(main_message, ID_4_1, "1", 10, 1);
  Find(main_message, ID_4_0, "0", 10, 1);

  if (ID_3_1)
  {
    if (ID_4_1)
    {
      s_message1 = 1;
    }
    if (ID_4_0)
    {
      s_message1 = 0;
    }
  }
  if (ID_3_2) 
  {
    if (ID_4_1)
    {
      s_message2 = -1;
    }
    if (ID_4_0)
    {
      s_message2 = 0;
    }
  }
}

void time_data_assign()
{
  Find(main_message, ID_4_i1, "1", 9, 1);
  Find(main_message, ID_4_i0, "0", 9, 1);
  Find(main_message, ID_4_1, "1", 10, 1);
  if (ID_4_i1)
  {
    if (ID_4_1)
    {
      t_message = 1;
    }
  }
  if (ID_4_i0)
  {
    if (ID_4_1)
    {
      t_message = -1;
    }
  }
}

void operator_command()
{
  M1_order = main_message;
  Main_order = M1_order.c_str();
  broadcast(Main_order);
  //exp: 01-04-01-01-01
  Find(main_message, ID_3_1, "01", 6, 2);
  Find(main_message, ID_4_1, "01", 9, 2);
  Find(main_message, ID_4_0, "00", 9, 2);
  if (ID_3_1)
  {
    if (ID_4_0)
    {
      computer = false;
    }
    if (ID_4_1)
    {
      computer = true;
    }
  }
}

void main_data_assign()
{

  Find(main_message, ID_2_1, "01", 3, 2);
  Find(main_message, ID_2_2, "02", 3, 2);
  Find(main_message, ID_2_3, "03", 3, 2);
  Find(main_message, ID_2_4, "04", 3, 2);
  if (ID_2_1) 
  {
    sensor_data_assign();
  }
  if (ID_2_2)
  {
    time_data_assign();
  }
  if (ID_2_4);
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
  Find(message, ID_1, "01", 0, 2);
  Find(message, ID_4_i1, "1", 9, 1);
  Find(message, ID_4_i0, "0", 9, 1);
  Find(message, ID_5_1, "1", 12, 1);
  Find(message, ID_5_0, "0", 12, 1);
  bool class_of_data = false;
  if (ID_5_0 == ID_4_i0 || ID_5_1 == ID_4_i1)
  {
    if (ID_1) {
      new_message = true;
      main_message = buffer;
      Serial.println("Data Verified");
      Serial.print('\n');
    }
  }
}

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

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
  Serial.printf("Received message from: %s - %s\n", macStr, buffer);
  // what are our instructions
  message_verification(buffer);
}

// callback when data is sent
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  //Serial.print("Last Packet Sent to: ");
  //Serial.println(macStr);
  //Serial.print("Last Packet Send Status: ");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  //Serial.print('\n');
}

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
    //Serial.println("ESPNOW not Init.");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    //Serial.println("Invalid Argument");
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    //Serial.println("Internal Error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    //Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    //Serial.println("Peer not found.");
  }
  else
  {
    //Serial.println("Unknown error");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
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
  }
  // use the built in button
  pinMode(0, INPUT_PULLUP);
  pinMode(2, OUTPUT);
}

void loop()
{
  if (new_message)
  {
    if (computer)
    {
      new_message = false;
      if (operations)
      {
        operations = false;
        main_message = buffer;
        main_data_assign();
        calculate();
      }
    }
  }
}