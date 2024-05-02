#include <WiFi.h>
#include <esp_now.h>
#include <algorithm>
#include <string>
#include <iostream>

const int trigPin = 5;
const int echoPin = 18;
const int led = 19;
const int led1 = 2;
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceInch;
int ticker;
int sec;
// Variables for test data
int distanceCm;
int distance1;
int distance2;
int distance3;

int ID_4_1;
int ID_4_0;
bool computer;
int ID_1;
int ID_4_i0;
int ID_5_0;
int ID_2_4;
int ID_3_2;
int REPORT;
bool wait_for_other = false;
std::string buffer;
int order = 0;
bool data_verified = false;
std::string main_message = "";

void Find(std::string text, int& ID, std::string search, int start, int length)
{
  ID = text.find(search.c_str(), start, length);
  if (ID != std::string::npos) {
        ID = 1; 
    } else {
        ID = 0; 
    }
}

void operator_command()
{
  //exp: 01-04-01-01-01
  Find(main_message, ID_4_1, "01", 9, 2);
  Find(main_message, ID_4_0, "00", 9, 2);
  if (ID_4_0)
  {
    computer = false;
  }
  if (ID_4_1)
  {
    computer = true;
  }
}

void report()
{
  //message example: 01-01-02-00-10
  //floor(01, 02...)-type(01- sensors, 02- time, 03-lights)-arrangment(01- down, 02- up)-message(x1= x1)-class of data(0- for negative result, 1- for positive results)
  //the last digit in the number is just a dummy
  //[0 in the left side if message would indicate negative numbers(0x= -x, 1x= x)]
  if (REPORT == 1)
  {
    broadcast("01-01-01-01-01");
    Serial.println("01-01-01-01-01");
    Serial.print('\n');
  }
  else 
  {
    broadcast("01-01-01-00-01");
    Serial.println("01-01-01-00-01");
    Serial.print('\n');
  }
}

void sensor(int& led_status)
{
  digitalWrite(led1, HIGH);
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  if (distanceCm <= 60 ){
    digitalWrite(led, HIGH);
    led_status = 1;
  }
  else {
    digitalWrite(led, LOW);
    led_status = 0;
  }
  Serial.println(distanceCm);
  Serial.print('\n');
}

void operations()
{
  sensor(distance1);
  delay(1000);
  sensor(distance2);
  delay(1000);
  sensor(distance3);
  delay(1000);
  order = (distance1 + distance2 + distance3);
  if (order > 2)
  {
    REPORT = 1;
  }
  else
  {
    REPORT = 0;
  }
}

void message_verification(std::string message) //in a scenario if anyone tried to infiltrate and tamper with the system
{
  //message example: 01-01-02-00-10
  //floor(01, 02...)-type(01- sensors, 02- time, 03-lights)-arrangment(01- down, 02- up)-message(x1= x1)-class of data(0- for negative result, 1- for positive results)
  //the last digit in the number is just a dummy
  //[0 in the left side if message would indicate negative numbers(0x= -x, 1x= x)]
  Find(message, ID_1, "01", 0, 2);
  Find(message, ID_4_i0, "0", 9, 1);
  Find(message, ID_5_0, "0", 12, 1);
  bool class_of_data = false;
  if (ID_5_0 == ID_4_i0)
  {
    if (ID_1) {
      main_message = message;
      Serial.println("Data Verified");
      Find(main_message, ID_2_4, "04", 3, 2);
      Find(main_message, ID_3_2, "02", 6, 2);
      Serial.print('\n');
      if (ID_2_4)
      {
        if (ID_3_2)
        {
          operator_command();
        }
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
  message_verification(buffer);
}

// callback when data is sent
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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
  //pinMode
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
}

void loop()
{
  if (computer)
  {
    operations();
    if (wait_for_other)
    {
      delay(500);
    }
    report();
    wait_for_other = false;
  }
}