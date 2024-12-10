#ifndef BLEhandlers_h
#define BLEhandlers_h
  #include <ArduinoBLE.h>

  #define buttonPin D0
  #define DEBOUNCE_DELAY 200  // Milliseconds

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

  void setup_BLE();

  void onButtonPress();

  //BLE connection handlers
  void blePeripheralConnectHandler(BLEDevice central);
  void blePeripheralDisconnectHandler(BLEDevice central);

  //BLE characteristics handlers
  void onCommand(BLEDevice central, BLECharacteristic characteristic);
  void onModeChange(BLEDevice central, BLECharacteristic characteristic);

#endif