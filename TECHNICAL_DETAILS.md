# Technical Details: The Minimax Algorithm

This document explains how the AI opponent works in the Tic-Tac-Toe game, walking through the minimax algorithm as implemented in [`src/main.cpp`](src/main.cpp).

## Overview

The AI uses **pure minimax without alpha-beta pruning**. It performs an **exhaustive search** of the entire game tree — every possible sequence of moves from the current board state to every possible end state (win, loss, or draw). This is feasible because the Tic-Tac-Toe game tree is very small.

## Why No Alpha-Beta Pruning?

**Alpha-beta pruning** is an optimization that skips branches of the game tree that provably cannot affect the final decision. It can reduce the number of nodes evaluated from $O(b^d)$ to $O(b^{d/2})$ in the best case, where $b$ is the branching factor and $d$ is the depth.

For Tic-Tac-Toe, the full game tree has at most **9! = 362,880** leaf nodes (and far fewer in practice due to early termination on wins). On an ESP32 running at 240 MHz, this evaluates in **under 1 millisecond**. Alpha-beta pruning would make it faster, but there's simply no need — the brute-force search is already instant.

For larger games (Chess, Go, Connect Four, etc.), alpha-beta pruning or other techniques (MCTS, neural networks) become essential.

## The Algorithm Step by Step

### 1. Game State Representation

```
Board: int8_t board[9]     (flat array, row-major)

  board[0] | board[1] | board[2]
  ---------+----------+---------
  board[3] | board[4] | board[5]
  ---------+----------+---------
  board[6] | board[7] | board[8]

Cell values:
   0  = EMPTY
  +1  = PLAYER_X (human)
  -1  = PLAYER_O (AI)
```

The clever use of `+1` and `-1` means a winning line can be detected by summing three cells: sum = +3 means X wins, sum = -3 means O wins.

### 2. The Minimax Function

```cpp
static int minimax(int8_t b[9], bool maximising, int depth) {
    // Check terminal state
    for (int i = 0; i < 8; i++) {
        int8_t s = b[WIN_LINES[i][0]] + b[WIN_LINES[i][1]] + b[WIN_LINES[i][2]];
        if (s ==  3) return  10 - depth;   // X wins (bad for AI)
        if (s == -3) return -10 + depth;   // O wins (good for AI)
    }
    bool full = true;
    for (int i = 0; i < 9; i++) if (b[i] == EMPTY) { full = false; break; }
    if (full) return 0;  // draw

    int best = maximising ? -100 : 100;
    for (int i = 0; i < 9; i++) {
        if (b[i] != EMPTY) continue;
        b[i] = maximising ? PLAYER_X : PLAYER_O;
        int score = minimax(b, !maximising, depth + 1);
        b[i] = EMPTY;  // undo move
        if (maximising) { if (score > best) best = score; }
        else            { if (score < best) best = score; }
    }
    return best;
}
```

The function is recursive with three key parts:

#### Part A — Terminal check (base case)

Check all 8 winning lines. If X has won, return a **positive** score. If O has won, return a **negative** score. If the board is full with no winner, return **0** (draw).

#### Part B — Depth-adjusted scoring

The scores aren't just +10 / -10. They're adjusted by depth:

$$\text{score}_X = 10 - \text{depth}$$
$$\text{score}_O = -10 + \text{depth}$$

This means **faster wins are preferred** and **slower losses are preferred**. Without depth adjustment, the AI might see "I lose no matter what" and play randomly — but with it, the AI will delay losing as long as possible (and choose the fastest win when winning).

**Curiosity — what about the slowest (most convoluted) win?** If we wanted the AI to _delay_ winning as long as possible (maximise showmanship!), we'd flip the depth sign:

$$\text{score}_X = 10 + \text{depth} \qquad \text{score}_O = -10 - \text{depth}$$

This way, a win at depth 7 is valued higher than a win at depth 3 for whichever player is winning, so the algorithm picks the longest winning path.

**Asymmetric styles — one clinical, one theatrical.** We can even mix styles per player! The scoring just needs different depth signs for X and O:

| X Style                  | O Style    | $\text{score}_X$ | $\text{score}_O$ | Effect                                 |
| ------------------------ | ---------- | ---------------- | ---------------- | -------------------------------------- |
| Clinical                 | Clinical   | $10 - d$         | $-10 + d$        | Both win fast (current implementation) |
| Theatrical               | Theatrical | $10 + d$         | $-10 - d$        | Both drag out wins                     |
| Clinical X, Theatrical O | —          | $10 - d$         | $-10 - d$        | X wins fast, O shows off               |
| Theatrical X, Clinical O | —          | $10 + d$         | $-10 + d$        | X shows off, O wins fast               |

The key insight: the depth sign for each player's win score is **independent**. A "clinical" player uses `−depth` (prefers shallow wins), a "theatrical" player uses `+depth` (prefers deep wins).

**Important caveat: we only control the AI.** The table above is slightly misleading — the "X style" isn't something we _choose_ for the real human player. The human can play however they want: optimally, randomly, or change strategy mid-game. What $\text{score}_X$ really controls is what the AI **assumes** about the human in its internal model.

Minimax is a _worst-case_ algorithm: it assumes the opponent plays **optimally against us**. The $\text{score}_X$ formula defines what "optimal" means for the modeled opponent:

- $10 - d$: AI assumes the human will win **as fast as possible** (worst case for AI → AI tries hardest to block)
- $10 + d$: AI assumes the human will **drag out** their win (weaker threat model → AI may play more loosely)

The safest choice — and the one in our implementation — is $10 - d$: assume the human always goes for the fastest kill. This is the game-theoretically optimal assumption because:

1. It prepares the AI for the **strongest possible opponent**
2. Against a weaker opponent, it still plays perfectly (it can't lose either way)
3. The AI's behavior degrades gracefully: even if the human plays suboptimally, the AI still makes the best possible moves

So in practice, the only _real_ style knob we can turn is the **AI's own style** ($\text{score}_O$):

| AI Mode        | $\text{score}_X$                   | $\text{score}_O$     | Behavior              |
| -------------- | ---------------------------------- | -------------------- | --------------------- |
| **Clinical**   | $10 - d$ (assume worst-case human) | $-10 + d$ (win fast) | Efficient, merciless  |
| **Theatrical** | $10 - d$ (assume worst-case human) | $-10 - d$ (win slow) | Showboating, dramatic |

Both are still **unbeatable** — the AI never loses either way. The only difference is whether it finishes you off quickly or toys with you first.

#### Part C — Recursive exploration

For each empty cell:

1. **Make** the move (place X or O)
2. **Recurse** — call minimax for the opponent's turn
3. **Undo** the move (backtrack)
4. **Track** the best score seen so far

The `maximising` flag alternates each level:

- **Maximising** (human/X) picks the move with the **highest** score
- **Minimising** (AI/O) picks the move with the **lowest** score

### 3. The AI Move Selection

```cpp
static void aiMove() {
    int bestIdx   = -1;
    int bestScore = 100;  // AI is minimising
    for (int i = 0; i < 9; i++) {
        if (board[i] != EMPTY) continue;
        board[i] = PLAYER_O;
        int score = minimax(board, true, 0);  // next turn is human (maximising)
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
```

This is the "root" of the search. For each empty cell, the AI:

1. Tentatively places O
2. Calls `minimax()` to evaluate all possible futures from that state (with the human moving next, hence `maximising = true`)
3. Picks the move with the **lowest** score (best for O)

## Visualizing a Partial Game Tree

Consider a board where it's O's turn and there are 3 empty cells:

```
  X | O | X
 ---+---+---
  O | X |         <-- cells 5, 6, 8 are empty
 ---+---+---
    | O |

AI (O) tries each empty cell:

           [Current Board]
          /       |       \
     O→cell5   O→cell6   O→cell8
        |         |         |
     X turn    X turn    X turn
      / \       / \       / \
    ...  ...  ...  ...  ...  ...   ← all possibilities explored
```

Each leaf node returns a score. The scores **bubble up**: X levels take the max, O levels take the min. The root picks the move with the minimum (best-for-O) score.

## Complexity Analysis

| Metric                                | Value                                                |
| ------------------------------------- | ---------------------------------------------------- |
| Max tree depth                        | 9 (all cells filled)                                 |
| Max branching factor                  | 9 (first move) → 8 → 7 → ... → 1                     |
| Max leaf nodes                        | $9! = 362{,}880$                                     |
| Actual nodes (with early termination) | ~$60{,}000$ typical                                  |
| Time on ESP32 (240 MHz)               | < 1 ms                                               |
| Memory                                | $O(d) = O(9)$ stack frames (in-place board mutation) |

## What Alpha-Beta Pruning Would Change

If we added alpha-beta pruning, the function signature would become:

```cpp
static int minimax(int8_t b[9], bool maximising, int depth, int alpha, int beta)
```

Where:

- $\alpha$ = best score the maximiser can guarantee (starts at $-\infty$)
- $\beta$ = best score the minimiser can guarantee (starts at $+\infty$)

During recursion, if $\alpha \geq \beta$, the branch is pruned ("cut off") because:

- The maximiser already has a move at least as good as $\alpha$
- The minimiser already has a move at least as good as $\beta$
- So neither player would allow play to reach this branch

For Tic-Tac-Toe with ~60K nodes, this would reduce evaluation to ~5K–15K nodes. A nice optimization, but unnecessary at this scale.

## Summary

| Property         | This Implementation                     |
| ---------------- | --------------------------------------- |
| Algorithm        | Pure minimax                            |
| Pruning          | None (exhaustive search)                |
| Optimality       | Perfect play — guaranteed               |
| Depth adjustment | Yes — prefers fast wins, delays losses  |
| Board mutation   | In-place (make/undo) — memory efficient |
| Suitable for     | 3×3 Tic-Tac-Toe (tiny game tree)        |

The AI is **provably unbeatable**: the best a perfect human player can achieve is a draw.
