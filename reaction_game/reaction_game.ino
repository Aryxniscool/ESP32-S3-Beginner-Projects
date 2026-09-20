#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BTN1 4
#define BTN2 5
#define GREEN1 6
#define GREEN2 7
#define RED1 15
#define RED2 16
#define RED3 17

#define WINS_NEEDED 3
#define MAX_ROUNDS 5

enum GameState { IDLE, COUNTDOWN, RANDOM_WAIT, GO, RESULT, GAME_OVER };
GameState state = IDLE;

unsigned long stateTimer = 0;
unsigned long goTime = 0;
int waitDuration = 0;

int score1 = 0, score2 = 0, round_ = 0;
unsigned long reactionTime = 0;

void allRedsOff() {
  digitalWrite(RED1, LOW); digitalWrite(RED2, LOW); digitalWrite(RED3, LOW);
}
void allGreensOff() {
  digitalWrite(GREEN1, LOW); digitalWrite(GREEN2, LOW);
}

// Note: Player 1 is displayed on the RIGHT, Player 2 on the LEFT
void drawIdleScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(8, 0);  display.println("PLAYER 2");
  display.setCursor(78, 0); display.println("PLAYER 1");
  display.drawLine(64, 10, 64, 40, SSD1306_WHITE);

  display.setTextSize(3);
  display.setCursor(20, 15); display.print(score2);
  display.setCursor(90, 15); display.print(score1);

  display.setTextSize(1);
  display.setCursor(15, 50);
  display.print("Round: "); display.print(round_); display.print("/"); display.print(MAX_ROUNDS);
  display.setCursor(5, 58);
  display.print("Press both to start");

  display.display();
}

void drawCountdown(int n) {
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(56, 20);
  display.print(n);
  display.display();
}

void drawReady() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(12, 20);
  display.print("READY");
  display.display();
}

void drawGo() {
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(30, 20);
  display.print("GO!");
  display.display();
}

void drawResult(int winner, unsigned long rt) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.print(rt); display.print(" ms");

  display.setTextSize(2);
  if (winner == 1) {
    display.setCursor(75, 20); display.print(">>>");
    display.setCursor(65, 42); display.print("WIN!");
  } else {
    display.setCursor(0, 20);  display.print("<<<");
    display.setCursor(0, 42);  display.print("WIN!");
  }
  display.display();
}

void drawGameOver(int winner) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 15); display.print("PLAYER "); display.print(winner);
  display.setCursor(20, 35); display.print("WINS!");
  display.setTextSize(1);
  display.setCursor(15, 55);
  display.print(score1); display.print(" - "); display.print(score2);
  display.display();
}

void falseStart() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5, 20);
  display.print("TOO SOON!");
  display.display();
  for (int i = 0; i < 6; i++) {
    digitalWrite(RED1, HIGH); digitalWrite(RED2, HIGH); digitalWrite(RED3, HIGH);
    delay(100);
    allRedsOff();
    delay(100);
  }
  state = IDLE;
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(GREEN1, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  pinMode(RED1, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(RED3, OUTPUT);
  allRedsOff();
  allGreensOff();
  randomSeed(analogRead(0));

  Wire.begin(8, 9); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (1);
  }
  drawIdleScreen();
}

void loop() {
  bool b1 = digitalRead(BTN1) == LOW;
  bool b2 = digitalRead(BTN2) == LOW;

  switch (state) {

    case IDLE:
      allRedsOff();
      allGreensOff();
      if (b1 && b2) {
        delay(30);
        if (digitalRead(BTN1) == LOW && digitalRead(BTN2) == LOW) {
          while (digitalRead(BTN1) == LOW || digitalRead(BTN2) == LOW) delay(10);
          delay(100);
          state = COUNTDOWN;
          stateTimer = millis();
        }
      }
      break;

    case COUNTDOWN: {
      unsigned long elapsed = millis() - stateTimer;
      if (elapsed < 700) {
        digitalWrite(RED1, HIGH);
        if (elapsed < 20) drawCountdown(3);
      } else if (elapsed < 1400) {
        digitalWrite(RED2, HIGH);
        if (elapsed - 700 < 20) drawCountdown(2);
      } else if (elapsed < 2100) {
        digitalWrite(RED3, HIGH);
        if (elapsed - 1400 < 20) drawCountdown(1);
      } else {
        drawReady();
        waitDuration = random(1000, 3000);
        stateTimer = millis();
        state = RANDOM_WAIT;
      }
      if (b1 || b2) falseStart();
      break;
    }

    case RANDOM_WAIT:
      if (b1 || b2) { falseStart(); break; }
      if (millis() - stateTimer >= (unsigned long)waitDuration) {
        allRedsOff();
        drawGo();
        goTime = millis();
        state = GO;
      }
      break;

    case GO:
      if (b1) {
        digitalWrite(GREEN1, HIGH);
        reactionTime = millis() - goTime;
        score1++; round_++;
        drawResult(1, reactionTime);
        stateTimer = millis();
        state = RESULT;
      } else if (b2) {
        digitalWrite(GREEN2, HIGH);
        reactionTime = millis() - goTime;
        score2++; round_++;
        drawResult(2, reactionTime);
        stateTimer = millis();
        state = RESULT;
      }
      break;

    case RESULT:
      if (millis() - stateTimer >= 2500) {
        allGreensOff();
        if (score1 >= WINS_NEEDED || score2 >= WINS_NEEDED || round_ >= MAX_ROUNDS) {
          int winner = (score1 > score2) ? 1 : 2;
          drawGameOver(winner);
          stateTimer = millis();
          state = GAME_OVER;
        } else {
          state = IDLE;
          drawIdleScreen();
        }
      }
      break;

    case GAME_OVER:
      if (millis() - stateTimer >= 4000) {
        score1 = 0; score2 = 0; round_ = 0;
        state = IDLE;
        drawIdleScreen();
      }
      break;
  }
}
