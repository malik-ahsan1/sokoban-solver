#pragma once
#include "Board.h"
#include "Array.h"
#include "Door.h"
#include "State.h"
#include "ConfigParser.h"
#include "SolverAStar.h"

// Runtime dynamic state for simulation (different from search State)
struct DynamicState
{
  int player_pos;           // Player position (board index)
  Array<int> unlockedBoxes; // Positions of unlocked boxes
  Array<int> lockedBoxes;   // Positions of locked boxes
  Array<char> boxIds;       // IDs of locked boxes (A-Z)
  Array<int> keys;          // Positions of keys on board
  Array<char> keyIds;       // IDs of keys (a-z)
  Array<bool> keysHeld;     // Which keys player is holding
  int energy_used;          // Total energy consumed
  int step_mod_L;           // Current step modulo L

  DynamicState() : player_pos(-1), energy_used(0), step_mod_L(0) {}

  // Create from InitialDynamicState
  DynamicState(const InitialDynamicState &init)
      : player_pos(init.player_pos), energy_used(0), step_mod_L(0)
  {
    // Copy box and key arrays
    for (int i = 0; i < init.unlockedBoxes.getSize(); i++)
    {
      unlockedBoxes.push_back(init.unlockedBoxes[i]);
    }
    for (int i = 0; i < init.lockedBoxes.getSize(); i++)
    {
      lockedBoxes.push_back(init.lockedBoxes[i]);
    }
    for (int i = 0; i < init.boxIds.getSize(); i++)
    {
      boxIds.push_back(init.boxIds[i]);
    }
    for (int i = 0; i < init.keys.getSize(); i++)
    {
      keys.push_back(init.keys[i]);
    }
    for (int i = 0; i < init.keyIds.getSize(); i++)
    {
      keyIds.push_back(init.keyIds[i]);
    }
    // Initialize all keys as not held
    for (int i = 0; i < init.keyIds.getSize(); i++)
    {
      keysHeld.push_back(false);
    }
  }

  // Check if all boxes are on targets
  bool isWin(const Board &board) const
  {
    // Check unlocked boxes
    for (int i = 0; i < unlockedBoxes.getSize(); i++)
    {
      if (!board.is_target_idx(unlockedBoxes[i]))
      {
        return false;
      }
    }
    // Check locked boxes
    for (int i = 0; i < lockedBoxes.getSize(); i++)
    {
      if (!board.is_target_idx(lockedBoxes[i]))
      {
        return false;
      }
    }
    return true;
  }
};

class Simulation
{
public:
  // Path reconstruction from A* solver nodes
  static void reconstruct_moves(AStarNode *goalNode, Array<char> &outMoves);

  // Apply a single player move (U/D/L/R) to the dynamic state
  static bool applyPlayerMove(const Board &board, DynamicState &ds, char move,
                              const Array<Door> &doors, const Meta &meta,
                              int &energyUsed, int &stepInc);

  // Pretty print the current board state with dynamic elements
  static void pretty_print(const Board &board, const DynamicState &ds);

  // Simulate a sequence of moves
  static bool simulateMoves(const Board &board, DynamicState &ds,
                            const Array<char> &moves, const Array<Door> &doors,
                            const Meta &meta, bool verbose = false);

private:
  // Helper: Check if position contains a box
  static bool hasBox(const DynamicState &ds, int pos);

  // Helper: Remove box from position
  static bool removeBox(DynamicState &ds, int pos);

  // Helper: Add box to position
  static void addBox(DynamicState &ds, int pos, char id = 0);

  // Helper: Get move deltas
  static bool getMoveDeltas(const Board &board, char move, int &dx, int &dy);
};