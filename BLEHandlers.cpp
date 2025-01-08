#include "BLEHandlers.h"
#include "elMagnDriver.h"

String serviceUUID = "19B10000-E8F2-537E-4F6C-D104768A1214";
String onCmdCharUUID = "19B10001-E8F2-537E-4F6C-D104768A1214";
String modeCharUUID = "19B10002-E8F2-537E-4F6C-D104768A1214";
String sendCmdCharUUID = "19B10003-E8F2-537E-4F6C-D104768A1214";
String EMStateCharUUID = "19B10004-E8F2-537E-4F6C-D104768A1214";

const int maxCharacteristicBytes = 4;

volatile unsigned long lastDebounceTime = 400;

// DEVICE NAME
String deviceName = "Controller";

bool buttonPressed = false;

// Initialize BLE service and define characteristics
BLEService electromagnetService(serviceUUID.c_str());

BLECharacteristic onCmdCharacteristic(onCmdCharUUID.c_str(), BLEWrite | BLENotify, maxCharacteristicBytes);
BLECharacteristic modeCharacteristic(modeCharUUID.c_str(), BLEWrite, maxCharacteristicBytes);
BLECharacteristic sendCmdCharacteristic(sendCmdCharUUID.c_str(), BLERead | BLENotify, maxCharacteristicBytes);

const String state_templ = "{\"device_name\" : \"" + deviceName + "\", \"EM_state\" : \"%s\"}";
BLECharacteristic EMStateCharacteristic(EMStateCharUUID.c_str(), BLERead | BLENotify, state_templ.length()+String("false").length()+1);

void setup_BLE(){
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName(deviceName.c_str());
  BLE.setAdvertisedService(electromagnetService);

  electromagnetService.addCharacteristic(onCmdCharacteristic);
  electromagnetService.addCharacteristic(modeCharacteristic);
  electromagnetService.addCharacteristic(sendCmdCharacteristic);
  electromagnetService.addCharacteristic(EMStateCharacteristic);
  
  BLE.addService(electromagnetService);

  // assign event handlers for connected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign events to the characteristics
  if(deviceName != "Controller") onCmdCharacteristic.setEventHandler(BLEWritten, onCommand);
  // modeCharacteristic.setEventHandler(BLEWritten, onModeChange);

  // setup action button only for the controller 
  attachInterrupt(digitalPinToInterrupt(buttonPin), onButtonPress, RISING);

  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void onButtonPress(){
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  sendCmdCharacteristic.writeValue(uint32_t(32));
  buttonPressed = true;
}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());

  digitalWrite(LED_BLUE, HIGH); 
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());

  deactivateElectromagnet();
  digitalWrite(LED_BLUE, LOW);
  //digitalWrite(RED, HIGH);
}

void onCommand(BLEDevice central, BLECharacteristic characteristic){
  Serial.print("Command received: ");
  int cmdValue = onCmdCharacteristic.value()[0];
  Serial.println(cmdValue);

  bool turn_on = (cmdValue == 1) || (cmdValue == 2 && !electromagnetState); 

  if(turn_on){
    activateElectromagnet();
  } else {
    deactivateElectromagnet();
  }
  
  updateEMStatus();  
}

void updateEMStatus(){
  String EM_state = electromagnetState ? "true" : "false";

  char state_msg[50];
  sprintf(state_msg, state_templ.c_str(), EM_state.c_str());

  if(EMStateCharacteristic.writeValue(state_msg)){
    Serial.print("Written state msg: ");
    Serial.println(state_msg);
  } else {
    Serial.println("Error while writing state!");
  }
}

// void onModeChange(BLEDevice central, BLECharacteristic characteristic){
//   byte value[4];
//   memcpy(value, modeCharacteristic.value(), 4);
//   //deviceMode = value[3] | (value[2] << 8) | (value[1] << 16) | (value[0] << 24);
// }