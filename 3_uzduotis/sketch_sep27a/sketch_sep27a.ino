#include <TFT.h>
#include <SPI.h>
#include <Servo.h>
#include <Adafruit_VL53L0X.h>

//const int servoPin = 8;
//Servo servoObj;

// Pins for TFT screen
const int CS  = 10;
const int DC  = 9;
const int RST = 8;

auto screen = TFT(CS, DC, RST); // TFT screen instance
char rangeArray[4];            // char array to be print on the screen

auto lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(9600);
  //servoObj.attach(servoPin);

  // initialize tft screen
  screen.begin();
  screen.background(0, 0, 0);

  // put to infinite loop if failed to boot VL53L0X
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
}

void loop() {
  screen.stroke(255, 255, 255);
  screen.circle(80, 125, 100);
  //screen.setTextSize(2);
  //screen.text("Distance (mm): ", 100, 0);
  
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): ");
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }

  // convert result to char array
  String range = String(measure.RangeMilliMeter);
  range.toCharArray(rangeArray, 4);

  // print range
  screen.stroke(255, 255, 255);
  screen.setTextSize(2);
  screen.text(rangeArray, 0, 0);
  screen.text("mm", 40, 0);

  // print degrees
  screen.text("105", 115, 0);
  screen.circle(155, 4, 2);

  delay(60);
  screen.background(0, 0, 0);
  
  /*
  // rotate from 0 to 180 degrees
  for (int pos = 0; pos < 180; ++pos) {
    servoObj.write(pos);
    delay(15);
  }

  // rotate from 180 to 0 degrees
  for (int pos = 180; pos >= 1; --pos) {
    servoObj.write(pos);
    delay(15);
  }*/
}
