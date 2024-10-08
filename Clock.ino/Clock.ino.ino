#include <WiFi.h>
#include <esp_now.h>
#include <algorithm>
#include <string>
#include <iostream>

std::string main_message = "";
int order = 0;

int seconds = 0;
int minutes = 0;
int start_time = 0;
int start_broadcast = 0;
bool operations = true;
bool time_up_permission = false;
bool time_down_permission = false;
std::string TIME = "";
String TIME1;
int order_time = 0;

void update_status()
{
  if ((millis() - start_broadcast) > 100)
  {
    if (order_time == 0)
    {
      broadcast("04-02-00-00-02-0404");
      broadcast("04-02-00-00-02-0103");
    }
    if (order_time < 0)
    {
      broadcast("04-02-00-01-02-0404");
      broadcast("04-02-00-01-02-0103");
    }
    if (order_time > 0)
    {
      broadcast("04-02-00-11-12-0404");
      broadcast("04-02-00-11-12-0103");
    }
    start_broadcast = millis();
  }
}

void main_time()
{
  if ((millis() - start_time) > 1000)
  {
    seconds++;
    start_time = millis();
  }

  if (seconds > 59)
  {
    seconds = 0;
    minutes = minutes + 1;
  }
  if (minutes == 0 && seconds > -1)
  {
    order_time = -1;
  }
  if (minutes == 2 && seconds > 29)
  {
    order_time = 1;
  }
  if (minutes == 5 && seconds > 1)
  {
    reset_time();
  }

  TIME = std::to_string(minutes) + ":" + std::to_string(seconds);
  TIME1 = TIME.c_str();

}

void reset_time()
{
  seconds = 0;
  minutes = 0;
  time_up_permission = false;
  time_down_permission = false;
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

void order_assign() 
{
  if (Find(main_message, "1", 10, 1))
  {
    order = 1;
  }
  if (Find(main_message, "0", 10, 1))
  {
    order = 0;
    order_time = 0;
  }
  start_time = millis();
}

void message_verification(std::string message) 
{
  if ((Find(message, "1", 12, 1) && Find(message, "1", 9, 1)) || (Find(message, "0", 12, 1) && Find(message, "0", 9, 1)))
  {
    if (Find(message, "0402", 15, 4))
    {
      main_message = message;
      order_assign();
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
  //Serial.printf("Received message from: %s - %s\n", macStr, buffer);
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

  start_broadcast = millis();
}

void loop()
{
  if (order == 1)
  {
    main_time();
    Serial.println(TIME1);
  }
  else 
  {
    reset_time();
    Serial.println("No time!");
  }

  update_status();
}

//it works now *phew