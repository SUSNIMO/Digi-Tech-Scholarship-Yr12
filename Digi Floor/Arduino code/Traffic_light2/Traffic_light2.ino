#include <WiFi.h>
#include <esp_now.h>
#include <algorithm>
#include <string>
#include <iostream>

std::string buffer;
int order = 0;

std::string main_message = "";

//Lights pins
int led_up = 26;
int led_down = 27;

//Lights state
bool up_ledState = false;
bool down_ledState = false;

//light time
unsigned long start_time = 0;
unsigned long u_order = 0;
unsigned long l_order = 0;
int send = 0;

bool compute = true;
int sensor1 = 0;
int sensor2 = 0;

void compute()
{
  order = sensor1 + sensor2;
}

void update()
{
  if ((millis() - send) > 100)
  {
    if (order == 0)
    {
      broadcast("02-03-00-00-00");
      Serial.print("OFF");
    }
    else
    {
      if (up_ledState)
      {
        broadcast("02-03-00-11-10");
        Serial.print("Up!");
      }
      else
      {
        broadcast("02-03-00-01-01");
        Serial.print("Down!");
      }
    }

    send = millis();
  }
}

//Time checker for the lights
void time_check() {
  if ((millis() - start_time) > u_order || (millis() - start_time) > (u_order * -1))
  {
    up_ledState = !up_ledState;
    down_ledState = !down_ledState;
    start_time = millis();
  }
}

bool int_check(int number)
{
  if (number > 0)
  {
    return 1 > 0;
  }
  if (number < 0)
  {
    return 1 < 0;
  }
}

//For how long the light should be on for Up
void light_up()
{
  u_order = order * 5000;
  up_ledState = int_check(u_order);
}

//For how long the light should be on for Down
void light_down()
{
  l_order = order * -5000;
  down_ledState = int_check(l_order);
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

void assign_compute()
{
  if (Find(main_message, "01", 3, 2))
  {
    if (Find(main_message, "01", 6, 2))
    {
      if (Find(main_message, "11", 9, 2))
      {
        sensor1 = 1;
      }
      if (Find(main_message, "00", 9, 2))
      {
        sensor1 = 0;
      }
    }
    if (Find(main_message, "11", 6, 2))
    {
      if (Find(main_message, "01", 9, 2))
      {
        sensor2 = -1;
      }
      if (Find(main_message, "00", 9, 2))
      {
        sensor2 = 0;
      }
    }
  }
}

void assign_order()
{
  if (Find(main_message, "04", 6, 2))
  {
    if (Find(main_message, "1", 9, 1))
    {
      if (Find(main_message, "1", 10, 1))
      {
        order = 1;
      }
      if (Find(main_message, "0", 10, 1))
      {
        order = 0;
      }
      if (Find(main_message, "2", 10, 1))
      {
        order = 2;
      }
    }
    if (Find(main_message, "0", 9, 1))
    {
      if (Find(main_message, "1", 10, 1))
      {
        order = -1;
      }
      if (Find(main_message, "0", 10, 1))
      {
        order = 0;
      }
      if (Find(main_message, "2", 10, 1))
      {
        order = -2;
      }
    }
  }

  if (Find(main_message, "03", 6, 2))
  {
    if (Find(main_message, "11", 9, 2))
    {
      compute = true;
    }
    if (Find(main_message, "00", 9, 2))
    {
      compute = false;
    }
  }
}

  Serial.println(order);
  Seril.println(compute);

  light_down();
  light_up();

  start_time = millis();
}

void message_verification(std::string message) //in a scenario if anyone tried to infiltrate and tamper with the system
{
  //message example: 01-01-02-00-10
  //floor(01, 02...)-type(01- sensors, 02- time, 03-lights)-arrangment(01- down, 02- up)-message(x1= x1)-class of data(0- for negative result, 1- for positive results)
  //the last digit in the number is just a dummy
  //[0 in the left side if message would indicate negative numbers(0x= -x, 1x= x)]
  if (Find(message, "0", 12, 1) == Find(message, "0", 9, 1) || Find(message, "1", 12, 1) == Find(message, "1", 9, 1))
  {
    if (Find(message, "02", 0, 2)) {
      if (Find(message, "04", 3, 2))
      {
        main_message = message;
        Serial.println("Data Verified for Command");
        assign_order();
      }
      if (Find(message, "01", 3, 2))
      {
        main_message = message;
        Serial.println("Data Verified for Compute");
        assign_compute();
      }
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
  Serial.println(buffer);
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
  pinMode(led_up, OUTPUT);
  pinMode(led_down, OUTPUT);

  digitalWrite(led_down, down_ledState);
  digitalWrite(led_up, up_ledState);
}

void loop()
{
  if (compute)
  {
    time_check();
    if (order == 0) {
      digitalWrite(led_down, LOW);
      digitalWrite(led_up, LOW);
    }
    else {
      digitalWrite(led_down, down_ledState);
      digitalWrite(led_up, up_ledState);
    }
    update();
  }
  }
  else
  {
    time_check();
    if (order == 0) {
      digitalWrite(led_down, LOW);
      digitalWrite(led_up, LOW);
    }
    else {
      digitalWrite(led_down, down_ledState);
      digitalWrite(led_up, up_ledState);
    }
    update();
  }
}