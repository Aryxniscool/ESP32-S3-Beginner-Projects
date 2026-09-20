#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BTN_LEFT 4
#define BTN_RIGHT 5
#define BTN_ROTATE 15
#define BTN_DROP 16
#define GREEN_LED 6
#define RED_LED 17

#define COLS 10
#define ROWS 16
#define CELL 4
// playfield occupies x:0..39, y:0..63

uint8_t board[ROWS][COLS];

// 7 pieces x 4 rotations x 4x4 grid (row-major, 1=filled)
const uint8_t PIECES[7][4][16] = {
  // I
  {{0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0},
   {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0},
   {0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0},
   {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0}},
  // O
  {{0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,1,1,0, 0,1,1,0, 0,0,0,0, 0,0,0,0}},
  // T
  {{0,1,0,0, 1,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,1,0,0, 0,1,1,0, 0,1,0,0, 0,0,0,0},
   {0,0,0,0, 1,1,1,0, 0,1,0,0, 0,0,0,0},
   {0,1,0,0, 1,1,0,0, 0,1,0,0, 0,0,0,0}},
  // S
  {{0,1,1,0, 1,1,0,0, 0,0,0,0, 0,0,0,0},
   {0,1,0,0, 0,1,1,0, 0,0,1,0, 0,0,0,0},
   {0,1,1,0, 1,1,0,0, 0,0,0,0, 0,0,0,0},
   {0,1,0,0, 0,1,1,0, 0,0,1,0, 0,0,0,0}},
  // Z
  {{1,1,0,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,0,1,0, 0,1,1,0, 0,1,0,0, 0,0,0,0},
   {1,1,0,0, 0,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,0,1,0, 0,1,1,0, 0,1,0,0, 0,0,0,0}},
  // J
  {{1,0,0,0, 1,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,1,1,0, 0,1,0,0, 0,1,0,0, 0,0,0,0},
   {0,0,0,0, 1,1,1,0, 0,0,1,0, 0,0,0,0},
   {0,1,0,0, 0,1,0,0, 1,1,0,0, 0,0,0,0}},
  // L
  {{0,0,1,0, 1,1,1,0, 0,0,0,0, 0,0,0,0},
   {0,1,0,0, 0,1,0,0, 0,1,1,0, 0,0,0,0},
   {0,0,0,0, 1,1,1,0, 1,0,0,0, 0,0,0,0},
   {1,1,0,0, 0,1,0,0, 0,1,0,0, 0,0,0,0}}
};

int curPiece, curRot, curX, curY;
unsigned long fallTimer = 0;
int fallDelay = 600;
int score = 0, level = 1, linesCleared = 0;
bool gameOver = false;

bool cellFilled(int piece, int rot, int r, int c) {
  return PIECES[piece][rot][r * 4 + c] == 1;
}

bool checkCollision(int piece, int rot, int x, int y) {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if (cellFilled(piece, rot, r, c)) {
        int bx = x + c;
        int by = y + r;
        if (bx < 0 || bx >= COLS || by >= ROWS) return true;
        if (by >= 0 && board[by][bx]) return true;
      }
    }
  }
  return false;
}

void spawnPiece() {
  curPiece = random(0, 7);
  curRot = 0;
  curX = 3;
  curY = -1;
  if (checkCollision(curPiece, curRot, curX, curY)) {
    gameOver = true;
  }
}

void lockPiece() {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if (cellFilled(curPiece, curRot, r, c)) {
        int by = curY + r, bx = curX + c;
        if (by >= 0) board[by][bx] = 1;
      }
    }
  }
  int cleared = 0;
  for (int r = ROWS - 1; r >= 0; r--) {
    bool full = true;
    for (int c = 0; c < COLS; c++) if (!board[r][c]) full = false;
    if (full) {
      cleared++;
      for (int rr = r; rr > 0; rr--)
        for (int c = 0; c < COLS; c++)
          board[rr][c] = board[rr - 1][c];
      for (int c = 0; c < COLS; c++) board[0][c] = 0;
      r++;
    }
  }
  if (cleared > 0) {
    score += cleared * 100 * level;
    linesCleared += cleared;
    level = 1 + linesCleared / 5;
    fallDelay = max(150, 600 - (level - 1) * 50);
    digitalWrite(GREEN_LED, HIGH);
    delay(80);
    digitalWrite(GREEN_LED, LOW);
  }
  spawnPiece();
}

void hardDrop() {
  while (!checkCollision(curPiece, curRot, curX, curY + 1)) curY++;
  lockPiece();
}

void drawBoard() {
  display.clearDisplay();
  display.drawRect(0, 0, COLS * CELL + 1, ROWS * CELL, SSD1306_WHITE);

  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c < COLS; c++)
      if (board[r][c])
        display.fillRect(c * CELL, r * CELL, CELL - 1, CELL - 1, SSD1306_WHITE);

  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      if (cellFilled(curPiece, curRot, r, c)) {
        int by = curY + r, bx = curX + c;
        if (by >= 0)
          display.fillRect(bx * CELL, by * CELL, CELL - 1, CELL - 1, SSD1306_WHITE);
      }

  display.setTextSize(1);
  display.setCursor(46, 0);
  display.print("SCORE");
  display.setCursor(46, 10);
  display.print(score);
  display.setCursor(46, 25);
  display.print("LEVEL");
  display.setCursor(46, 35);
  display.print(level);
  display.setCursor(46, 50);
  display.print("LINES");
  display.setCursor(46, 60);
  display.print(linesCleared);

  display.display();
}

void drawGameOver() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 15);
  display.print("GAME");
  display.setCursor(10, 35);
  display.print("OVER");
  display.setTextSize(1);
  display.setCursor(10, 55);
  display.print("Score: "); display.print(score);
  display.display();
}

void resetGame() {
  memset(board, 0, sizeof(board));
  score = 0; level = 1; linesCleared = 0;
  fallDelay = 600;
  gameOver = false;
  spawnPiece();
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_ROTATE, INPUT_PULLUP);
  pinMode(BTN_DROP, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  Wire.begin(8, 9);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (1);
  }

  randomSeed(analogRead(0));
  resetGame();
}

bool lastLeft = false, lastRight = false, lastRot = false, lastDrop = false;

void loop() {
  if (gameOver) {
    drawGameOver();
    digitalWrite(RED_LED, HIGH);
    delay(300);
    digitalWrite(RED_LED, LOW);
    delay(300);
    static unsigned long goStart = 0;
    if (goStart == 0) goStart = millis();
    if (millis() - goStart > 3000) {
      goStart = 0;
      resetGame();
    }
    return;
  }

  bool left = digitalRead(BTN_LEFT) == LOW;
  bool right = digitalRead(BTN_RIGHT) == LOW;
  bool rot = digitalRead(BTN_ROTATE) == LOW;
  bool drop = digitalRead(BTN_DROP) == LOW;

  if (left && !lastLeft) {
    if (!checkCollision(curPiece, curRot, curX - 1, curY)) curX--;
  }
  if (right && !lastRight) {
    if (!checkCollision(curPiece, curRot, curX + 1, curY)) curX++;
  }
  if (rot && !lastRot) {
    int newRot = (curRot + 1) % 4;
    if (!checkCollision(curPiece, newRot, curX, curY)) curRot = newRot;
  }
  if (drop && !lastDrop) {
    hardDrop();
  }
  lastLeft = left; lastRight = right; lastRot = rot; lastDrop = drop;

  if (millis() - fallTimer > (unsigned long)fallDelay) {
    fallTimer = millis();
    if (!checkCollision(curPiece, curRot, curX, curY + 1)) {
      curY++;
    } else {
      lockPiece();
    }
  }

  drawBoard();
  delay(20);
}
