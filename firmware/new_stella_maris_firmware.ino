#include "ESPNow_init.h"

#define waketime 5000

hw_timer_t *wakeTime_timer = NULL;

bool sleep_flag = false;

RTC_DATA_ATTR bool first_boot = true;
void wakeup_routine(int wake_up_pin){
  if(first_boot) {
    Serial.println("First boot!");
  } else{
    Serial.print("I'm UP! Waken by TOF "); Serial.print(wake_up_pin+1); 
    Serial.print(" on D"); Serial.print(wake_up_pin); Serial.println(".");
  }
  
  Serial.print("Waiting "); Serial.print(waketime/1000); Serial.println("s before going back to sleep...");
  //delay(waketime);
}
void IRAM_ATTR sleep_time(){
  sleep_flag = true;
}

// https://www.electronicwings.com/esp32/esp32-timer-interrupts
void setup_WakeTime_timer(){
  // Set timer frequency to 1Mhz
  wakeTime_timer = timerBegin(1000000);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(wakeTime_timer, &sleep_time);

  // Set alarm time (in microseconds)
  timerAlarm(wakeTime_timer, waketime*1000, true, 0);

  Serial.println("Timer set up!");
}

bool clean_int = false;
void reset_timer(){
  bool cond = false;

  for(uint8_t TOF_pin = 0; TOF_pin<3; TOF_pin++){
    if (enabled_TOFs[TOF_pin] && digitalRead(TOF_pin)){
      cond = true;
      last_tof_in_range = TOF_pin;
    }
  }

  // if (enabled_TOFs[0]) cond |= digitalRead(WAKE_UP_PIN_0);
  // if (enabled_TOFs[1]) cond |= digitalRead(WAKE_UP_PIN_1);
  // if (enabled_TOFs[2]) cond |= digitalRead(WAKE_UP_PIN_2);

  if(cond) clean_int = true;
}

void setup() {
  uint64_t wakeup_pin = esp_sleep_get_ext1_wakeup_status() >> 1ULL;

  // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/sleep_modes.html#_CPPv431esp_sleep_disable_wakeup_source18esp_sleep_source_t
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL); // Disable ext trigger wake for deep sleep to avoid debounce

  Serial.begin(9600);
  delay(500);
  Serial.println("Starting...");

  setup_EMs();

  setup_ESPNOW();

  setup_VL530X();

  // Setup interrupt handles for the TOF sensors to keep track if an object is still in range while awake
  for(uint8_t TOF_pin=0; TOF_pin < 3; TOF_pin++){
    if(enabled_TOFs[TOF_pin]){
      pinMode(TOF_pin, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(TOF_pin), reset_timer, CHANGE); //ONLY CHANGE WORKS BOOOOH
      deactivateEM(TOF_pin);
    }
  }

  if(first_boot){
    first_boot = false;
  }

  wakeup_routine(wakeup_pin);
  
  setup_WakeTime_timer();
  clear_All_InterruptMask();
}



bool first_test = false;
void loop() {
  if (sleep_flag && EM_in_use == -1){
    // Serial.println("Sleep time?");
    sleep_flag = false;

    if (enabled_TOFs[0]) WAKE_UP_MASK |= (1ULL << WAKE_UP_PIN_0);
    if (enabled_TOFs[1]) WAKE_UP_MASK |= (1ULL << WAKE_UP_PIN_1);
    if (enabled_TOFs[2]) WAKE_UP_MASK |= (1ULL << WAKE_UP_PIN_2);

    Serial.print("Current wake_up mask: ");
    Serial.println(WAKE_UP_MASK);
    
    // https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/system/sleep_modes.html#_CPPv428esp_sleep_enable_ext1_wakeup8uint64_t28esp_sleep_ext1_wakeup_mode_t
    esp_sleep_enable_ext1_wakeup(WAKE_UP_MASK, ESP_EXT1_WAKEUP_ANY_HIGH);
        
    clear_All_InterruptMask();
    delay(1000);

    Serial.println("Going back to sleep...");
    esp_deep_sleep_start();
  } else {
    sleep_flag = false;
  }

  // If object still in range the sleep timer is reset and the device doesn't go to sleep
  if(clean_int || EM_in_use != -1){
    if(clean_int){
      Serial.print("Object still in range of");
      for(uint8_t TOF_pin=0; TOF_pin < 3; TOF_pin++){
        if(enabled_TOFs[TOF_pin] && digitalRead(TOF_pin)){
          Serial.print(" TOF "); Serial.print(TOF_pin);
        }
      }
      Serial.println("!");
      
      Serial.print("Last TOF in range: "); Serial.println(last_tof_in_range);
      clean_int = false;
    }

    if(EM_in_use != -1){
      Serial.print("EM"); Serial.print(EM_in_use);
      Serial.println(" is still in use! Can't go to sleep...");
    }

    clear_All_InterruptMask();
    
    timerRestart(wakeTime_timer);
    delay(50);
  }

  
  
  delay(10);
}
