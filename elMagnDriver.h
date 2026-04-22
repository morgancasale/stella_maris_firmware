#ifndef elMagnDriver_h
#define elMagnDriver_h
  //#include <DRV8835MotorShield.h>
  // #include <Arduino_LSM6DS3.h>
  #include <Arduino.h>
  #include "VL53L0X_init.h"

  // Constants for operating modes and electromagnet state
  #define MODE_MANUAL 1
  #define MODE_AUTO 2
  #define ELECTROMAGNET_ON true
  #define ELECTROMAGNET_OFF false

  #define EM_POWER 255
  #define EM_BACK_CURRENT_POWER 255
  #define EM_BACK_CURRENT_DELAY 200
  #define POSITIVE_PIN D0
  #define NEGATIVE_PIN D1

  const uint8_t positive_pins[3] = {D6, D8, D10};
  const uint8_t negative_pins[3] = {D3, D7, D9};

  //extern DRV8835MotorShield motors;
  //extern LSM6DS3 myIMU;

  // Mode and state variables
  extern int deviceMode;
  extern bool EMStates[3];
  extern int EM_in_use;
  extern int amplitude;

  void setup_EMs();

  void activateEM(uint8_t EM_num);
  void deactivateEM(uint8_t EM_num);
  void toggleEM(uint8_t EM_num);

  extern void ElMagnetAutoControl();

#endif