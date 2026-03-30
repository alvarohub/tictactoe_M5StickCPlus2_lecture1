// Tic-Tac-Toe for M5StickC Plus2
// Controls:
//   BtnA (front)  — move cursor to next empty cell
//   BtnB (side)   — place your mark (X)
//   After game ends, press any button to restart
//
// Human plays X, simple AI plays O.

#include <M5Unified.h>

// --- Game constants ---
enum Cell : int8_t { EMPTY = 0, PLAYER_X = 1, PLAYER_O = -1 };

static int8_t board[9];        // 3x3 board stored flat
static int8_t cursor;          // index 0-8 of highlighted cell
static bool   playerTurn;      // true = human (X), false = AI (O)
static bool   gameOver;
static int8_t winner;          // 0=draw, 1=X wins, -1=O wins

// --- Display layout ---
// Landscape 240x135. Grid on the left 135x135, status on the right.
static const int GRID_X0   = 0;
static const int GRID_Y0   = 0;
static const int CELL_SIZE = 45;  // 135 / 3
static const int STATUS_X  = 145;

// --- Colours ---
static const uint32_t COL_BG       = TFT_BLACK;
static const uint32_t COL_GRID     = TFT_DARKGREY;
static const uint32_t COL_X        = TFT_CYAN;
static const uint32_t COL_O        = TFT_ORANGE;
static const uint32_t COL_CURSOR   = TFT_GREEN;
static const uint32_t COL_WIN_LINE = TFT_YELLOW;
static const uint32_t COL_TEXT     = TFT_WHITE;

// Winning lines: indices of three cells in a row
static const uint8_t WIN_LINES[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},  // rows
    {0,3,6}, {1,4,7}, {2,5,8},  // cols
    {0,4,8}, {2,4,6}            // diagonals
};

// --- Forward declarations ---
static void resetGame();
static void drawBoard();
static void drawCell(int idx, bool highlight);
static void drawX(int cx, int cy, int r, uint32_t col);
static void drawO(int cx, int cy, int r, uint32_t col);
static void drawStatus();
static int8_t checkWinner();
static bool   boardFull();
static void   advanceCursor();
static void   aiMove();
static int    minimax(int8_t b[9], bool maximising, int depth);

// =====================================================================
void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(1);          // landscape 240x135
    M5.Display.fillScreen(COL_BG);
    M5.Display.setTextColor(COL_TEXT, COL_BG);
    M5.Display.setTextSize(1);
    resetGame();
}

void loop() {
    M5.update();

    if (gameOver) {
        // Any button restarts
        if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
            resetGame();
        }
        return;
    }

    if (playerTurn) {
        // --- Human turn (X) ---
        if (M5.BtnA.wasPressed()) {
            // Move cursor to next empty cell
            advanceCursor();
            drawBoard();
        }
        if (M5.BtnB.wasPressed()) {
            if (board[cursor] == EMPTY) {
                board[cursor] = PLAYER_X;
                playerTurn = false;
                winner = checkWinner();
                if (winner != 0 || boardFull()) {
                    gameOver = true;
                }
                drawBoard();
                drawStatus();
            }
        }
    } else {
        // --- AI turn (O) ---
        delay(300);  // small pause so move is visible
        aiMove();
        playerTurn = true;
        winner = checkWinner();
        if (winner != 0 || boardFull()) {
            gameOver = true;
        }
        advanceCursor();  // put cursor on a valid cell for next human turn
        drawBoard();
        drawStatus();
    }

    delay(20);
}

// =====================================================================
// Game logic
// =====================================================================

static void resetGame() {
    memset(board, EMPTY, sizeof(board));
    cursor     = 0;
    playerTurn = true;
    gameOver   = false;
    winner     = 0;
    M5.Display.fillScreen(COL_BG);
    drawBoard();
    drawStatus();
}

static int8_t checkWinner() {
    for (int i = 0; i < 8; i++) {
        int8_t sum = board[WIN_LINES[i][0]]
                   + board[WIN_LINES[i][1]]
                   + board[WIN_LINES[i][2]];
        if (sum ==  3) return PLAYER_X;
        if (sum == -3) return PLAYER_O;
    }
    return 0;
}

static bool boardFull() {
    for (int i = 0; i < 9; i++)
        if (board[i] == EMPTY) return false;
    return true;
}

static void advanceCursor() {
    // Advance cursor to the next empty cell (wrapping)
    for (int i = 1; i <= 9; i++) {
        int idx = (cursor + i) % 9;
        if (board[idx] == EMPTY) {
            cursor = idx;
            return;
        }
    }
}

// --- Minimax AI ---
static int minimax(int8_t b[9], bool maximising, int depth) {
    // Check terminal state
    for (int i = 0; i < 8; i++) {
        int8_t s = b[WIN_LINES[i][0]] + b[WIN_LINES[i][1]] + b[WIN_LINES[i][2]];
        if (s ==  3) return  10 - depth;   // X wins (bad for AI)
        if (s == -3) return -10 + depth;   // O wins (good for AI)
    }
    bool full = true;
    for (int i = 0; i < 9; i++) if (b[i] == EMPTY) { full = false; break; }
    if (full) return 0;

    int best = maximising ? -100 : 100;
    for (int i = 0; i < 9; i++) {
        if (b[i] != EMPTY) continue;
        b[i] = maximising ? PLAYER_X : PLAYER_O;
        int score = minimax(b, !maximising, depth + 1);
        b[i] = EMPTY;
        if (maximising) { if (score > best) best = score; }
        else            { if (score < best) best = score; }
    }
    return best;
}

static void aiMove() {
    int bestIdx   = -1;
    int bestScore = 100;  // AI is minimising
    for (int i = 0; i < 9; i++) {
        if (board[i] != EMPTY) continue;
        board[i] = PLAYER_O;
        int score = minimax(board, true, 0);  // next is human (maximising)
        board[i] = EMPTY;
        if (score < bestScore) {
            bestScore = score;
            bestIdx   = i;
        }
    }
    if (bestIdx >= 0) {
        board[bestIdx] = PLAYER_O;
    }
}

// =====================================================================
// Drawing
// =====================================================================

static void drawBoard() {
    // Grid lines
    for (int i = 1; i < 3; i++) {
        int x = GRID_X0 + i * CELL_SIZE;
        int y = GRID_Y0 + i * CELL_SIZE;
        M5.Display.drawFastVLine(x, GRID_Y0, 3 * CELL_SIZE, COL_GRID);
        M5.Display.drawFastHLine(GRID_X0, y, 3 * CELL_SIZE, COL_GRID);
    }
    // Cells
    for (int i = 0; i < 9; i++) {
        drawCell(i, (i == cursor && !gameOver && playerTurn));
    }

    // If there's a winner, highlight the winning line
    if (winner != 0) {
        for (int i = 0; i < 8; i++) {
            int8_t s = board[WIN_LINES[i][0]]
                     + board[WIN_LINES[i][1]]
                     + board[WIN_LINES[i][2]];
            if (s == 3 || s == -3) {
                int r0 = WIN_LINES[i][0] / 3, c0 = WIN_LINES[i][0] % 3;
                int r2 = WIN_LINES[i][2] / 3, c2 = WIN_LINES[i][2] % 3;
                int x0 = GRID_X0 + c0 * CELL_SIZE + CELL_SIZE / 2;
                int y0 = GRID_Y0 + r0 * CELL_SIZE + CELL_SIZE / 2;
                int x2 = GRID_X0 + c2 * CELL_SIZE + CELL_SIZE / 2;
                int y2 = GRID_Y0 + r2 * CELL_SIZE + CELL_SIZE / 2;
                M5.Display.drawLine(x0, y0, x2, y2, COL_WIN_LINE);
                M5.Display.drawLine(x0+1, y0, x2+1, y2, COL_WIN_LINE);
                M5.Display.drawLine(x0, y0+1, x2, y2+1, COL_WIN_LINE);
                break;
            }
        }
    }
}

static void drawCell(int idx, bool highlight) {
    int row = idx / 3;
    int col = idx % 3;
    int x   = GRID_X0 + col * CELL_SIZE;
    int y   = GRID_Y0 + row * CELL_SIZE;
    int cx  = x + CELL_SIZE / 2;
    int cy  = y + CELL_SIZE / 2;
    int pad = 6;   // padding inside cell
    int r   = CELL_SIZE / 2 - pad;

    // Clear cell interior (leave grid lines)
    M5.Display.fillRect(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, COL_BG);

    // Highlight cursor
    if (highlight) {
        M5.Display.drawRect(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4, COL_CURSOR);
        M5.Display.drawRect(x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6, COL_CURSOR);
    }

    // Draw mark
    if (board[idx] == PLAYER_X) {
        drawX(cx, cy, r, COL_X);
    } else if (board[idx] == PLAYER_O) {
        drawO(cx, cy, r, COL_O);
    }
}

static void drawX(int cx, int cy, int r, uint32_t col) {
    M5.Display.drawLine(cx - r, cy - r, cx + r, cy + r, col);
    M5.Display.drawLine(cx - r + 1, cy - r, cx + r + 1, cy + r, col);
    M5.Display.drawLine(cx + r, cy - r, cx - r, cy + r, col);
    M5.Display.drawLine(cx + r - 1, cy - r, cx - r - 1, cy + r, col);
}

static void drawO(int cx, int cy, int r, uint32_t col) {
    M5.Display.drawCircle(cx, cy, r, col);
    M5.Display.drawCircle(cx, cy, r - 1, col);
}

static void drawStatus() {
    // Clear status area
    M5.Display.fillRect(STATUS_X, 0, 240 - STATUS_X, 135, COL_BG);

    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COL_TEXT, COL_BG);

    if (gameOver) {
        M5.Display.setCursor(STATUS_X, 20);
        if (winner == PLAYER_X) {
            M5.Display.setTextColor(COL_X, COL_BG);
            M5.Display.print("You");
            M5.Display.setCursor(STATUS_X, 40);
            M5.Display.print("win!");
        } else if (winner == PLAYER_O) {
            M5.Display.setTextColor(COL_O, COL_BG);
            M5.Display.print("AI");
            M5.Display.setCursor(STATUS_X, 40);
            M5.Display.print("wins");
        } else {
            M5.Display.setTextColor(COL_TEXT, COL_BG);
            M5.Display.print("Draw");
        }
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(COL_TEXT, COL_BG);
        M5.Display.setCursor(STATUS_X, 80);
        M5.Display.print("Press any");
        M5.Display.setCursor(STATUS_X, 95);
        M5.Display.print("button to");
        M5.Display.setCursor(STATUS_X, 110);
        M5.Display.print("restart");
    } else {
        M5.Display.setCursor(STATUS_X, 10);
        M5.Display.print("Tic");
        M5.Display.setCursor(STATUS_X, 30);
        M5.Display.print("Tac");
        M5.Display.setCursor(STATUS_X, 50);
        M5.Display.print("Toe");

        M5.Display.setTextSize(1);
        M5.Display.setCursor(STATUS_X, 80);
        if (playerTurn) {
            M5.Display.setTextColor(COL_X, COL_BG);
            M5.Display.print("Your turn");
            M5.Display.setCursor(STATUS_X, 100);
            M5.Display.setTextColor(COL_TEXT, COL_BG);
            M5.Display.print("A:move");
            M5.Display.setCursor(STATUS_X, 115);
            M5.Display.print("B:place");
        } else {
            M5.Display.setTextColor(COL_O, COL_BG);
            M5.Display.print("AI thinking");
        }
    }
}
