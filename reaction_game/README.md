# Two-Player Reaction Game

A head-to-head reaction speed game with a synchronized LED countdown and live OLED scoreboard. First to 3 wins (best of 5 rounds).

## Wiring

| Component | GPIO |
|-----------|------|
| Button 1 (Player 1) | 4 |
| Button 2 (Player 2) | 5 |
| Green LED 1 (P1 win) | 6 |
| Green LED 2 (P2 win) | 7 |
| Red LED 1 | 15 |
| Red LED 2 | 16 |
| Red LED 3 | 17 |
| OLED SDA | 8 |
| OLED SCL | 9 |

Buttons wired GPIO → GND (internal pull-ups used, no external resistor needed). LEDs wired GPIO → resistor → LED → GND.

## Libraries

- `Adafruit SSD1306`
- `Adafruit GFX`

## Game flow

1. **Idle** — OLED shows scoreboard (Player 2 left, Player 1 right). Waits for both buttons to be pressed simultaneously to start.
2. **Countdown** — Red LEDs light up one by one (synced with OLED "3-2-1"), then all three light together = "READY".
3. **Random wait** — A randomized 1-3s pause with all reds lit, so players can't anticipate the exact cue. Pressing early triggers a false start (LEDs flash, OLED shows "TOO SOON!", round resets).
4. **Go** — All red LEDs snap off simultaneously and OLED shows "GO!" — this is the reaction cue.
5. **Result** — First button pressed wins the round; their green LED lights, reaction time and a directional arrow (`<<<` / `>>>`) show on OLED.
6. First to 3 round wins (or after 5 rounds) ends the match; OLED shows final result, then resets.

## Flash it

Open `reaction_game.ino` in Arduino IDE, install the libraries above via Library Manager, select Board: `ESP32S3 Dev Module`, upload.
