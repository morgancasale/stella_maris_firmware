#include "ESPNow_init.h"

char msg_data[32];

int last_tof_in_range = -1;

bool toggling_lock = false;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&msg_data, incomingData, sizeof(msg_data));
  // Serial.print("Received message: ");
  // Serial.println(msg_data);
  if(last_tof_in_range != -1){
    if(!toggling_lock){
      toggling_lock = true;

      int EM_to_toggle = -1;
      if(EM_in_use != -1) EM_to_toggle = EM_in_use;
      else EM_to_toggle = last_tof_in_range;

      Serial.print("Toggling EM ");
      Serial.println(EM_to_toggle);
      // if(!EMStates[last_tof_in_range]) activateEM(last_tof_in_range);
      // else deactivateEM(last_tof_in_range);
      toggleEM(EM_to_toggle);

      //EM_in_use = EM_to_toggle

      toggling_lock = false;
    }
    
  } else {
    Serial.println("No TOF in range, doing nothing...");
  }
}

void setup_ESPNOW(){
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}