# new_3_bank_display_test

[![Compile](https://github.com/K9DTV/new_3_bank_display_test/actions/workflows/compile.yml/badge.svg)](https://github.com/K9DTV/new_3_bank_display_test/actions/workflows/compile.yml)
[![Status](https://img.shields.io/badge/status-rewrite%20v1.0.0-brightgreen)](https://github.com/K9DTV/new_3_bank_display_test)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Arduino%20Pro%20Mini-red)](https://k9dtv.com/project-3-bank-display.html)
[![Display](https://img.shields.io/badge/display-3x%20PDSP1880-orange)](https://k9dtv.com/project-3-bank-display.html)
[![Website](https://img.shields.io/badge/k9dtv.com-project%20page-0a7ea4)](https://k9dtv.com/project-3-bank-display.html)

**Project page:** https://k9dtv.com/project-3-bank-display.html  
**Sister project (original 2013 sketch):** [3_bank_display_test](https://github.com/K9DTV/3_bank_display_test)

Same hardware as the 2013 breadboard: **OSEPP / Arduino Pro Mini** (ATmega328, 5 V, 16 MHz) driving **three PDSP1880** LED character displays with a pot on **A0** for scroll delay.

This repo is a **structure rewrite** of that sketch. Bus timing, bank selects, charset (`0x00`..`0x7E`), and scroll behavior match the original. Changes:

- Named constants instead of magic hex
- Small helpers for bank select, digit write, and scroll wrap
- Scroll delay range is a real **20..500 ms** (`map()` into `int`; the original used `byte(500)`, which truncated to 244)

Sketch: `new_3_bank_display/new_3_bank_display.ino`

![Breadboard prototype - three PDSP1880 displays, MCU, scroll pot](docs/3-bank-display.jpg)

*Same breadboard / wiring as the original project (photo from the 2013 build).*

## Hardware

| Item | Detail |
|------|--------|
| MCU | OSEPP / Arduino Pro Mini - ATmega328 @ 5 V, 16 MHz |
| Displays | 3x PDSP1880 (left master + two slaves) |
| Scroll control | Potentiometer wiper on **A0** |
| Power | +5 V (`VIN`) and GND |

## Pin map

| Pro Mini | Port | Function |
|----------|------|----------|
| D0..D6 | PD0..PD6 | Display data bits |
| D7 | PD7 | Display WR (active low) |
| D8..D10 | PB0..PB2 | Digit address A0..A2 |
| D11 | PB3 | Bank select 1 (left) |
| D12 | PB4 | Bank select 2 (middle) |
| D13 | PB5 | Bank select 3 (right) |
| A0 | PC0 | Scroll-speed pot wiper |

D0/D1 are data lines, so hardware Serial is not available.

## Schematic

![Wiring schematic - Pro Mini, three PDSP1880 modules, scroll pot](assets/schematic.svg)

Shared data / WR / digit address to all three modules; D11/D12/D13 are individual bank selects.

## Build and upload

1. Open `new_3_bank_display/new_3_bank_display.ino` in the Arduino IDE (no external libraries).
2. Board: **Arduino Pro or Pro Mini**, processor **ATmega328P (5 V, 16 MHz)**.
3. Wire as in the schematic / pin map (same as the original).
4. Upload and turn the pot to adjust scroll speed.

GitHub Actions compiles this sketch on every push to `master` (compile check only).

## License

MIT -- see `LICENSE`.
