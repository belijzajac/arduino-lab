#include <LedControl.h>    // for LED Dot Matrix
#include <LiquidCrystal.h> // for LCD display

const int button1Pin = 2; // yellow button
const int button2Pin = 3; // red button

// pins for LED Dot Matrix
const int DIN = 12;
const int CS  = 10;
const int CLK = 11;

// pins for LCD display
const int RS = 9;
const int EN = 8;
const int D4 = 7;
const int D5 = 6;
const int D6 = 5;
const int D7 = 4;

auto lc  = LedControl(DIN, CLK, CS, 1);
auto lcd = LiquidCrystal(RS, EN, D4, D5, D6, D7);

unsigned long drawFrameStart = millis();
unsigned long scoreStart = millis();

byte heartSymbol[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

void setup() {
  Serial.begin(9600);

  DDRD = B11110011; // set pins 2 and 3 to input, the rest to output
  DDRB = B11111111; // set all of pins 6-13 to output
  //pinMode(button1Pin, INPUT);
  //pinMode(button2Pin, INPUT);
  
  // https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
  attachInterrupt(digitalPinToInterrupt(button1Pin), onYellowBtnPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(button2Pin), onRedBtnPress, FALLING);
  
  lc.shutdown(0, false);    // wake up led display
  lc.setIntensity(0, 1);    // set intensity to 1 (0-15)
  lc.clearDisplay(0);       // clear display
  lc.setLed(0, 7, 3, true); // light up a pixel at (7, 3)

  // set up LCD display
  lcd.createChar(0, heartSymbol);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Highscore:");
}

struct Player {
  int score = 0;
  int numLives = 3;
  int prevCol = 0;
  int currCol = 3;

  // update player's position
  void draw() {
    if (currCol != prevCol) {
      lc.setLed(0, 7, prevCol, false);
      lc.setLed(0, 7, currCol, true);
    }
  }

  // move player "dot" to the right
  void increase() {
    if (currCol + 1 < 8) {
      prevCol = currCol;
      ++currCol;
      draw();
    }
  }

  // move player "dot" to the left
  void decrease() {
    if (currCol - 1 >= 0) {
      prevCol = currCol;
      --currCol;
      draw();
    }
  }
};

struct FallingObjects {
  int clmns[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int turnNo = 1;
  bool isGameOver = false;

  void draw() {
    for(int i = 0; i < 8; ++i){
      if(clmns[i] > 0) {
        // draws a 3-dots-long tail
        lc.setLed(0, clmns[i] - 1, i, true);
        lc.setLed(0, clmns[i] - 2, i, true);
        lc.setLed(0, clmns[i] - 3, i, true);
      }
    }
  }
  
  void update(Player *pl) {
    // spawn a new falling object every 4 turns
    if(turnNo == 4){
      int randomNum = random(0, 8);
      if(clmns[randomNum] == 0){  
        clmns[randomNum] = 1;
      }

      turnNo = 1;
    }

    // animate falling objects
    for(int i = 0; i < 8; ++i){
      // remove objects that went out of display
      if(clmns[i] == 11) { // 8 + 3 dots per tail
        clmns[i] = 0;
      }
      
      // object falls 1 row below
      if(clmns[i] != 0) {
        ++clmns[i];
      }
    }

    ++turnNo;
    lc.clearDisplay(0);
    pl->draw();
  }
  
  void checkCollision(Player *pl) {
    if(clmns[pl->currCol] == 8 || clmns[pl->currCol] == 9 || clmns[pl->currCol] == 10){      
      // remove object
      for(int i = 0; i < 8; ++i){
        clmns[i] = 0;
      }
      
      killPlayer(pl);
    }
  }

  void killPlayer(Player *pl) {
    // check life points
    --pl->numLives;
    if (pl->numLives == 0) {
      gameOver(pl);
    }

    // draw dead animation
    // sides
    lc.setLed(0, 7, pl->currCol+1, true);
    lc.setLed(0, 7, pl->currCol-1, true);
    lc.setLed(0, 7, pl->currCol+2, true);
    lc.setLed(0, 7, pl->currCol-2, true);
    // diagonals
    lc.setLed(0, 6, pl->currCol+1, true);
    lc.setLed(0, 6, pl->currCol-1, true);
    lc.setLed(0, 5, pl->currCol+2, true);
    lc.setLed(0, 5, pl->currCol-2, true);
  }
  
  void gameOver(Player *pl) {
    // reset stats
    isGameOver = true;
    turnNo = 1;
    pl->numLives = 3;
    pl->score = 0;
    
    // draws end-game "animation"
    for (int row = 0; row < 8; ++row) {
      for (int col = 0; col < 8; ++col) {
        lc.setLed(0, row, col, true);
        clmns[col] = 0;
      }
    }
    
    delay(2000);
    lc.clearDisplay(0);
    
    unsigned long diff_1 = millis() - drawFrameStart - 100;
    unsigned long diff_2 = millis() - scoreStart - 100;
    
    drawFrameStart += diff_1;
    scoreStart += diff_2;

    isGameOver = false;
    lcd.clear();
    lcd.print("Highscore:");
  }
};

struct Player playerDot{};
struct FallingObjects fallingObjects{};

void loop() {
  if (millis() - drawFrameStart >= 100) {        
    fallingObjects.update(&playerDot);
    fallingObjects.draw();
    fallingObjects.checkCollision(&playerDot);

    // write on LCD screen
    lcd.setCursor(14, 1);
    lcd.print(playerDot.numLives);
    lcd.setCursor(15, 1);
    lcd.write(byte(0));
    
    drawFrameStart += 100;
  }
  
  if (millis() - scoreStart >= 500) {
    ++playerDot.score;
    lcd.setCursor(0, 1);
    lcd.print(playerDot.score);
    
    scoreStart += 500;
  }
}

// interrupt function to trigger on the yellow button press
void onYellowBtnPress() {
  if (!fallingObjects.isGameOver) {
    playerDot.decrease();
  }
}

// interrupt function to trigger on the red button press
void onRedBtnPress() {
  if (!fallingObjects.isGameOver) {
    playerDot.increase();
  }
}
