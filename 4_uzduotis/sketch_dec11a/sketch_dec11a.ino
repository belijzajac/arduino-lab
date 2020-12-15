#include <Adafruit_VL53L0X.h> // for ToF distance sensor
#include <Servo.h>            // for Servo motor

// for TFT display
#include <SPI.h>
#include "Ucglib.h"

// pins for TFT screen
const int CLK = 13;
const int DIN = 11;
const int DC  = 9;
const int CS  = 10;
const int RST = 8;

Ucglib_ST7735_18x128x160_SWSPI ucg(CLK, DIN, DC, CS, RST); // TFT display's object
auto lox = Adafruit_VL53L0X();                             // ToF sensor's objec

const int SERVO_PIN = 12;
Servo servoObj;

// constants for ToF
const int WORKING_DISTANCE = 600; // 300 mm (= 30 cm)
// constants for the big circle
const int CIRCLE_CENTER_X = 64;
const int CIRCLE_CENTER_Y = 80;
const int CIRCLE_RADIUS   = 60;

void setup() {
  Serial.begin(9600);

  // servo
  servoObj.attach(SERVO_PIN);

  // put to infinite loop if failed to boot CJVL53L0XV2
  if (!lox.begin()) {
    Serial.println(F("Failed to boot CJVL53L0XV2"));
    while(1);
  }

  // setting up the TFT display
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
}

void clearDisplay () {
  ucg.clearScreen();
  
  ucg.setFont(ucg_font_ncenR12_tr);
  ucg.setColor(255, 255, 255);
  ucg.setColor(1, 255, 0,0);
  
  ucg.setColor(255, 255, 255);
  ucg.drawCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_RADIUS, UCG_DRAW_ALL);
}

void drawObstacle (int servoDegrees, int tofDistance) {  
  ucg.setColor(0, 255, 0); // green
  
  float x_pos = CIRCLE_CENTER_X + CIRCLE_RADIUS * cos(servoDegrees * PI / 180) * tofDistance/WORKING_DISTANCE;
  float y_pos = CIRCLE_CENTER_Y + CIRCLE_RADIUS * sin(servoDegrees * PI / 180) * tofDistance/WORKING_DISTANCE;
  
  ucg.drawCircle(x_pos, y_pos, 2, UCG_DRAW_ALL);
}

void measureDistance(int servoPos) {
  VL53L0X_RangingMeasurementData_t result;
  lox.rangingTest(&result, false); // false - don't output debug info

  // if not an error code and within working distance
  if (result.RangeStatus != 4 && result.RangeMilliMeter <= WORKING_DISTANCE) {
    drawObstacle(servoPos, result.RangeMilliMeter);
  }
}

void loop() {
  clearDisplay();
  
  // rotate from 0 to 180 degrees
  for (int pos = 0; pos < 180; ++pos) {
    servoObj.write(pos);
    measureDistance(pos);
  }

  clearDisplay();

  // rotate from 180 to 0 degrees
  for (int pos = 180; pos >= 1; --pos) {
    servoObj.write(pos);
    measureDistance(pos);
  }
}
