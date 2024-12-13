#include "BLEHandlers.h"
#include "elMagnDriver.h"

String serviceUUID = "19B10000-E8F2-537E-4F6C-D104768A1214";
String cmdCharUUID = "19B10001-E8F2-537E-4F6C-D104768A1214";
String modeCharUUID = "19B10002-E8F2-537E-4F6C-D104768A1214";
String msgCharUUID = "19B10003-E8F2-537E-4F6C-D104768A1214";
String stateCharUUID = "19B10004-E8F2-537E-4F6C-D104768A1214";

const int maxCharacteristicBytes = 4;

volatile unsigned long lastDebounceTime = 400;

// DEVICE NAME
String deviceName = "Orange";

// Initialize BLE service and characteristics
BLEService electromagnetService(serviceUUID.c_str());
BLECharacteristic commandCharacteristic(cmdCharUUID.c_str(), BLEWrite, maxCharacteristicBytes);
BLECharacteristic modeCharacteristic(modeCharUUID.c_str(), BLEWrite, maxCharacteristicBytes);
BLECharacteristic msgCharacteristic(msgCharUUID.c_str(), BLERead | BLENotify, maxCharacteristicBytes);
BLECharacteristic stateCharacteristic(stateCharUUID.c_str(), BLEWrite, maxCharacteristicBytes);


void setup_BLE(){
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName(deviceName.c_str());
  BLE.setAdvertisedService(electromagnetService);

  electromagnetService.addCharacteristic(commandCharacteristic);
  electromagnetService.addCharacteristic(modeCharacteristic);
  electromagnetService.addCharacteristic(msgCharacteristic);
  electromagnetService.addCharacteristic(stateCharacteristic);
  
  BLE.addService(electromagnetService);

  // assign event handlers for connected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign events to the characteristics
  commandCharacteristic.setEventHandler(BLEWritten, onCommand);
  modeCharacteristic.setEventHandler(BLEWritten, onModeChange);

  // setup action button only for the controller
  if(deviceName == "Controller"){
    attachInterrupt(digitalPinToInterrupt(buttonPin), onButtonPress, RISING);
  }

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void onButtonPress(){
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  msgCharacteristic.writeValue(uint32_t(32));
}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());

  digitalWrite(LED_BLUE, HIGH); 
}

void onCommand(BLEDevice central, BLECharacteristic characteristic){
  Serial.print("Command received: ");
  int cmdValue = commandCharacteristic.value()[0];
  Serial.println(cmdValue);

  if(cmdValue == 0 || electromagnetState){
    deactivateElectromagnet();
    //stateCharacteristic.writeValue(0);
  } else {
    activateElectromagnet();
    //stateCharacteristic.writeValue(1);
  }
}

void onModeChange(BLEDevice central, BLECharacteristic characteristic){
  byte value[4];
  memcpy(value, modeCharacteristic.value(), 4);
  //deviceMode = value[3] | (value[2] << 8) | (value[1] << 16) | (value[0] << 24);
}