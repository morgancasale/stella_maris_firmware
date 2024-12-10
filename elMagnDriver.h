#ifndef elMagnDriver_h
#define elMagnDriver_h
  //#include <DRV8835MotorShield.h>
  #include <Arduino.h>


  // Constants for operating modes and electromagnet state
  #define MODE_MANUAL 1
  #define MODE_AUTO 2
  #define ELECTROMAGNET_ON true
  #define ELECTROMAGNET_OFF false

  #define POSITIVE_PIN D0
  #define NEGATIVE_PIN D1

  #define EM_POWER 255
  #define EM_BACK_CURRENT_POWER 255
  #define EM_BACK_CURRENT_DELAY 200

  //extern DRV8835MotorShield motors;
  //extern LSM6DS3 myIMU;

  // Mode and state variables
  extern int deviceMode;
  extern bool electromagnetState;
  extern int amplitude;

  void setup_EM();

  void activateElectromagnet();
  void deactivateElectromagnet();

  extern void ElMagnetAutoControl();

#endif