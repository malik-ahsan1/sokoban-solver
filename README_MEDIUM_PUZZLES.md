# Solving Medium Puzzles with Key Collection

## Quick Start

The Sokoban solver now supports medium-difficulty puzzles that require collecting keys to unlock boxes!

### Running the Solver

```bash
# Build the solver
make sokoban

# Run it
./bin/sokoban
```

### Example: Solving Medium-1

```
1. Load board from file
2. Solve current puzzle
...
Enter your choice: 1
Enter filename: configs/Medium-1.txt

[Board displays with keys 'a' and 'b', and locked boxes 'A' and 'B']

Enter your choice: 2
[Solver runs...]
✓ Solution found!
  Solution length: 36 moves
  Elapsed time: 13297 ms
  Nodes expanded: 27734
```

## Understanding Medium Puzzles

### Board Symbols
- `.` - Goal positions (where boxes need to be)
- `$` - Unlocked box (regular Sokoban box)
- `A`, `B`, `C`, etc. - **Locked boxes** (uppercase letters)
- `a`, `b`, `c`, etc. - **Keys** (lowercase letters)
- `@` - Player
- `#` - Walls

### How Keys Work

1. **Collecting Keys**: When the player walks over a key (`a`, `b`, etc.), it's automatically picked up
2. **Using Keys**: A key unlocks the corresponding box (key `a` unlocks box `A`)
3. **Key Consumption**: When you push a locked box, the key is consumed and the box becomes unlocked
4. **One at a Time**: Player can only hold one key at a time

### Puzzle Strategy

For Medium-1:
```
##############
#   .   .    #
#       B    #      Keys: a, b (lowercase)
#  A        ##      Locked boxes: A, B (uppercase)
####    @   b#      Unlocked box: $ (dollar sign)
# a#  $ #    #      Goals: . (dots)
#           .#
##############
```

Strategy:
1. Collect key `a` to unlock box `A`
2. Collect key `b` to unlock box `B`
3. Push all unlocked boxes to goal positions
4. Only unlocked boxes on goals count toward the solution!

## Results

### Successfully Solved
✅ **Medium-1** - 2 keys, 2 locked boxes + 1 unlocked
- Time: ~13 seconds
- Solution: 36 moves
- Nodes explored: 27,734

✅ **Medium-2** - 3 keys, 3 locked boxes
- Time: ~29 milliseconds
- Solution: 44 moves
- Nodes explored: 305

### Complex Puzzles
⏱️ **Medium-3** - 5 keys, 5 locked boxes (requires optimization)
⏱️ **Medium-4** - 3 keys, 3 locked boxes + 1 unlocked (requires optimization)

## Technical Details

### What Changed

The solver now:
- Tracks which keys are on the board vs. held by player
- Generates "key pickup" actions as valid moves
- Verifies boxes are unlocked before accepting goal state
- Includes key positions in state hashing for correctness

### Implementation

Key files modified:
- `src/State.h/cpp` - State tracking for keys
- `src/SuccessorGenerator.cpp` - Generates key pickup moves
- `src/SolverAStar.cpp` - Handles key actions
- `src/Zobrist.h/cpp` - Hashing for key positions

## Tips for Creating Custom Puzzles

When creating your own medium puzzles:
1. Use lowercase letters (`a-z`) for keys
2. Use matching uppercase letters (`A-Z`) for locked boxes
3. Ensure each locked box has a corresponding key
4. Test complexity - 5+ keys may timeout without optimization

## Known Limitations

- Player can only hold one key at a time
- Complex puzzles (5+ keys) may take a very long time
- Solution path shows 'K' for key pickups in debug mode
- Keys must match their boxes exactly (case-sensitive)

## Next Steps

For even better performance on complex puzzles:
- Implement better heuristics that account for key distances
- Add pattern databases for common key collection patterns
- Consider macro-actions (combine key pickup + box unlock)

Enjoy solving medium puzzles! 🎮🔑
