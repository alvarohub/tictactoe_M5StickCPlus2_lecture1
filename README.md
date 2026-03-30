# Tic-Tac-Toe on M5StickC Plus2

A fully playable Tic-Tac-Toe game running on the **M5StickC Plus2** (ESP32-based), featuring an unbeatable AI opponent powered by the **minimax algorithm**.

Built live during a lecture as a "vibe coding" demo — coded entirely through conversation with GitHub Copilot (Claude) in VS Code.

![M5StickC Plus2](https://static-cdn.m5stack.com/resource/docs/products/core/StickC%20PLUS2/img-1f034239-46c0-4a57-a2a1-6cfcfb62e783.webp)

## Features

- **3×3 grid** rendered on the 240×135 LCD in landscape mode
- **Human vs AI** — you play X, the AI plays O
- **Minimax AI** — perfect play, unbeatable (best you can do is a draw)
- **Cursor navigation** — highlights the currently selected cell in green
- **Win detection** — draws a yellow line through the winning three cells
- **Status panel** — shows game title, whose turn it is, controls, and results
- **One-button restart** — press any button after game over to play again

## Controls

| Button                           | Action                                      |
| -------------------------------- | ------------------------------------------- |
| **BtnA** (front, M5 logo)        | Move cursor to next empty cell              |
| **BtnB** (side)                  | Place your mark (X) in the highlighted cell |
| **Any button** (after game ends) | Restart the game                            |

## Hardware

- **M5StickC Plus2** (ESP32-PICO-V3-02, 240×135 TFT LCD)
- USB-C cable for programming

## Build & Upload

### Prerequisites

- [VS Code](https://code.visualstudio.com/) with [PlatformIO](https://platformio.org/) extension
- USB driver for CH340 serial chip (macOS: usually auto-detected)

### Steps

1. Clone or download this project
2. Open the folder in VS Code
3. Connect M5StickC Plus2 via USB-C
4. Click the PlatformIO **Upload** button (→) or run:
   ```bash
   pio run --target upload
   ```
5. The game starts immediately after upload

### Serial Monitor

```bash
pio device monitor --baud 115200
```

## Project Structure

```
tictactoe/
├── platformio.ini   # PlatformIO config (board, libs)
├── src/
│   └── main.cpp     # Complete game (~335 lines)
├── README.md        # This file
└── JOURNAL.md       # Development log & conversation history
```

## How It Works

### Display Layout (240×135 landscape)

```
┌─────────────────┬──────────┐
│                  │ Tic      │
│   3×3 Grid      │ Tac      │
│   (135×135)      │ Toe      │
│                  │          │
│                  │ Your turn│
│                  │ A:move   │
│                  │ B:place  │
└─────────────────┴──────────┘
  0,0          135,0    240,0
```

### AI (Minimax)

The AI uses a classic **minimax** search with depth tracking:

- Explores all possible future moves recursively
- Minimises its own score (plays as O = -1)
- Returns optimal move every time
- On a 3×3 board, the search space is small enough to run in real-time on ESP32

### Color Scheme

| Element          | Color     |
| ---------------- | --------- |
| Grid lines       | Dark grey |
| X marks          | Cyan      |
| O marks          | Orange    |
| Cursor highlight | Green     |
| Winning line     | Yellow    |
| Text             | White     |
| Background       | Black     |

## Dependencies

- [`M5Unified`](https://github.com/m5stack/M5Unified) ^0.2.2 — unified library for all M5Stack devices

## Context

Created on **30 March 2026** as a live demo for an invited lecture on _"Latest Trends in Physical Computing"_ by **Alvaro Cassinelli**, hosted by **Can Liu**. The project demonstrates "vibe coding" — building a complete embedded application through natural language conversation with an AI coding assistant.

## License

MIT — use it however you like.
