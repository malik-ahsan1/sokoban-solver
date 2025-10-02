# Bug Fix: Medium Puzzle Solution Path Reconstruction

## Issue
Medium puzzles were producing invalid solutions. For example, Medium-1 produced a 36-move solution that failed at the 15th move when simulated. The solver claimed to solve the puzzle but the returned move sequence was incomplete.

## Root Cause
The bug was in `src/SolverAStar.cpp` in the `generateSuccessorsWithMoves()` function. When handling key pickup actions (marked with action character 'K'):

**Before the fix:**
```cpp
if (pushDirection == 'K')
{
  // Key pickup action - player just moves to key position
  // No box push involved
  successorNode->g = computeMoveCost(currentState, successors[i]);
  successorNode->h = heuristics.heuristic(successors[i]);
  successorNodes.push_back(successorNode);
  currentStats.nodesGenerated++;
  continue;  // <-- Bug: skips adding moves to actionsFromParent!
}
```

The code was creating successor nodes for key pickup actions but NOT populating the `actionsFromParent` array with the actual move sequence needed to reach the key. This resulted in missing moves in the final solution path.

## The Fix
**After the fix:**
```cpp
if (pushDirection == 'K')
{
  // Key pickup action - player moves to key position
  // Get the path from current player position to the key position
  int keyPos = successors[i].getPlayerPos();
  Array<char> pathMoves = pathfinder.getPath(keyPos);
  
  // Add all path moves to actionsFromParent
  for (int j = 0; j < pathMoves.getSize(); j++)
  {
    successorNode->actionsFromParent.push_back(pathMoves[j]);
  }
  
  successorNode->g = computeMoveCost(currentState, successors[i]);
  successorNode->h = heuristics.heuristic(successors[i]);
  successorNodes.push_back(successorNode);
  currentStats.nodesGenerated++;
  continue;
}
```

Now the code:
1. Gets the key position (where the player moved to)
2. Uses the pathfinder to compute the path from current player position to the key
3. Adds all those moves to `actionsFromParent`

This ensures that the full move sequence is captured and reconstructed correctly.

## Test Results

### Before Fix
- **Medium-1**: 36 moves (INVALID - fails at step 15)
- **Medium-2**: 44 moves (INVALID)

### After Fix
- **Medium-1**: ✓ 50 moves (VALID)
- **Medium-2**: ✓ 85 moves (VALID)
- **Easy-1 to Easy-6**: ✓ All still pass (regression test)

## Why This Matters
In medium puzzles, the player must:
1. Walk to key positions to pick up keys
2. Walk to locked boxes and push them
3. The final solution must include ALL moves

Without this fix, the solution only included the box push moves but omitted the walking moves to collect keys, making the solution incomplete and invalid.

## Technical Details

### How Path Reconstruction Works
1. Each `AStarNode` stores its parent node pointer
2. Each node stores `actionsFromParent` - the move sequence from parent to this node
3. When goal is found, `Simulation::reconstruct_moves()` walks the parent chain backwards
4. It collects all `actionsFromParent` arrays and concatenates them in correct order

### Why Key Pickups Are Special
- Box pushes: Player path + 1 push move
- Key pickups: Just player path (no push)
- Both need the full path sequence stored in `actionsFromParent`

The bug only affected key pickups because the code had a special case for them that returned early without adding the path moves.
