// Based on https://github.com/mysensors/MySensors/tree/master/examples/GatewayESP8266OTA

#include <ArduinoOTA.h>

// Enable debug prints to serial monitor
#define MY_DEBUG
#define MY_BAUD_RATE 74880 // To be able to see reset messages

#define WIFI_REPORT_INTERVAL 300000

// Enables and select radio type (if attached)
#define MY_RFM69_NEW_DRIVER
//#define MY_RX_MESSAGE_BUFFER_FEATURE
#define MY_RFM69_ATC_MODE_DISABLED
#define MY_RFM69_CSMA_LIMIT_DBM -75
#define MY_RADIO_RFM69
#define MY_IS_RFM69HW
#define MY_RF69_IRQ_PIN D2
#define MY_RF69_IRQ_NUM MY_RF69_IRQ_PIN
#define MY_RF69_SPI_CS D8
#define MY_RFM69_FREQUENCY RFM69_433MHZ

#define MY_DEBUG_VERBOSE_RFM69 // Temporary
#define MY_DEBUG_VERBOSE_GATEWAY

#define MY_GATEWAY_ESP8266

#define MY_WIFI_SSID "Mr-IoT.com"
#include "settings.h"

// Set the hostname for the WiFi Client. This is the hostname
// it will pass to the DHCP server if not static.
#define MY_HOSTNAME "RFM69-Gateway"

// Enable MY_IP_ADDRESS here if you want a static ip address (no DHCP)
//#define MY_IP_ADDRESS 192,168,178,87

// If using static ip you can define Gateway and Subnet address as well
//#define MY_IP_GATEWAY_ADDRESS 192,168,178,1
//#define MY_IP_SUBNET_ADDRESS 255,255,255,0

// The port to keep open on node server mode
#define MY_PORT 5003

// How many clients should be able to connect to this gateway (default 1)
#define MY_GATEWAY_MAX_CLIENTS 2

// Controller ip address. Enables client mode (default is "server" mode).
// Also enable this if MY_USE_UDP is used and you want sensor data sent somewhere.
//#define MY_CONTROLLER_IP_ADDRESS 192, 168, 178, 68

#if defined(MY_USE_UDP)
#include <WiFiUDP.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <MySensors.h>
#define CHILD_ID_WIFI_RSSI 0
MyMessage msgWifiRssi(CHILD_ID_WIFI_RSSI, V_LEVEL);
#define CHILD_ID_RSSI1 1
MyMessage msgRssi1(CHILD_ID_RSSI1, V_LEVEL);
bool sendRssi = false;
int16_t rssi;

void setup()
{
  // Setup locally attached sensors
  ArduinoOTA.onStart([]() {
    DEBUG_OUTPUT("ArduinoOTA start\n");
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_OUTPUT("\nArduinoOTA end\n");
  });
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);
  ArduinoOTA.setHostname(MY_HOSTNAME);
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_OUTPUT("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_OUTPUT("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      DEBUG_OUTPUT("Auth Failed\n");
    } else if (error == OTA_BEGIN_ERROR) {
      DEBUG_OUTPUT("Begin Failed\n");
    } else if (error == OTA_CONNECT_ERROR) {
      DEBUG_OUTPUT("Connect Failed\n");
    } else if (error == OTA_RECEIVE_ERROR) {
      DEBUG_OUTPUT("Receive Failed\n");
    } else if (error == OTA_END_ERROR) {
      DEBUG_OUTPUT("End Failed\n");
    }
  });
  ArduinoOTA.begin();
}

void presentation()
{
  // Present locally attached sensors here
  present(CHILD_ID_WIFI_RSSI, S_SOUND);
  wait(100);
  present(CHILD_ID_RSSI1, S_SOUND);
}

long lastSend = 0;
void loop()
{
  ArduinoOTA.handle();
  if (millis() - lastSend > WIFI_REPORT_INTERVAL) {
    send(msgWifiRssi.set(WiFi.RSSI(), 1));
    lastSend = millis();
  }
  if (sendRssi) {
    sendRssi = false;
    send(msgRssi1.set(rssi, 0));
  }
}

void receive(const MyMessage &message)
{
  rssi = RFM69_getReceivingRSSI();
  if (message.sender == 1) {
    sendRssi = true;
  }
}
