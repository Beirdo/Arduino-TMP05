// #include <EnableInterrupt.h>

#include "TMP05.h"

TMP05 sensors(4, 8, 9);

void setup() {
}

void loop() {
  uint8_8t reading;
  
  if (sensors.getState() == -1) {
    for (uint8_t i=0; i < 4; i++) {
      reading = sensors.getReading(i);
      sensors.startConversion();
    }
  }
  delay(1);
}  
