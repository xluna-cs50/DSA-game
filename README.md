# DSA-game

# Console-Tetris

A C++ console-based Tetris game demonstrating fundamental Data Structures and Algorithms (DSA) concepts through an interactive gameplay experience. Players control falling tetromino pieces, clear completed lines, manage held pieces, and earn scores while learning how arrays, stacks, queues, searching, sorting, and basic algorithms can be applied in a real game.

## How to Play

Download or open the `tetris.cpp` source code and run it using a C++ compiler or IDE such as Visual Studio, Code::Blocks, or another C++ development environment.

Once the program is executed, the game will launch directly in the Windows Command Prompt/console.

### Controls

- **A / D** — Move left / right
- **W** — Rotate the current piece
- **S** — Soft drop
- **SPACE** — Hard drop
- **C** — Hold / swap the current piece
- **Q** — Quit the game

## DSA Concepts Demonstrated

This project was designed to demonstrate several fundamental DSA concepts:

- **2D Array** — Used to represent and manage the Tetris game board.
- **4×4 Arrays** — Used to store each tetromino shape and its four rotation states.
- **Array Traversal** — Used for drawing the board and checking completed rows.
- **Array Manipulation** — Used to lock pieces into the board and shift rows downward when lines are cleared.
- **Queue (FIFO)** — An array-based circular queue stores upcoming Tetris pieces. The front piece is dequeued when spawned and a new random piece is added to the rear.
- **Stack (LIFO)** — An array-based stack implements the Hold feature, allowing the player to push and pop a held piece.
- **Bubble Sort** — Used to arrange the high-score table from highest to lowest score.
- **Linear Search Pattern** — Used while scanning rows to determine whether cells are empty during line-clear detection.
- **Collision Detection** — Checks whether a falling piece would overlap another piece, a wall, or the floor.
- **Line-Clear Algorithm** — Detects completed rows and shifts the rows above them downward.
- **Score Calculation** — Awards points based on the number of lines cleared and the current level.
- **Random Piece Generation** — Generates new tetromino pieces during gameplay.

## Features

- Classic falling-block Tetris gameplay
- Seven different tetromino pieces: **I, O, T, S, Z, J, L**
- Four rotation states for each piece
- Upcoming piece preview using a queue
- Hold piece functionality using a stack
- Line clearing and automatic row shifting
- Increasing difficulty as the level rises
- Score and line tracking
- Top 5 high-score table
- Bubble-sorted high scores
- Windows console interface
- Play-again functionality

## Running the Game

1. Download or clone this repository.
2. Open the `tetris.cpp` file in a C++ compiler or IDE.
3. Make sure the compiler is configured for Windows, as the game uses Windows console functions.
4. Compile and run the program.
5. The Tetris game will open directly in the console.

> **Note:** This project is designed for Windows because it uses Windows-specific console functionality such as `windows.h` and `conio.h`.

## Purpose

This project combines a playable game with practical DSA implementation, making it easier to understand how fundamental data structures and algorithms can be applied to a real-world interactive program.
