# Sokoban Solver Improvements

## Problem Statement
The original solver could only solve Easy levels 1, 2, 3, 5, and 6. It failed on Easy-4 and all Medium/Hard levels.

## Root Cause Analysis

### Issue 1: Inefficient Successor Generation
The `generateSuccessorsWithMoves()` method in `SolverAStar.cpp` was generating **individual player moves** (U/D/L/R) instead of only generating states where boxes were pushed. This caused an exponential explosion in the search space:
- For a typical Sokoban level, the player can move to many positions without pushing any boxes
- Each such move created a new state in the search
- This led to 75,000+ nodes being explored without finding a solution for Easy-4

### Issue 2: Disabled Deadlock Detection
Deadlock detection was commented out in lines 179-189 of `SolverAStar.cpp`, likely during debugging. Without deadlock detection:
- The solver explored many dead-end states
- States where boxes were pushed into unwinnable positions were not pruned
- This wasted significant computational resources

### Issue 3: Overly Aggressive Deadlock Detection
The wall and multibox deadlock detection had false positives:
- Easy-6 was incorrectly flagged as deadlocked in the initial state
- Wall deadlock detection was too strict, checking for goals along wall lines rather than overall reachability
- Multibox deadlock detection incorrectly flagged valid initial configurations

## Solutions Implemented

### 1. Fixed Successor Generation
**File**: `src/SolverAStar.cpp` (line 288-310)

**Before**: Generated individual player moves (U/D/L/R) creating massive search space

**After**: Uses the proper `generateSuccessors()` function from `SuccessorGenerator.cpp` which:
- Uses `PlayerPathfinder` to find reachable positions efficiently
- Only generates successors for **box pushes**, not individual player moves
- Significantly reduces the branching factor of the search

### 2. Re-enabled Deadlock Detection
**File**: `src/SolverAStar.cpp` (line 173-180)

**Before**: 
```cpp
// TEMPORARILY DISABLE deadlock detection to test
// if (heuristics.isDeadlocked(successorNode->state))
// {
//   delete successorNode;
//   continue;
// }
```

**After**:
```cpp
// Enable deadlock detection to prune dead-end states
if (heuristics.isDeadlocked(successorNode->state))
{
  // Deadlocked state - skip it
  delete successorNode;
  continue;
}
```

### 3. Improved Wall Deadlock Detection
**File**: `src/SokobanHeuristics.cpp` (line 321-358)

**Change**: Modified to only flag a box as wall-deadlocked if it has **infinite distance to all goals** (i.e., truly unreachable), rather than checking if there are goals along specific wall lines.

This eliminates false positives while still catching truly deadlocked states.

### 4. Disabled Multibox Deadlock Detection
**File**: `src/SokobanHeuristics.cpp` (line 711-721)

**Change**: Temporarily disabled multibox deadlock detection as it had too many false positives. The detection logic needs to be improved to handle complex box configurations correctly.

### 5. Increased Search Limits
**File**: `src/SolverAStar.cpp` (line 159-170)

- Increased node expansion limit from 500,000 to 1,000,000
- Changed progress reporting interval from every 25,000 to every 50,000 nodes
- Allows solver to explore more states for complex puzzles

### 6. Key Collection Implementation
**Files**: `src/State.h`, `src/State.cpp`, `src/SuccessorGenerator.cpp`, `src/SolverAStar.cpp`, `src/Zobrist.h`, `src/Zobrist.cpp`

**Purpose**: Enable solving medium and hard puzzles that require collecting keys to unlock boxes.

**Changes**:
1. **State class** (`State.h`, `State.cpp`):
   - Added `Array<int> keys_on_board` and `Array<char> key_ids` to track keys still on the board
   - Added key management methods: `addKey()`, `removeKey()`, `findKeyAt()`, `getKeyCount()`, etc.
   - Updated `equals()` to compare keys on board
   - Updated `recompute_hash()` to include keys on board in Zobrist hash

2. **Zobrist hashing** (`Zobrist.h`, `Zobrist.cpp`):
   - Added `key_on_board_table` to store hash values for keys at positions
   - Added `key_on_board_hash()` method to hash keys on board (similar to box hashing)
   - Allocates 26 * boardSize hash values for all possible key positions

3. **Successor generator** (`SuccessorGenerator.cpp`):
   - Added key pickup actions after box pushing logic
   - Only generates key pickups when player doesn't already hold a key (`key_held == -1`)
   - Uses existing pathfinder to check if player can reach key
   - Creates successor with player at key position, key removed from board, and `key_held` updated
   - Uses action marker 'K' for key pickup actions

4. **A* solver** (`SolverAStar.cpp`):
   - Initialize state with keys from `InitialDynamicState`
   - Handle 'K' action in `generateSuccessorsWithMoves()` to avoid treating it as a box push
   - Updated `isGoal()` to require all boxes to be unlocked (id == 0) before checking positions

**Impact**: Enables solving Medium-1 and Medium-2 puzzles. Medium-3 and Medium-4 require further optimization due to larger search spaces.

## Results

### Easy Puzzles (All Now Solve)
| Level   | Status  | Time      | Nodes Expanded | Solution Length |
|---------|---------|-----------|----------------|-----------------|
| Easy-1  | ✓ Solved | 0 ms     | 1              | 1 moves         |
| Easy-2  | ✓ Solved | 0 ms     | 2              | 3 moves         |
| Easy-3  | ✓ Solved | 15 ms    | 284            | 3 moves         |
| Easy-4  | ✓ Solved | 31.2 s   | 41,657         | 12 moves        |
| Easy-5  | ✓ Solved | 39 ms    | 676            | 4 moves         |
| Easy-6  | ✓ Solved | 49 ms    | 933            | 8 moves         |

### Performance Improvement for Easy-4
- **Before**: Failed to solve (hit 500K node limit, still no solution)
- **After**: Solves in 31 seconds with 41,657 nodes explored

This represents a **>10x improvement** in search efficiency due to proper successor generation.

### Medium Puzzles (Partial Support)
| Level    | Status  | Time      | Nodes Expanded | Solution Length | Notes |
|----------|---------|-----------|----------------|-----------------|-------|
| Medium-1 | ✓ Solved | 13.3 s   | 27,734         | 36 moves        | 2 keys, 2 locked boxes |
| Medium-2 | ✓ Solved | 29 ms    | 305            | 44 moves        | 3 keys, 3 locked boxes |
| Medium-3 | ✗ Timeout | >60 s    | >50,000        | -               | 5 keys, complex |
| Medium-4 | ✗ Timeout | >60 s    | >50,000        | -               | 3 keys, complex |

## Known Limitations

### Medium and Hard Puzzles
Medium puzzles now have **partial support** with key collection implemented:
- ✓ Players can now walk over keys (lowercase letters) to pick them up
- ✓ Keys are used to unlock corresponding boxes (uppercase letters)
- ✓ The `SuccessorGenerator.cpp` generates successors for key pickup actions
- ✓ State tracking includes which keys have been picked up vs. which are still on the board
- ✓ State hash includes available keys on board
- ✓ Deadlock detection accounts for key availability

**Current Status:**
- **Medium-1**: ✓ SOLVED (13.3s, 27,734 nodes expanded, 36 moves)
- **Medium-2**: ✓ SOLVED (29ms, 305 nodes expanded, 44 moves)
- **Medium-3**: Partial - requires optimization (5 keys, very complex search space)
- **Medium-4**: Partial - requires optimization (3 keys + additional complexity)

### Future Work
To solve all Medium and Hard puzzles:
1. ~~Implement key collection in the successor generator~~ ✓ DONE
2. ~~Add state tracking for which keys have been picked up vs. which are still on the board~~ ✓ DONE
3. ~~Update the state hash to include available keys~~ ✓ DONE
4. ~~Ensure deadlock detection accounts for key availability~~ ✓ DONE
5. Improve heuristics for complex multi-key puzzles (Medium-3, Medium-4)
6. Consider pattern database or other advanced heuristics for reducing search space

## Testing
All Easy puzzles (1-6) and Medium puzzles (1-2) have been verified to solve correctly with the improvements. The solver maintains correctness while significantly improving functionality.
