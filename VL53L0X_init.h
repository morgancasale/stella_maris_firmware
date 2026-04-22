#ifndef VL530X_init_h
#define VL530X_init_h
  #include "Adafruit_VL53L0X.h"
  #include "Wire.h"
  
  #define VL530X_I2C_ADD 0x29

  #define CUSTOM_SDA D4
  #define CUSTOM_SCL D5

  #define WAKE_UP_PIN_0 D0 // GPIO 0 bitmask for ext1
  #define WAKE_UP_PIN_1 D1
  #define WAKE_UP_PIN_2 D2 // GPIO 2 is used by the internal led

  extern uint64_t WAKE_UP_MASK;

  extern bool enabled_TOFs[3];
  extern uint8_t num_of_enabled_TOFs;

  extern Adafruit_VL53L0X lox;
  extern TwoWire customI2C;

  const FixPoint1616_t tof_low_thr = 40; // [mm]
  const FixPoint1616_t tof_high_thr = 200; // [mm] random high value, it will not be used

  void change_bus(uint8_t bus);
  void setup_VL530X();

  void clear_All_InterruptMask();
  bool check_in_range_any();
  void check_active_TOF();
#endif