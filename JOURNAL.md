# Tic-Tac-Toe — Development Journal

**Project:** Tic-Tac-Toe on M5StickC Plus2  
**Author:** Alvaro Cassinelli (with GitHub Copilot / Claude)  
**Date:** 30 March 2026  
**Context:** Live "vibe coding" demo during invited lecture on _"Latest Trends in Physical Computing"_ for Can Liu's group

---

## Session 1 — 30 March 2026 (Live during lecture)

### What Happened

The entire project was created live in front of an audience as a demonstration of AI-assisted "vibe coding" for embedded systems. The goal: go from zero to a working game on physical hardware in minutes, using only natural language conversation with GitHub Copilot (Claude, via VS Code agent mode).

### Timeline

1. **Project scaffolding** — PlatformIO project created with `m5stick-c` board config and `M5Unified` library
2. **Full game implementation** — ~335 lines of C++ generated in a single pass:
   - Game logic (board state, win detection, turn management)
   - Minimax AI (perfect play, unbeatable)
   - Display rendering (grid, X/O marks, cursor, status panel, win line)
   - Button input handling (BtnA = navigate, BtnB = place mark)
3. **Serial port check** — identified available ports (`/dev/cu.usbserial-*` and `/dev/cu.wchusbserial*`)
4. **Upload & run** — compiled and uploaded successfully on first attempt
5. **Live demo** — played the game in front of the audience. It worked! The audience applauded.

### Conversation Log

Below is the interaction as it happened in VS Code Copilot chat:

---

**Alvaro:** _(opened platformio.ini — context was already set from a prior conversation that generated the code)_

**Copilot:** I don't have any prior conversation context — this is the start of a new session. Here's what's in your workspace: **Tic-Tac-Toe for M5StickC Plus2** — a complete, working PlatformIO project. It features a 3×3 grid on the left side of the 240×135 landscape display, human plays X (BtnA = move cursor, BtnB = place mark), AI plays O with minimax (unbeatable), status panel on the right. The code looks complete (~335 lines). What would you like to do with it?

**Alvaro:** what serial ports are available?

**Copilot:** _(ran `ls /dev/cu._` and listed all ports)\*  
Available serial ports include:

- `/dev/cu.usbserial-2130` and `/dev/cu.wchusbserial2130` — CH340 USB-serial
- `/dev/cu.usbserial-5A490731851` and `/dev/cu.wchusbserial5A490731851` — another USB-serial
- `/dev/cu.PICO` — Raspberry Pi Pico
- Various Bluetooth devices (earbuds, speakers, etc.)

Your M5StickC Plus2 likely uses a CH340 USB-serial chip.

**Alvaro:** _(uploaded successfully — Exit Code: 0)_

**Alvaro:** I am giving a lecture, and people applaud you!!! it was amazing, it worked!!

**Copilot:** That's awesome — glad it was a hit with the audience! A tic-tac-toe game with minimax AI running on a tiny M5StickC Plus2 is a fun demo. Enjoy the rest of your lecture! 🎉

**Alvaro:** Can you write a good README, also a journal documenting our work, the new ideas, possible improvements as it goes; and do record also the interaction text in this window.

**Copilot:** _(created README.md and this JOURNAL.md)_

---

### Technical Notes

- **Board:** `m5stick-c` in PlatformIO is compatible with M5StickC Plus2 via M5Unified's auto-detection
- **Display:** 240×135 TFT, landscape mode (rotation 1). Grid occupies left 135×135 pixels, status panel on the right
- **AI performance:** Minimax on a 3×3 board is trivial for ESP32 — search completes near-instantly
- **AI delay:** A 300ms artificial delay is added before the AI move so the player can see it happen

### Ideas for Improvement

- [ ] **Difficulty levels** — Add a "dumb" AI mode (random moves) and a medium mode (minimax with limited depth or random mistakes) so the game isn't always a draw
- [ ] **First-move choice** — Let the player choose to go first or second
- [ ] **Score counter** — Track wins/losses/draws across rounds and display on the status panel
- [ ] **Sound effects** — Use the built-in buzzer for move placement, win, and loss sounds
- [ ] **Animations** — Animate X/O drawing (stroke by stroke), cursor blinking, or a winning celebration
- [ ] **Two-player mode** — Pass-and-play using BtnA/BtnB for both players (no AI)
- [ ] **IMU input** — Use the M5StickC's accelerometer/gyroscope for tilt-based cursor movement
- [ ] **BLE remote play** — Two M5StickC devices playing against each other over Bluetooth
- [ ] **Larger boards** — 4×4 or 5×5 variants (would need alpha-beta pruning for minimax)
- [ ] **Visual polish** — Anti-aliased lines, thicker marks, gradient backgrounds, custom font for status text
- [ ] **Power button (BtnC)** — Use the power button press for an additional input (e.g., toggle AI difficulty)

### Reflections

This was a compelling demonstration of AI-assisted embedded development:

- The entire project — from empty folder to working hardware demo — was created through conversation
- No manual coding was required; the human's role was to direct, test, and iterate
- The audience reaction (applause!) validates that this workflow is both impressive and practical
- The "vibe coding" approach works especially well for self-contained embedded projects where the scope is clear

---

_This journal will be updated as the project evolves._
