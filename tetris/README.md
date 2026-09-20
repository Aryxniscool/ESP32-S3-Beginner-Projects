# Tetris on 0.96" OLED

A playable Tetris clone rendered on a 128x64 monochrome SSD1306 display, controlled with 4 buttons.

## Wiring

| Component | GPIO |
|-----------|------|
| Button LEFT | 4 |
| Button RIGHT | 5 |
| Button ROTATE | 15 |
| Button DROP (hard drop) | 16 |
| Green LED (line clear) | 6 |
| Red LED (game over) | 17 |
| OLED SDA | 8 |
| OLED SCL | 9 |

Buttons wired GPIO → GND (internal pull-ups). LEDs wired GPIO → resistor → LED → GND.

## Libraries

- `Adafruit SSD1306`
- `Adafruit GFX`

## Features

- 10x16 playfield, all 7 standard tetrominoes (I, O, T, S, Z, J, L)
- Basic rotation (no wall-kicks — kept intentionally simple)
- Hard drop
- Score, level, and lines-cleared counter on-screen
- Speed increases automatically every 5 lines cleared
- Green LED flashes on line clear, red LED blinks on game over
- Auto-restarts a few seconds after game over

## Controls

- **LEFT / RIGHT** — move piece
- **ROTATE** — cycle rotation state
- **DROP** — instantly hard-drop the current piece

## Flash it

Open `tetris_oled.ino` in Arduino IDE, install the libraries above via Library Manager, select Board: `ESP32S3 Dev Module`, upload.

## Possible extensions

- Auto-repeat movement on button hold
- Wall-kick rotation (SRS-style)
- "Hold piece" feature using a spare button
- Next-piece preview
