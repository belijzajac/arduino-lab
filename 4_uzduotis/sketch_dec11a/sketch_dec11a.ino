#include <Adafruit_VL53L0X.h> // for ToF distance sensor
#include <Servo.h>            // for Servo motor

// for TFT display
#include <SPI.h>
#include "Ucglib.h"
Ucglib_ST7735_18x128x160_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

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

  // setting up the TFT display
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();
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
  //ucg.setRotate90();
  ucg.setFont(ucg_font_ncenR12_tr);
  ucg.setColor(255, 255, 255);
  //ucg.setColor(0, 255, 0);
  ucg.setColor(1, 255, 0,0);
  
  ucg.setPrintPos(0,25);
  ucg.print("Hello World!");
  
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
