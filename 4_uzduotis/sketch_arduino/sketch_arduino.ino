#include <Adafruit_VL53L0X.h> // for ToF distance sensor
#include <Servo.h>            // for Servo motor

auto lox = Adafruit_VL53L0X();

const int servoPin = 12;
Servo servoObj;

void setup() {
  Serial.begin(9600);
  
  // servo
  servoObj.attach(servoPin);

  // put to infinite loop if failed to boot CJVL53L0XV2
  if (!lox.begin()) {
    Serial.println(F("Failed to boot CJVL53L0XV2"));
    while(1);
  }
}

void measureDistance(int servoPos) {
  VL53L0X_RangingMeasurementData_t result;
  lox.rangingTest(&result, false); // false - don't output debug info

  // if not an error
  if (result.RangeStatus != 4) {
    // output to serial: "<angle> <distance>"
    Serial.print(servoPos);
    Serial.print(" ");
    Serial.print(result.RangeMilliMeter);
    Serial.println();
  } else {
    ; // do nothing since we received an error
      // indicating that we're out of range
  }
}

void loop() {
  // rotate from 0 to 180 degrees
  for (int pos = 0; pos < 180; ++pos) {
    servoObj.write(pos);
    measureDistance(pos);
  }

  // rotate from 180 to 0 degrees
  for (int pos = 180; pos >= 1; --pos) {
    servoObj.write(pos);
    measureDistance(pos);
  }
}
