#include "elMagnDriver.h"

// Initialize motor shield and IMU
//DRV8835MotorShield motors(0, 1, 7, 8);
//LSM6DS3 myIMU(I2C_MODE, 0x6A);

// Mode and state variables
int deviceMode = MODE_MANUAL;
bool EMStates[3] = {ELECTROMAGNET_OFF, ELECTROMAGNET_OFF, ELECTROMAGNET_OFF};
int EM_in_use = 3; //Until setup_EMs() isn't called the Electromagnets will not work 
int amplitude = 100;


void setup_EMs(){
  for(uint8_t i=0; i < 3; i++){
    pinMode(positive_pins[i], OUTPUT);
    analogWrite(positive_pins[i], 0);
    delay(10);
    pinMode(negative_pins[i], OUTPUT);
    analogWrite(negative_pins[i], 0);
    delay(20);
  }
  EM_in_use = -1;
  Serial.print("EM value: "); Serial.println(EM_in_use);
}

void activateEM(uint8_t EM_num){
  if(EM_num < 0 || EM_num > 2) return;

  if(EM_in_use != -1 && EM_in_use != EM_num){
    Serial.println("An EM is already in use!");
    return;
  }

  if (!EMStates[EM_num] && enabled_TOFs[EM_num]){
    EM_in_use = EM_num;

    analogWrite(negative_pins[EM_num], 0);
    analogWrite(positive_pins[EM_num], EM_POWER);
    
    EMStates[EM_num] = ELECTROMAGNET_ON;
    Serial.print("Electromagnet "); Serial.print(EM_num); Serial.println(" is ON!");
  }
}

void deactivateEM(uint8_t EM_num){
  if(EM_num < 0 || EM_num > 2) return;

  if(EM_in_use != -1 && EM_in_use != EM_num){
    Serial.println("An EM is already in use!");
    return;
  }

  if (EMStates[EM_num] && enabled_TOFs[EM_num]){
    // First put positive current to 0
    analogWrite(positive_pins[EM_num], 0);
    // Then pass negative current for some time 
    analogWrite(negative_pins[EM_num], EM_BACK_CURRENT_POWER);
    delay(EM_BACK_CURRENT_DELAY);

    // Finally set negative current also back to 0
    analogWrite(negative_pins[EM_num], 0);

    EMStates[EM_num] = ELECTROMAGNET_OFF;

    EM_in_use = -1;

    Serial.println("Electromagnet OFF");
  }
}


void toggleEM(uint8_t EM_num){
  if(EM_num < 0 || EM_num > 2) return;

  if(EM_in_use != -1 && EM_in_use != EM_num){
    Serial.println("An EM is already in use!");
    return;
  }

  if (!EMStates[EM_num] && enabled_TOFs[EM_num]){
    activateEM(EM_num);
  } else if(enabled_TOFs[EM_num]){
    deactivateEM(EM_num);
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