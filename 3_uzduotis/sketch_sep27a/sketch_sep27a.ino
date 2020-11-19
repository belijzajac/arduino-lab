#include <Servo.h> // to control servo

const int servoPin = 9;
Servo servoObj;

void setup() {
  Serial.begin(9600);

  servoObj.attach(servoPin);
}


void loop() {
  // rotate from 0 to 180 degrees
  for (int pos = 0; pos < 180; ++pos) {
    servoObj.write(pos);
    delay(15);
  }

  // rotate from 180 to 0 degrees
  for (int pos = 180; pos >= 1; --pos) {
    servoObj.write(pos);
    delay(15);
  }
}
