# Arduino-MySensors-ESP8266-RFM69-Gateway
MySensors RFM69 gateway for ESP8266 with OTA support.

Also has support for reporting RSSI for RFM69 node(s) and the Wifi connection.

Wifi password (and optionally SSID) is kept safe in settings.h which is ignored by git. settings.h can look like this:
```
#define MY_WIFI_PASSWORD "mysecretwifipassword123"
#define OTA_PASSWORD "mysecretotapassword123"
```

Designed for MySensors 2.3

