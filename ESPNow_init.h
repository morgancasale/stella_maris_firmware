#ifndef ESPNow_init_h
#define ESPNow_init_h
  #include "esp_now.h"
  #include "WiFi.h"
  #include "elMagnDriver.h"


  #define ESPNOW_WIFI_CHANNEL 6

  extern char msg_data[32];

  extern int last_tof_in_range;

  void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

  void setup_ESPNOW();
#endif