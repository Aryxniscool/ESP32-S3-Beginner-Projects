# ESP32-S3 Projects

A collection of small embedded systems projects built on the ESP32-S3, going from basic GPIO control to a full game engine running on an SSD1306 OLED display. Each project builds on the concepts from the last — state machines, debouncing, timing, and display rendering.

## Projects

| # | Project | Description | Hardware |
|---|---------|-------------|----------|
| 1 | [LED Blinker](./led_blinker) | Basic GPIO output control — blinks an external LED | 1x LED, resistor |
| 2 | [Reaction Game](./reaction_game) | Two-player reaction speed game with synchronized countdown | 2x buttons, 2x green LEDs, 3x red LEDs, 0.96" OLED |
| 3 | [Tetris on OLED](./tetris_oled) | Fully playable Tetris clone rendered on a 128x64 monochrome display | 4x buttons, 0.96" OLED, status LEDs |

## Hardware Used

- **MCU:** ESP32-S3 (Dev board)
- **Display:** 0.96" I2C OLED (SSD1306, 128x64)
- **Input:** Tactile push buttons
- **Output:** Red & green 5mm LEDs, 220–330Ω resistors

## Toolchain

- Arduino IDE (Board: `ESP32S3 Dev Module`)
- Libraries: `Adafruit SSD1306`, `Adafruit GFX`

## Setup

1. Clone this repo
2. Open the `.ino` file for the project you want inside Arduino IDE
3. Install the required libraries via Library Manager
4. Wire up according to the pin table in that project's README
5. Select your board + COM port, upload

## Author

Built by [Aryan Khanna](https://github.com/Aryxniscool) — ECE student at Jabalpur Engineering College.

[GitHub](https://github.com/Aryxniscool) · [LinkedIn](https://linkedin.com/in/aryan-khanna-a12a5737a)

## License

MIT — see [LICENSE](./LICENSE)
