#include "BLEHandlers.h"
#include "elMagnDriver.h"

unsigned long lastMoveTime = 0;

int buttonState = 0;

void setup(){
  Serial.begin(9600);

  Serial.println("Starting setup...");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(P0_13, OUTPUT); // charging Pin

  tone(0, 21000);
  tone(1, 21000);

  setup_BLE();
  setup_EM();

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
      // buttonState = digitalRead(buttonPin);
      // if(buttonState == HIGH){
      //   onButtonPress();
      // }
      // delay(100);
    }
  }
  
}
