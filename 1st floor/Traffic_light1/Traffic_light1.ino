#include <WiFi.h>
#include <esp_now.h>
#include <algorithm>
#include <string>
#include <iostream>

std::string buffer;
int order = 0;
int Order = 0;
int i;

std::string main_message = "";

//Lights pins
int led_up = 19;
int led_down = 18;  

//Lights state
bool up_ledState = false;
bool down_ledState = false;

//light time
unsigned long start_time = 0;
unsigned long u_order = 0;
unsigned long l_order = 0;
int send = 0;
bool direction = false;

bool compute = true;
bool DC = false;
int sensor1 = 0;
int sensor2 = 0;
int oras = 0;

void Compute()
{
  order = sensor1 + sensor2 + oras;
}

void update()
{
  if ((millis() - send) > 100)
  {
    if (compute || DC)
    {
      if (up_ledState)
      {
        broadcast("01-03-00-11-10-0404");
        broadcast("01-03-00-11-10-0203");
      }
      else 
      {
        broadcast("01-03-00-01-00-0404");
        broadcast("01-03-00-01-00-0203");
      }
    }
    else
    {
      broadcast("01-03-00-00-00-0404");
      broadcast("01-03-00-00-00-0203");
    }
    send = millis();
  }
}

// Time checker for the lights
void time_check() 
{
  if (direction) // Up light is supposed to be on
  {
    if ((millis() - start_time) > u_order) // Check if the time for Up light has passed
    {
      // Turn off Up light and turn on Down light
      up_ledState = false; 
      down_ledState = true;
      start_time = millis(); // Reset timer
      direction = false; // Switch direction
      i++;
    }
    else
    {
      // Ensure Up light is on and Down light is off during this phase
      up_ledState = true;
      down_ledState = false;
    }
  }
  else // Down light is supposed to be on
  {
    if ((millis() - start_time) > (l_order)) // Check if the time for Down light has passed
    {
      // Turn off Down light and turn on Up light
      up_ledState = true; 
      down_ledState = false;
      start_time = millis(); // Reset timer
      direction = true; // Switch direction
      i++;
    }
    else
    {
      // Ensure Down light is on and Up light is off during this phase
      up_ledState = false;
      down_ledState = true;
    }
  }

  if (i == 2)
  {
    i = 0;
    Compute();
    if (compute)
    {
      light_up(order);
      light_down(order);
    }
  }
}

//For how long the light should be on for Up
void light_up(int time)
{
  if (time > 0)
  {
    u_order = (time + 1) * 5000;
  }
  if (time == 0 || time < 0)
  {
    u_order = 5000;
  }
}

//For how long the light should be on for Down
void light_down(int time)
{
  if (time > 0 || time == 0)
  {
    l_order = 5000;
  }
  if (time < 0)
  {
    l_order = ((time - 1) * 5000) * -1;
  }
}

bool Find(const std::string& text, const std::string& search, int start, int length) {
    if (start >= text.length() || length <= 0) 
    {
        return false; // Invalid parameters
    }
    // Ensure length does not exceed the remaining text length
    length = std::min(length, static_cast<int>(text.length()) - start);
    
    size_t found = text.find(search, start);
    if (found != std::string::npos && found + search.length() <= start + length) 
    {
        //Serial.print("Found '"); Serial.print(search.c_str()); Serial.print("' at position "); Serial.println(found);
        return true;
    } 
    else 
    {
        //Serial.print("Did not find '"); Serial.print(search.c_str()); Serial.print("' within range starting at "); Serial.println(start);
        return false;
    }
}

void assign_compute()
{
  //data from sensor on 1st floor
  if (Find(main_message, "01", 0, 2))
  {
    if (Find(main_message, "01", 3, 2))
    {
      if (Find(main_message, "01", 6, 2))
      {
        if (Find(main_message, "11", 9, 2))
        {
          sensor1 = -1;
        }
        if (Find(main_message, "00", 9, 2))
        {
          sensor1 = 0;
        }
      }
      if (Find(main_message, "02", 6, 2))
      {
        if (Find(main_message, "11", 9, 2))
        {
          sensor2 = 1;
        }
        if (Find(main_message, "00", 9, 2))
        {
          sensor2 = 0;
        }
      }
    }
  }

  //data from timer
  if (Find(main_message, "04", 0, 2))
  {
    if (Find(main_message, "02", 3, 2))
    {
      if (Find(main_message, "00", 9, 2))
      {
        oras = 0;
      }
      if (Find(main_message, "01", 9, 2))
      {
        oras = -1;
      }
      if (Find(main_message, "11", 9, 2))
      {
        oras = 1;
      }
      
    }
  }
}

void assign_order()
{
  //direct order from web
  if (Find(main_message, "04", 6, 2))
  {
    compute = false;
    if (Find(main_message, "1", 9, 1))
    {
      direction = true;
      if (Find(main_message, "1", 10, 1))
      {
        Order = 1;
        DC = true;
      }
      if (Find(main_message, "0", 10, 1))
      {
        Order = 0;
        DC = false;
      }
      if (Find(main_message, "2", 10, 1))
      {
        Order = 2;
        DC = true;
      }
      light_down(Order);
      light_up(Order);

      start_time = millis();
    }
    if (Find(main_message, "0", 9, 1))
    {
      direction = false;
      if (Find(main_message, "1", 10, 1))
      {
        Order = -1;
        DC = true;
      }
      if (Find(main_message, "0", 10, 1))
      {
        Order = 0;
        DC = false;
      }
      if (Find(main_message, "2", 10, 1))
      {
        Order = -2;
        DC = true;
      }
      light_down(Order);
      light_up(Order);

      start_time = millis();
    }
    Serial.println("DC");
  }
  //command to compute and to self operate 
  if (Find(main_message, "03", 6, 2))
  {
    if (Find(main_message, "11", 9, 2))
    {
      compute = true;
      Serial.println(order);
      Serial.println(compute);

      light_down(order);
      light_up(order);

      start_time = millis();
    }
    if (Find(main_message, "00", 9, 2))
    {
      compute = false;
    }
    Serial.println("COMP");
  }
}

void message_verification(std::string message) //in a scenario if anyone tried to infiltrate and tamper with the system
{
  if (Find(message, "0", 12, 1) == Find(message, "0", 9, 1) || Find(message, "1", 12, 1) == Find(message, "1", 9, 1))
  {
    if (Find(message, "0103", 15, 4))
    {
      main_message = message;
      if (Find(main_message, "04-04-04-", 0, 9) || Find(main_message, "04-04-03-", 0, 9))
      {
        assign_order();
      }
      if (Find(main_message, "01-01-", 0, 6) || Find(main_message, "04-02-00-", 0, 9))
      {
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
  //Serial.printf("Received message from: %s - %s\n", macStr, buffer);
  // what are our instructions
  //Serial.println(buffer);
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
    //Serial.println("Broadcast message success");
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

  u_order = 5000;
  start_time = millis();
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

    digitalWrite(led_down, down_ledState);
    digitalWrite(led_up, up_ledState);
  }
  else
  {
    time_check();
    if (Order == 0) {
      digitalWrite(led_down, LOW);
      digitalWrite(led_up, LOW);
    }
    else {
      digitalWrite(led_down, down_ledState);
      digitalWrite(led_up, up_ledState);
    }
  }
  update();

  Serial.print(Order);
  Serial.print("-");
  Serial.print(order);  
  Serial.print("=");
  Serial.print(compute);
  Serial.print(":");
  Serial.println(direction);
  
}