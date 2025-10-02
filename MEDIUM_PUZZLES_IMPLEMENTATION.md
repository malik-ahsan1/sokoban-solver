# Medium Puzzles Implementation - Summary

## Overview
Successfully implemented key collection functionality to enable solving medium-difficulty Sokoban puzzles. The solver can now handle puzzles that require collecting keys to unlock boxes.

## Implementation Details

### Key Changes

1. **State Representation** (`src/State.h`, `src/State.cpp`)
   - Added tracking for keys still on the board: `Array<int> keys_on_board` and `Array<char> key_ids`
   - Added methods: `addKey()`, `removeKey()`, `findKeyAt()`, `getKeyCount()`, `getKeyPos()`, `getKeyId()`
   - Updated equality checking and hash computation to include keys on board

2. **Zobrist Hashing** (`src/Zobrist.h`, `src/Zobrist.cpp`)
   - Added `key_on_board_table` for hashing keys at specific positions
   - New method `key_on_board_hash(char id, int pos)` to hash keys similar to boxes
   - Allocates 26 * boardSize values for all possible key positions (a-z)

3. **Successor Generation** (`src/SuccessorGenerator.cpp`)
   - Added logic to generate key pickup actions when player has no key
   - Uses pathfinder to check if player can reach key
   - Creates successor state with:
     - Player moved to key position
     - Key removed from board
     - `key_held` updated with key index
     - Action marked as 'K' for tracking

4. **A* Solver** (`src/SolverAStar.cpp`)
   - Initialize state with keys from configuration
   - Handle 'K' action type in `generateSuccessorsWithMoves()`
   - Updated goal check: boxes must be unlocked (id == 0) AND on target positions

## Test Results

### Easy Puzzles (Regression Test)
All 6 easy puzzles still solve correctly:
- Easy-1: ✓ PASS
- Easy-2: ✓ PASS  
- Easy-3: ✓ PASS
- Easy-4: ✓ PASS
- Easy-5: ✓ PASS
- Easy-6: ✓ PASS

### Medium Puzzles
| Puzzle | Status | Time | Nodes | Moves | Description |
|--------|--------|------|-------|-------|-------------|
| Medium-1 | ✓ SOLVED | 13.3s | 27,734 | 36 | 2 keys, 2 locked boxes, 1 unlocked box |
| Medium-2 | ✓ SOLVED | 29ms | 305 | 44 | 3 keys, 3 locked boxes |
| Medium-3 | ⏱ Timeout | >60s | >50K | - | 5 keys, 5 locked boxes (very complex) |
| Medium-4 | ⏱ Timeout | >60s | >50K | - | 3 keys, 3 locked boxes + 1 unlocked |

## How It Works

### Key Pickup Flow
1. Player explores the board looking for reachable positions
2. If player has no key (`key_held == -1`), successor generator creates key pickup actions
3. For each key on board, check if player can reach it
4. If reachable, create a new state with:
   - Player at key position
   - Key removed from `keys_on_board`
   - Key stored in `key_held` (as index 0-25 for 'a'-'z')

### Box Unlocking Flow
1. When player has a key and pushes a locked box
2. Check if held key matches box requirement (e.g., key 'a' unlocks box 'A')
3. If match, the push succeeds and:
   - Box ID is changed from letter to 0 (unlocked)
   - `key_held` is set to -1 (key consumed)

### Goal Checking
Goal is reached when:
- All boxes are on target positions (`.` on board)
- All boxes are unlocked (`id == 0`)

This ensures that locked boxes must be unlocked before counting toward the goal.

## Future Improvements

For Medium-3 and Medium-4:
1. **Better Heuristics**: Current heuristic doesn't account for key collection cost
2. **Pattern Databases**: Pre-compute optimal solutions for sub-problems
3. **Macro Actions**: Combine key pickup + box unlock into single actions
4. **Smarter Search**: Prune states where keys are picked up in suboptimal order

## Usage

To test the implementation:
```bash
# Build the solver
make sokoban

# Run interactively
./bin/sokoban

# Then:
# 1. Choose option 1 to load a board
# 2. Enter: configs/Medium-1.txt
# 3. Choose option 2 to solve
```

The solver will automatically handle key collection and box unlocking.

## Technical Notes

- Keys are represented as lowercase letters ('a'-'z')
- Locked boxes are represented as uppercase letters ('A'-'Z')
- A key with letter 'x' unlocks a box with letter 'X'
- Player can only hold one key at a time
- Keys are consumed when used to unlock boxes
- State hash includes both held key and keys still on board
- Action 'K' is used to track key pickup in the solution path
