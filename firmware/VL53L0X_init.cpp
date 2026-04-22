#include "VL53L0X_init.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
TwoWire customI2C = TwoWire(0);

bool enabled_TOFs[3] = {true, false, false};
uint8_t num_of_enabled_TOFs = enabled_TOFs[0]+enabled_TOFs[1]+enabled_TOFs[2];

uint64_t WAKE_UP_MASK = 0ULL;

// // Checks if the gpios are high --> the sensor is connected
// void check_active_TOF(){ 
//   pinMode(WAKE_UP_PIN_0, INPUT);
//   if(digitalRead(WAKE_UP_PIN_0)){
//     WAKE_UP_MASK = (1ULL << WAKE_UP_PIN_0); // TODO da cambiare
//     active_TOF[0] = true;
//     Serial.println("TOF 0 available!");
//   }
//   pinMode(WAKE_UP_PIN_1, INPUT);
//   if(digitalRead(WAKE_UP_PIN_1)){
//     WAKE_UP_MASK = WAKE_UP_MASK | (1ULL << WAKE_UP_PIN_1);
//     active_TOF[1] = true;
//     Serial.println("TOF 1 available!");
//   }
//   pinMode(WAKE_UP_PIN_2, INPUT);
//   if(digitalRead(WAKE_UP_PIN_2)){
//     WAKE_UP_MASK = WAKE_UP_MASK | (1ULL << WAKE_UP_PIN_2);
//     active_TOF[2] = true;
//     Serial.println("TOF 2 available!");
//   }
// }

void change_bus(uint8_t bus){
  if(num_of_enabled_TOFs>1){
    Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
    Wire.write(1 << bus);          // send byte to select bus
    Wire.endTransmission();
    Serial.print("Changed I2C bus to "); Serial.print(bus); Serial.println("!");
  }
}

void clear_All_InterruptMask(){
  //Serial.println("Clearing interrupt mask for all TOF!");
  for(uint8_t i = 0; i < 3; i++){
    if(enabled_TOFs[i]){
      change_bus(i+5);
      //Serial.println(lox.clearInterruptMask(false));
      lox.clearInterruptMask(false);
      //Serial.print("Cleared TOF "); Serial.println(i+5);
      delay(100);
    }
  }
}

bool check_in_range_any(){
  clear_All_InterruptMask();
  delay(300);

  bool result = false;
  for(int i=0; i< 3; i++){
    if(enabled_TOFs[i]){
      bool a = digitalRead(i);
      //Serial.println(a);
      result |= !a;
    }
  }
  return result;
}

void setup_VL530X(){
  Wire.begin();
  //check_active_TOF(); //Check which TOF are connected and define the wake up deep sleep mask

  //customI2C.begin(CUSTOM_SDA, CUSTOM_SCL);
  
  for(uint8_t i = 0; i < 3; i++){
    if(enabled_TOFs[i]){
      // The interrupt's pins need to be high as the TOF interrupt pulls them low 
      //pinMode(i, INPUT_PULLUP);

      change_bus(i+5);
      // Initialize sensor
      while(!lox.begin(VL530X_I2C_ADD)) {
        Serial.println(F("Failed to boot VL53L0X"));
        delay(200);
      }

      // Slower reading but way more accurate
      lox.configSensor(lox.VL53L0X_SENSE_HIGH_ACCURACY);

      //Serial.print("TOF "); Serial.println(i+5);

      // Setup continous ranging mode and turn on the low threshold interrupt
      lox.setGpioConfig(
        VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
        VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
        VL53L0X_INTERRUPTPOLARITY_HIGH
      );

      //lox.getGpioConfig(VL53L0X_DeviceModes *pDeviceMode, VL53L0X_GpioFunctionality *pFunctionality, VL53L0X_InterruptPolarity *pPolarity)

      // Set low and high interrupt thresholds
      lox.setInterruptThresholds(tof_low_thr * 65536.0, tof_high_thr * 65536.0);
      
      // Enable Continous Measurement Mode
      lox.setDeviceMode(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, false);

      lox.startMeasurement();
    }
  }
}