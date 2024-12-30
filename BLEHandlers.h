#ifndef BLEhandlers_h
#define BLEhandlers_h
  #include <ArduinoBLE.h>

  #define buttonPin D2
  #define DEBOUNCE_DELAY 200  // Milliseconds

  #define BLUE 12
  #define RED 11
  #define GREEN 13

  extern String serviceUUID;
  extern String cmdCharUUID;
  extern String modeCharUUID;
  extern String msgCharUUID;

  extern const int maxCharacteristicBytes;

  extern volatile unsigned long lastDebounceTime;

  extern BLEService electromagnetService;
  extern BLECharacteristic commandCharacteristic;
  extern BLECharacteristic modeCharacteristic;

  extern String deviceName;

  extern bool buttonPressed;

  void setup_BLE();

  void onButtonPress();

  //BLE connection handlers
  void blePeripheralConnectHandler(BLEDevice central);
  void blePeripheralDisconnectHandler(BLEDevice central);

  //BLE characteristics handlers
  void onCommand(BLEDevice central, BLECharacteristic characteristic);
  void onModeChange(BLEDevice central, BLECharacteristic characteristic);

  //BLE status updater
  void updateEMStatus();
#endif