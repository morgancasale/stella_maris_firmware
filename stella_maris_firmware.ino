#include "BLEHandlers.h"
#include "elMagnDriver.h"

unsigned long lastMoveTime = 0;

int buttonState = 0;

void setup(){
  Serial.begin(9600);

  Serial.println("Starting setup...");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // device is on

  pinMode(P0_13, OUTPUT); // charging Pin

  setup_BLE();

  // The controller doesn't use the electromagnet and uses pin 0 and 1
  // for buttons
  if(deviceName != "Controller"){
    setup_EM();
  }

  Serial.println("Setup complete!");

  // if (myIMU.begin() != 0) {
  //   Serial.println("IMU initialization error!");
  // } else {
  //   Serial.println("IMU ready!");
  // }
  // motors.setM1Speed(0);
}

void loop(){
  BLEDevice central = BLE.central();
  digitalWrite(P0_13, LOW); //Charge Battery

  digitalWrite(LED_RED, !digitalRead(LED_RED));
  delay(300);

  if (central){
    digitalWrite(LED_RED, HIGH); 
    digitalWrite(LED_BLUE, LOW);

    while (central.connected()){
      delay(100);
    }
  }
  
}
