/*
    CONSOLE TETRIS  (Windows command prompt)
    -----------------------------------------
    DSA concepts used, as requested:
      - 2D array             : the game board (grid)
      - 4x4 arrays            : each tetromino shape + its rotations
      - Array traversal       : drawing the board, checking rows
      - Array manipulation    : locking a piece into the board, shifting
                                 rows down when a line is cleared (like
                                 shifting elements in an array after a
                                 deletion)
      - Queue (array-based)   : pieceQueue[] holds the upcoming pieces;
                                 spawnPiece() dequeues the front piece and
                                 enqueues a fresh random one at the rear
                                 (FIFO — first piece generated is the
                                 first piece played)
      - Stack (array-based)   : holdStack[] implements the "Hold" feature
                                 (press C) using push/pop (LIFO)
      - Bubble Sort           : the high-score table is kept sorted in
                                 descending order using bubble sort after
                                 every game
      - Linear search pattern : clearLines() scans each row left to right
                                 looking for an empty cell
      - Simple algorithms     : collision detection, line-clear detection,
                                 score calculation, random piece generator

    Compile (Windows / MinGW):
        g++ -O2 -o tetris.exe tetris.cpp
    Run:
        tetris.exe

    Controls:
        A / D   : move left / right
        S       : soft drop
        W       : rotate
        SPACE   : hard drop
        C       : hold / swap piece (stack push/pop)
        Q       : quit to game-over screen
*/

#include <iostream>
#include <windows.h>   // Sleep, console cursor / color, _kbhit-friendly conio
#include <conio.h>     // _kbhit(), _getch()
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// ---------------------------------------------------------------------
// Board / geometry constants
// ---------------------------------------------------------------------
const int BOARD_W = 12;   // includes side walls
const int BOARD_H = 22;   // includes floor
const int PLAY_W  = BOARD_W - 2;   // playable columns (1..PLAY_W)
const int PLAY_H  = BOARD_H - 1;   // playable rows    (0..PLAY_H-1)

// The board is a 2D array: 0 = empty, 1 = wall/floor, 2..8 = locked piece colors
int board[BOARD_H][BOARD_W];

// ---------------------------------------------------------------------
// Tetromino definitions
// Each piece has 4 rotation states, each state is a 4x4 array of 0/1.
// This is the classic array-of-arrays representation used to teach
// 2D array indexing / rotation algorithms.
// ---------------------------------------------------------------------
struct Piece {
    int shape[4][4][4]; // [rotation][row][col]
    int colorId;
    char symbol;
};

// index 0 = I, 1 = O, 2 = T, 3 = S, 4 = Z, 5 = J, 6 = L
Piece pieces[7];

void initPieces() {
    // I piece
    int I[4][4][4] = {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
        {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}
    };
    // O piece
    int O[4][4][4] = {
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}
    };
    // T piece
    int T[4][4][4] = {
        {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
    };
    // S piece
    int S[4][4][4] = {
        {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
        {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
    };
    // Z piece
    int Z[4][4][4] = {
        {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}
    };
    // J piece
    int J[4][4][4] = {
        {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}
    };
    // L piece
    int L[4][4][4] = {
        {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
        {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}
    };

    int* srcs[7] = { &I[0][0][0], &O[0][0][0], &T[0][0][0], &S[0][0][0],
                     &Z[0][0][0], &J[0][0][0], &L[0][0][0] };
    char syms[7]  = { 'I','O','T','S','Z','J','L' };

    for (int p = 0; p < 7; p++) {
        int* s = srcs[p];
        for (int r = 0; r < 4; r++)
            for (int y = 0; y < 4; y++)
                for (int x = 0; x < 4; x++)
                    pieces[p].shape[r][y][x] = *(s + r*16 + y*4 + x);
        pieces[p].colorId = p + 2; // 2..8, keeps 0/1 reserved for empty/wall
        pieces[p].symbol  = syms[p];
    }
}

// ---------------------------------------------------------------------
// Console helpers
// ---------------------------------------------------------------------
HANDLE hConsole;

void gotoXY(int x, int y) {
    COORD c; c.X = (SHORT)x; c.Y = (SHORT)y;
    SetConsoleCursorPosition(hConsole, c);
}

void setColor(int c) {
    SetConsoleTextAttribute(hConsole, (WORD)c);
}

void hideCursor(bool hide) {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = !hide;
    SetConsoleCursorInfo(hConsole, &info);
}

// ---------------------------------------------------------------------
// Game state
// ---------------------------------------------------------------------
int curPiece, curRotation, curX, curY;
long score;
int linesCleared;
int level;
bool gameOver;

// ---------------------------------------------------------------------
// QUEUE (array-based, FIFO) — holds the upcoming pieces.
// This is the classic circular-queue-using-an-array technique:
// qFront = index of the next item to come OUT
// qRear  = index where the next item goes IN
// qCount = how many items are currently in the queue
// ---------------------------------------------------------------------
const int QUEUE_CAP    = 5;   // array size for the queue
const int PREVIEW_SIZE = 3;   // how many upcoming pieces we show on screen
int pieceQueue[QUEUE_CAP];
int qFront = 0, qRear = 0, qCount = 0;

void enqueuePiece(int p) {
    pieceQueue[qRear] = p;
    qRear = (qRear + 1) % QUEUE_CAP;   // wrap around -> circular queue
    qCount++;
}

int dequeuePiece() {
    int p = pieceQueue[qFront];
    qFront = (qFront + 1) % QUEUE_CAP;
    qCount--;
    return p;
}

void initQueue() {
    qFront = qRear = qCount = 0;
    for (int i = 0; i < PREVIEW_SIZE; i++) enqueuePiece(rand() % 7);
}

// ---------------------------------------------------------------------
// STACK (array-based, LIFO) — used for the "Hold" feature.
// Real Tetris only ever holds one piece at a time, but it is still a
// genuine push/pop stack: pushHold() puts a piece on top, popHold()
// removes and returns the piece currently on top.
// ---------------------------------------------------------------------
const int STACK_CAP = 1;
int holdStack[STACK_CAP];
int holdTop = -1;              // -1 means the stack is empty
bool holdUsedThisTurn = false; // classic Tetris rule: only one hold per piece

bool holdIsEmpty() { return holdTop == -1; }

void pushHold(int p) {
    holdTop = 0;
    holdStack[holdTop] = p;
}

int popHold() {
    int p = holdStack[holdTop];
    holdTop = -1;
    return p;
}

// ---------------------------------------------------------------------
// BUBBLE SORT — keeps the high-score table sorted highest to lowest.
// Classic bubble sort: repeatedly step through the array, compare each
// pair of neighbors, and swap them if they are in the wrong order.
// ---------------------------------------------------------------------
const int MAX_SCORES = 5;
long highScores[MAX_SCORES] = {0, 0, 0, 0, 0};

void bubbleSortScoresDescending() {
    for (int i = 0; i < MAX_SCORES - 1; i++) {
        for (int j = 0; j < MAX_SCORES - 1 - i; j++) {
            if (highScores[j] < highScores[j + 1]) {
                long temp = highScores[j];
                highScores[j] = highScores[j + 1];
                highScores[j + 1] = temp;
            }
        }
    }
}

void insertHighScore(long newScore) {
    // Overwrite the current lowest slot with the new score, then
    // re-sort. If newScore doesn't beat anything, it simply sorts
    // back to the bottom and nothing changes visibly.
    highScores[MAX_SCORES - 1] = newScore;
    bubbleSortScoresDescending();
}

// Initialize the board array: 0 = empty inside, 1 = wall on the border
void initBoard() {
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            if (x == 0 || x == BOARD_W - 1 || y == BOARD_H - 1)
                board[y][x] = 1;   // walls / floor
            else
                board[y][x] = 0;   // empty
        }
    }
}

int randomPiece() { return rand() % 7; }

// Pulls the next piece OFF the front of the queue and adds a fresh
// random piece to the REAR — this is the enqueue/dequeue pattern.
void spawnPiece() {
    curPiece = dequeuePiece();
    enqueuePiece(randomPiece());
    curRotation = 0;
    curX = (PLAY_W / 2) - 1;   // roughly centered
    curY = -1;                 // start slightly above the visible board
    holdUsedThisTurn = false;  // a new piece means hold is usable again

    // If the spawn location is already blocked, the game is over.
    // (Collision check re-used below.)
}

// Swap the falling piece with whatever is in the hold stack (push/pop).
void doHold() {
    if (holdUsedThisTurn) return; // classic rule: one hold per piece
    if (holdIsEmpty()) {
        pushHold(curPiece);
        curPiece = dequeuePiece();
        enqueuePiece(randomPiece());
    } else {
        int temp = curPiece;
        curPiece = popHold();
        pushHold(temp);
    }
    curRotation = 0;
    curX = (PLAY_W / 2) - 1;
    curY = -1;
    holdUsedThisTurn = true;
}

// Collision detection algorithm: for every filled cell of the piece's
// 4x4 shape array, check whether the corresponding board cell is
// occupied or out of bounds.
bool collides(int piece, int rotation, int posX, int posY) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (pieces[piece].shape[rotation][y][x] == 0) continue;
            int bx = posX + x;
            int by = posY + y;
            if (by < 0) continue;               // above the board is fine
            if (bx < 0 || bx >= BOARD_W) return true;
            if (by >= BOARD_H) return true;
            if (board[by][bx] != 0) return true;
        }
    }
    return false;
}

// Lock the current piece into the board array permanently.
void lockPiece() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (pieces[curPiece].shape[curRotation][y][x] == 0) continue;
            int bx = curX + x;
            int by = curY + y;
            if (by >= 0 && by < BOARD_H && bx >= 0 && bx < BOARD_W)
                board[by][bx] = pieces[curPiece].colorId;
        }
    }
}

// Line-clear algorithm:
//   1. Scan every row for a full line (array traversal).
//   2. For each full row found, shift every row above it down by one
//      (classic "delete element from array, shift the rest" pattern),
//      then clear the top row.
int clearLines() {
    int cleared = 0;
    for (int y = 0; y < BOARD_H - 1; y++) {
        bool full = true;
        for (int x = 1; x < BOARD_W - 1; x++) {
            if (board[y][x] == 0) { full = false; break; }
        }
        if (full) {
            cleared++;
            // shift everything above row y down by one row
            for (int yy = y; yy > 0; yy--) {
                for (int x = 1; x < BOARD_W - 1; x++) {
                    board[yy][x] = board[yy - 1][x];
                }
            }
            // clear the new top row
            for (int x = 1; x < BOARD_W - 1; x++) board[0][x] = 0;
        }
    }
    return cleared;
}

void updateScore(int cleared) {
    if (cleared <= 0) return;
    // Classic-style scoring: more lines at once = disproportionately more points
    static const int table[5] = { 0, 40, 100, 300, 1200 };
    score += (long)table[cleared] * (level + 1);
    linesCleared += cleared;
    level = linesCleared / 10;
}

// ---------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------
void drawBoard() {
    gotoXY(0, 0);
    setColor(11);
    cout << "===== CONSOLE TETRIS =====\n";
    setColor(7);

    // Merge locked board + falling piece into one snapshot for drawing
    int snapshot[BOARD_H][BOARD_W];
    for (int y = 0; y < BOARD_H; y++)
        for (int x = 0; x < BOARD_W; x++)
            snapshot[y][x] = board[y][x];

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (pieces[curPiece].shape[curRotation][y][x] == 0) continue;
            int bx = curX + x, by = curY + y;
            if (by >= 0 && by < BOARD_H && bx >= 0 && bx < BOARD_W)
                snapshot[by][bx] = pieces[curPiece].colorId;
        }
    }

    for (int y = 0; y < BOARD_H - 1; y++) {
        cout << ' ';
        for (int x = 0; x < BOARD_W; x++) {
            int v = snapshot[y][x];
            if (v == 0) { setColor(7); cout << "."; }
            else if (v == 1) { setColor(8); cout << "#"; }
            else { setColor(v + 1); cout << "#"; }
        }
        setColor(7);

        // Side panel: score / level / queue preview / hold slot
        if (y == 1) {
            cout << "   NEXT (queue): ";
            for (int i = 0; i < PREVIEW_SIZE; i++) {
                int idx = (qFront + i) % QUEUE_CAP;
                cout << pieces[pieceQueue[idx]].symbol << " ";
            }
        }
        if (y == 2) {
            cout << "   HOLD (stack): "
                 << (holdIsEmpty() ? '-' : pieces[holdStack[0]].symbol);
        }
        if (y == 4) cout << "   SCORE: " << score;
        if (y == 5) cout << "   LEVEL: " << level;
        if (y == 6) cout << "   LINES: " << linesCleared;
        if (y == 8) cout << "   A/D move";
        if (y == 9) cout << "   W rotate";
        if (y == 10) cout << "   S soft drop";
        if (y == 11) cout << "   SPACE hard drop";
        if (y == 12) cout << "   C hold";
        if (y == 13) cout << "   Q quit";

        cout << "\n";
    }
    // bottom wall row
    cout << ' ';
    for (int x = 0; x < BOARD_W; x++) cout << "#";
    cout << "                              \n";
}

// ---------------------------------------------------------------------
// Screens
// ---------------------------------------------------------------------
void clearScreen() {
    system("cls");
}

bool titleScreen() {
    clearScreen();
    gotoXY(0, 0);
    setColor(11);
    cout << "  _____ ______ _______ _____  _____  _____ \n";
    cout << " |_   _|  ____|__   __|  __ \\|_   _|/ ____|\n";
    cout << "   | | | |__     | |  | |__) | | | | (___  \n";
    cout << "   | | |  __|    | |  |  _  /  | |  \\___ \\ \n";
    cout << "  _| |_| |____   | |  | | \\ \\ _| |_ ____) |\n";
    cout << " |_____|______|  |_|  |_|  \\_\\_____|_____/ \n\n";
    setColor(7);
    cout << " A console Tetris written in C++.\n\n";
    cout << " Controls: A/D move   W rotate   S soft drop   SPACE hard drop\n";
    cout << "           C hold (stack)   Q quit\n\n";
    cout << " Press ENTER to start, or ESC to exit...\n";

    while (true) {
        if (_kbhit()) {
            int c = _getch();
            if (c == 13) return true;   // Enter
            if (c == 27) return false;  // Esc
        }
        Sleep(20);
    }
}

// Returns true if the player wants to play again, false to exit.
bool gameOverScreen() {
    clearScreen();
    gotoXY(0, 2);
    setColor(12);
    cout << "  _____          __  __ ______    ______      ________ _____  \n";
    cout << " / ____|   /\\   |  \\/  |  ____|  / __ \\ \\    / /  ____|  __ \\ \n";
    cout << "| |  __   /  \\  | \\  / | |__    | |  | \\ \\  / /| |__  | |__) |\n";
    cout << "| | |_ | / /\\ \\ | |\\/| |  __|   | |  | |\\ \\/ / |  __| |  _  / \n";
    cout << "| |__| |/ ____ \\| |  | | |____  | |__| | \\  /  | |____| | \\ \\ \n";
    cout << " \\_____/_/    \\_\\_|  |_|______|  \\____/   \\/   |______|_|  \\_\\\n";
    setColor(7);
    cout << "\n\n";
    cout << "  Final Score : " << score << "\n";
    cout << "  Lines Cleared : " << linesCleared << "\n";
    cout << "  Level Reached : " << level << "\n\n";

    // Bubble-sort the current score into the high-score table, then
    // display it top to bottom (highest first).
    insertHighScore(score);
    cout << "  --- TOP " << MAX_SCORES << " HIGH SCORES (bubble sorted) ---\n";
    for (int i = 0; i < MAX_SCORES; i++) {
        cout << "   " << (i + 1) << ". " << highScores[i] << "\n";
    }
    cout << "\n  Press R to play again, or Q to quit...\n";

    while (true) {
        if (_kbhit()) {
            int c = _getch();
            c = tolower(c);
            if (c == 'r') return true;
            if (c == 'q') return false;
        }
        Sleep(20);
    }
}

// ---------------------------------------------------------------------
// One full round of the game. Returns when gameOver becomes true.
// ---------------------------------------------------------------------
void runGame() {
    initBoard();
    score = 0;
    linesCleared = 0;
    level = 0;
    gameOver = false;
    holdTop = -1;
    holdUsedThisTurn = false;

    initQueue();
    spawnPiece();

    if (collides(curPiece, curRotation, curX, curY)) {
        gameOver = true;
        return;
    }

    clearScreen();
    hideCursor(true);

    DWORD lastFall = GetTickCount();

    while (!gameOver) {
        // ---- input ----
        if (_kbhit()) {
            int c = _getch();
            c = tolower(c);
            if (c == 'a') {
                if (!collides(curPiece, curRotation, curX - 1, curY)) curX--;
            } else if (c == 'd') {
                if (!collides(curPiece, curRotation, curX + 1, curY)) curX++;
            } else if (c == 's') {
                if (!collides(curPiece, curRotation, curX, curY + 1)) {
                    curY++;
                    score += 1; // small reward for soft drop, like classic Tetris
                }
            } else if (c == 'w') {
                int newRot = (curRotation + 1) % 4;
                if (!collides(curPiece, newRot, curX, curY)) {
                    curRotation = newRot;
                } else if (!collides(curPiece, newRot, curX - 1, curY)) {
                    // simple wall-kick attempt
                    curRotation = newRot; curX--;
                } else if (!collides(curPiece, newRot, curX + 1, curY)) {
                    curRotation = newRot; curX++;
                }
            } else if (c == ' ') {
                // hard drop: keep moving down until it collides
                while (!collides(curPiece, curRotation, curX, curY + 1)) {
                    curY++;
                    score += 2;
                }
            } else if (c == 'c') {
                doHold();   // stack push/pop swap
            } else if (c == 'q') {
                gameOver = true;
                break;
            }
        }

        // ---- gravity ----
        DWORD now = GetTickCount();
        int fallDelay = 500 - level * 40;   // speeds up as level rises
        if (fallDelay < 100) fallDelay = 100;

        if (now - lastFall >= (DWORD)fallDelay) {
            lastFall = now;
            if (!collides(curPiece, curRotation, curX, curY + 1)) {
                curY++;
            } else {
                // piece has landed
                lockPiece();
                int cleared = clearLines();
                updateScore(cleared);
                spawnPiece();
                if (collides(curPiece, curRotation, curX, curY)) {
                    gameOver = true;
                }
            }
        }

        drawBoard();
        Sleep(16); // ~60 fps input polling
    }

    hideCursor(false);
}

// ---------------------------------------------------------------------
int main() {
    srand((unsigned)time(nullptr));
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    initPieces();

    if (!titleScreen()) return 0;

    bool playAgain = true;
    while (playAgain) {
        runGame();
        playAgain = gameOverScreen();
    }

    clearScreen();
    setColor(7);
    cout << "Thanks for playing Console Tetris!\n";
    return 0;
}