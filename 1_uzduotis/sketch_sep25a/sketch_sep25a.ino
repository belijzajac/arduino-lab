#include <LedControl.h> // for LED Dot Matrix

const int button1Pin = 2; // yellow button
const int button2Pin = 3; // red button

// LED Dot Matrix pins
const int DIN = 12;
const int CS  = 10;
const int CLK = 11;

LedControl lc = LedControl(DIN, CLK, CS, 1);

void setup() {
  Serial.begin(9600);

  // https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
  pinMode(button1Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(button1Pin), onYellowBtnPress, FALLING);
  pinMode(button2Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(button2Pin), onRedBtnPress, FALLING);
  
  lc.shutdown(0, false);    // wake up led display
  lc.setIntensity(0, 1);    // set intensity to 1 (0-15)
  lc.clearDisplay(0);       // clear display
  lc.setLed(0, 3, 0, true); // light up a pixel at (3, 0)
}

struct Player {
  int prevRow = 0;
  int currRow = 3;

  // update player's position
  void update() {
    if (currRow != prevRow) {
      lc.setLed(0, prevRow, 0, false);
      lc.setLed(0, currRow, 0, true);
    }
  }

  // move player "dot" down
  void increase() {
    if (currRow + 1 < 8) {
      prevRow = currRow;
      ++currRow;
      update();
    }
  }

  // move player "dot" up
  void decrease() {
    if (currRow - 1 >= 0) {
      prevRow = currRow;
      --currRow;
      update();
    }
  }
};

struct Player playerDot{};

void loop() {
  /*for (int row=0; row < 8; ++row) {
    for (int col=0; col < 8; ++col) {
      lc.setLed(0, row, col, true);      
      delay(50);
    }
  }*/
}

// interrupt function to trigger on the yellow button press
void onYellowBtnPress() {
  playerDot.decrease();
}

// interrupt function to trigger on the red button press
void onRedBtnPress() {
  playerDot.increase();
}
