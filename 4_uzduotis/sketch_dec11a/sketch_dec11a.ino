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
  //ucg.begin(UCG_FONT_MODE_SOLID);
}

// Didysis apskritimas
// centro x: 64
// centro y: 80
// rad: 60
void clear_display () {
  ucg.clearScreen();
  
  ucg.setFont(ucg_font_ncenR12_tr);
  ucg.setColor(255, 255, 255);
  ucg.setColor(1, 255, 0,0);
  
  ucg.setColor(255, 255, 255);
  ucg.drawCircle(64, 80, 60, UCG_DRAW_ALL);
}

void draw_obstacle (int servoDegrees, int tofDistance) {  
  ucg.setColor(0, 255, 0);
  
  float x_pos = 64 + 60 * cos(servoDegrees * PI / 180) * tofDistance/300;
  float y_pos = 80 + 60 * sin(servoDegrees * PI / 180) * tofDistance/300;
  
  ucg.drawCircle(x_pos, y_pos, 2, UCG_DRAW_ALL);
}

void measureDistance(int servoPos) {
  VL53L0X_RangingMeasurementData_t result;
  lox.rangingTest(&result, false); // false - don't output debug info

  // if not an error code and within 30 cm
  if (result.RangeStatus != 4 && result.RangeMilliMeter <= 300) {
    draw_obstacle(servoPos, result.RangeMilliMeter);
  }
}

void loop() {
  clear_display();
  
  // rotate from 0 to 180 degrees
  for (int pos = 0; pos < 180; ++pos) {
    servoObj.write(pos);
    measureDistance(pos);
  }

  clear_display();

  // rotate from 180 to 0 degrees
  for (int pos = 180; pos >= 1; --pos) {
    servoObj.write(pos);
    measureDistance(pos);
  }
}
