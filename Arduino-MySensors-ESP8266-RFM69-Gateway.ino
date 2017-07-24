// Based on https://github.com/mysensors/MySensors/tree/master/examples/GatewayESP8266OTA

#include <ArduinoOTA.h>

// Enable debug prints to serial monitor
#define MY_DEBUG

// Use a bit lower baudrate for serial prints on ESP8266 than default in MyConfig.h
#define MY_BAUD_RATE 9600
#define WIFI_REPORT_INTERVAL 60000

// Enables and select radio type (if attached)
#define MY_RADIO_RFM69
#define MY_IS_RFM69HW
#define MY_RF69_IRQ_PIN D2
#define MY_RF69_IRQ_NUM MY_RF69_IRQ_PIN
#define MY_RF69_SPI_CS D8
#define MY_RFM69_FREQUENCY RFM69_433MHZ // RFM69_433MHZ for development branch, RF69_433MHZ for master

#define MY_GATEWAY_ESP8266

#define MY_ESP8266_SSID "Mr-IoT.com"
#include "settings.h"

// Set the hostname for the WiFi Client. This is the hostname
// it will pass to the DHCP server if not static.
#define MY_ESP8266_HOSTNAME "RFM69-Gateway"

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

void setup()
{
  // Setup locally attached sensors
  ArduinoOTA.onStart([]() {
    debug("ArduinoOTA start\n");
  });
  ArduinoOTA.onEnd([]() {
    debug("\nArduinoOTA end\n");
  });
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    debug("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    debug("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      debug("Auth Failed\n");
    } else if (error == OTA_BEGIN_ERROR) {
      debug("Begin Failed\n");
    } else if (error == OTA_CONNECT_ERROR) {
      debug("Connect Failed\n");
    } else if (error == OTA_RECEIVE_ERROR) {
      debug("Receive Failed\n");
    } else if (error == OTA_END_ERROR) {
      debug("End Failed\n");
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
  // Send locally attech sensors data here
  ArduinoOTA.handle();
  if (millis() - lastSend > WIFI_REPORT_INTERVAL) {
    send(msgWifiRssi.set(WiFi.RSSI(), 1));
    lastSend = millis();
  }
}

void receive(const MyMessage &message)
{
  int16_t rssi = _radio.RSSI;
  if (message.sender == 1) {
    send(msgRssi1.set(rssi, 0));
  }
}

