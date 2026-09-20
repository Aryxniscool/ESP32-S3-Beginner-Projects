# LED Blinker

The "hello world" of embedded systems — blinks an external LED on the ESP32-S3, avoiding the built-in LED to confirm raw GPIO output control.

## Wiring

| Component | GPIO |
|-----------|------|
| LED (anode, via resistor) | 4 |

Circuit: `GPIO4 → 220–330Ω resistor → LED anode → LED cathode → GND`

Avoid strapping pins (GPIO0, 3, 45, 46) since they can interfere with boot mode.

## What it does

Toggles the LED on/off every 500ms using `digitalWrite`.

## Flash it

Open `led_blinker.ino` in Arduino IDE, select Board: `ESP32S3 Dev Module`, pick the correct COM port, and upload.
