#include "elMagnDriver.h"

// Initialize motor shield and IMU
//DRV8835MotorShield motors(0, 1, 7, 8);
//LSM6DS3 myIMU(I2C_MODE, 0x6A);

// Mode and state variables
int deviceMode = MODE_MANUAL;
bool electromagnetState = ELECTROMAGNET_OFF;
int amplitude = 100;

void setup_EM(){
  pinMode(POSITIVE_PIN, OUTPUT);
  pinMode(NEGATIVE_PIN, OUTPUT);
}

void activateElectromagnet() {
  if (!electromagnetState) {
    //motors.setM1Speed(motorValue);
    // analogWrite(NEGATIVE_PIN, 0);
    // analogWrite(POSITIVE_PIN, EM_POWER);
    digitalWrite(NEGATIVE_PIN, LOW);
    digitalWrite(POSITIVE_PIN, HIGH);
    
    electromagnetState = ELECTROMAGNET_ON;
    Serial.println("Electromagnet ON");
  }
}

void deactivateElectromagnet() {
  if (electromagnetState) {
    // First put positive current to 0
    // analogWrite(POSITIVE_PIN, 0);
    digitalWrite(POSITIVE_PIN, LOW);

    // Then pass negative current for some time 
    // analogWrite(NEGATIVE_PIN, EM_BACK_CURRENT_POWER);
    digitalWrite(NEGATIVE_PIN, HIGH);
    delay(EM_BACK_CURRENT_DELAY);

    // Finally set negative current also back to 0
    // analogWrite(NEGATIVE_PIN, 0);
    digitalWrite(NEGATIVE_PIN, LOW);

    electromagnetState = ELECTROMAGNET_OFF;
    Serial.println("Electromagnet OFF");
  }
}

// void ElMagnetAutoControl(){
//   float normAccel = sqrt(
//     myIMU.readFloatAccelX() * myIMU.readFloatAccelX() +
//     myIMU.readFloatAccelY() * myIMU.readFloatAccelY() +
//     myIMU.readFloatAccelZ() * myIMU.readFloatAccelZ()
//   );

//   //Serial.print("Message = ");
//   //Serial.println(normAccel);
//   if (electromagnetState == true){
//     if (normAccel > 1.08) {
//       lastMoveTime = millis();
//     }else if (millis() - lastMoveTime > 1000) {
//       deactivateElectromagnet();
//       delay(500);
//     }

//   }else{
//     if (normAccel > 1.04) {
//       activateElectromagnet(amplitude);
//       Serial.println(normAccel);
//       Serial.println(amplitude);
//       lastMoveTime = millis();
//     }
//   }
// }