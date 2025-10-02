#include "Simulation.h"
#include <iostream>

void Simulation::reconstruct_moves(AStarNode *goalNode, Array<char> &outMoves)
{
  // Clear output array
  while (outMoves.getSize() > 0)
  {
    outMoves.pop_back();
  }

  if (!goalNode)
    return;

  // Collect all action sequences by walking parent pointers
  Array<Array<char> *> actionSequences;
  AStarNode *current = goalNode;

  while (current && current->parent)
  {
    actionSequences.push_back(&current->actionsFromParent);
    current = reinterpret_cast<AStarNode *>(current->parent);
  }

  // Reverse order and concatenate all actions
  for (int i = actionSequences.getSize() - 1; i >= 0; i--)
  {
    Array<char> *actions = actionSequences[i];
    for (int j = 0; j < actions->getSize(); j++)
    {
      outMoves.push_back((*actions)[j]);
    }
  }
}

bool Simulation::applyPlayerMove(const Board &board, DynamicState &ds, char move,
                                 const Array<Door> &doors, const Meta &meta,
                                 int &energyUsed, int &stepInc)
{
  energyUsed = 0;
  stepInc = 0;

  // Get move deltas
  int dx, dy;
  if (!getMoveDeltas(board, move, dx, dy))
  {
    return false;
  }

  // Get current player position as row/col
  int playerRow = ds.player_pos / board.get_width();
  int playerCol = ds.player_pos % board.get_width();

  // Calculate new position
  int newRow = playerRow + dx;
  int newCol = playerCol + dy;

  // Check bounds
  if (newRow < 0 || newRow >= board.get_height() ||
      newCol < 0 || newCol >= board.get_width())
  {
    return false;
  }

  int newPos = newRow * board.get_width() + newCol;

  // Check if new position is a wall
  if (board.is_wall_idx(newPos))
  {
    return false;
  }

  // Check doors at the new position
  int doorId = board.get_door_id_idx(newPos);
  if (doorId > 0)
  {
    // Find door definition and check if it's open
    for (int i = 0; i < doors.getSize(); i++)
    {
      if (doors[i].id == doorId)
      {
        int nextStep = (ds.step_mod_L + 1) % board.getTimeModuloL();
        if (!doors[i].isOpenAtTime(nextStep))
        {
          return false;
        }
        break;
      }
    }
  }

  // Check if there's a box at the new position
  bool hasBoxAtNewPos = hasBox(ds, newPos);

  if (hasBoxAtNewPos)
  {
    // This is a push move - need to check if box can be pushed
    int boxNewRow = newRow + dx;
    int boxNewCol = newCol + dy;

    // Check box destination bounds
    if (boxNewRow < 0 || boxNewRow >= board.get_height() ||
        boxNewCol < 0 || boxNewCol >= board.get_width())
    {
      return false;
    }

    int boxNewPos = boxNewRow * board.get_width() + boxNewCol;

    // Check if box destination is wall
    if (board.is_wall_idx(boxNewPos))
    {
      return false;
    }

    // Check if there's another box at destination
    if (hasBox(ds, boxNewPos))
    {
      return false;
    }

    // Check doors at box destination
    int boxDoorId = board.get_door_id_idx(boxNewPos);
    if (boxDoorId > 0)
    {
      for (int i = 0; i < doors.getSize(); i++)
      {
        if (doors[i].id == boxDoorId)
        {
          int nextStep = (ds.step_mod_L + 1) % board.getTimeModuloL();
          if (!doors[i].isOpenAtTime(nextStep))
          {
            return false;
          }
          break;
        }
      }
    }

    // Check energy for push move
    int pushEnergy = meta.pushCost;
    if (ds.energy_used + pushEnergy > meta.energyLimit)
    {
      return false;
    }

    // Perform the push
    char boxId = 0;
    if (!removeBox(ds, newPos))
    {
      return false; // Should not happen if hasBox returned true
    }
    addBox(ds, boxNewPos, boxId);

    energyUsed = pushEnergy;
  }
  else
  {
    // Regular move
    int moveEnergy = meta.moveCost;
    if (ds.energy_used + moveEnergy > meta.energyLimit)
    {
      return false;
    }
    energyUsed = moveEnergy;
  }

  // Check if there's a key at the new position
  for (int i = 0; i < ds.keys.getSize(); i++)
  {
    if (ds.keys[i] == newPos && i < ds.keysHeld.getSize() && !ds.keysHeld[i])
    {
      ds.keysHeld[i] = true; // Pick up the key
      break;
    }
  }

  // Apply the move
  ds.player_pos = newPos;
  ds.energy_used += energyUsed;
  ds.step_mod_L = (ds.step_mod_L + 1) % board.getTimeModuloL();
  stepInc = 1;

  return true;
}

void Simulation::pretty_print(const Board &board, const DynamicState &ds)
{
  int width = board.get_width();
  int height = board.get_height();

  for (int row = 0; row < height; row++)
  {
    for (int col = 0; col < width; col++)
    {
      int pos = row * width + col;

      // Check what's at this position
      bool isPlayer = (pos == ds.player_pos);
      bool isUnlockedBox = false;
      bool isLockedBox = false;
      char boxId = 0;
      bool isKey = false;
      char keyId = 0;

      // Check for boxes
      for (int i = 0; i < ds.unlockedBoxes.getSize(); i++)
      {
        if (ds.unlockedBoxes[i] == pos)
        {
          isUnlockedBox = true;
          break;
        }
      }
      for (int i = 0; i < ds.lockedBoxes.getSize(); i++)
      {
        if (ds.lockedBoxes[i] == pos)
        {
          isLockedBox = true;
          if (i < ds.boxIds.getSize())
          {
            boxId = ds.boxIds[i];
          }
          break;
        }
      }

      // Check for keys (only show if not held)
      for (int i = 0; i < ds.keys.getSize(); i++)
      {
        if (ds.keys[i] == pos && i < ds.keysHeld.getSize() && !ds.keysHeld[i])
        {
          isKey = true;
          if (i < ds.keyIds.getSize())
          {
            keyId = ds.keyIds[i];
          }
          break;
        }
      }

      // Determine what to print
      char c = ' ';
      if (board.is_wall_idx(pos))
      {
        c = '#';
      }
      else if (isPlayer)
      {
        if (board.is_target_idx(pos))
        {
          c = '+'; // Player on target
        }
        else
        {
          c = '@'; // Player
        }
      }
      else if (isUnlockedBox || isLockedBox)
      {
        if (board.is_target_idx(pos))
        {
          c = '*'; // Box on target
        }
        else
        {
          c = isLockedBox && boxId ? boxId : '$'; // Labeled or unlabeled box
        }
      }
      else if (isKey)
      {
        c = keyId ? keyId : 'k'; // Key
      }
      else if (board.is_target_idx(pos))
      {
        c = '.'; // Target
      }

      std::cout << c;
    }
    std::cout << std::endl;
  }
}

bool Simulation::simulateMoves(const Board &board, DynamicState &ds,
                               const Array<char> &moves, const Array<Door> &doors,
                               const Meta &meta, bool verbose)
{
  if (verbose)
  {
    std::cout << "Initial state:" << std::endl;
    pretty_print(board, ds);
    std::cout << std::endl;
  }

  for (int i = 0; i < moves.getSize(); i++)
  {
    int energyUsed, stepInc;
    if (!applyPlayerMove(board, ds, moves[i], doors, meta, energyUsed, stepInc))
    {
      if (verbose)
      {
        std::cout << "Invalid move '" << moves[i] << "' at step " << i << std::endl;
      }
      return false;
    }

    if (verbose)
    {
      std::cout << "After move '" << moves[i] << "' (step " << (i + 1) << "):" << std::endl;
      pretty_print(board, ds);
      std::cout << "Energy used: " << energyUsed << ", Total energy: " << ds.energy_used << std::endl;
      std::cout << std::endl;
    }
  }

  return true;
}

// Helper functions
bool Simulation::hasBox(const DynamicState &ds, int pos)
{
  // Check unlocked boxes
  for (int i = 0; i < ds.unlockedBoxes.getSize(); i++)
  {
    if (ds.unlockedBoxes[i] == pos)
    {
      return true;
    }
  }
  // Check locked boxes
  for (int i = 0; i < ds.lockedBoxes.getSize(); i++)
  {
    if (ds.lockedBoxes[i] == pos)
    {
      return true;
    }
  }
  return false;
}

bool Simulation::removeBox(DynamicState &ds, int pos)
{
  // Try to remove from unlocked boxes
  for (int i = 0; i < ds.unlockedBoxes.getSize(); i++)
  {
    if (ds.unlockedBoxes[i] == pos)
    {
      // Remove by swapping with last element and popping
      ds.unlockedBoxes[i] = ds.unlockedBoxes[ds.unlockedBoxes.getSize() - 1];
      ds.unlockedBoxes.pop_back();
      return true;
    }
  }

  // Try to remove from locked boxes
  for (int i = 0; i < ds.lockedBoxes.getSize(); i++)
  {
    if (ds.lockedBoxes[i] == pos)
    {
      // Remove by swapping with last element and popping
      ds.lockedBoxes[i] = ds.lockedBoxes[ds.lockedBoxes.getSize() - 1];
      ds.lockedBoxes.pop_back();
      // Also remove corresponding ID if it exists
      if (i < ds.boxIds.getSize())
      {
        ds.boxIds[i] = ds.boxIds[ds.boxIds.getSize() - 1];
        ds.boxIds.pop_back();
      }
      return true;
    }
  }

  return false;
}

void Simulation::addBox(DynamicState &ds, int pos, char id)
{
  if (id == 0)
  {
    ds.unlockedBoxes.push_back(pos);
  }
  else
  {
    ds.lockedBoxes.push_back(pos);
    ds.boxIds.push_back(id);
  }
}

bool Simulation::getMoveDeltas(const Board &board, char move, int &dx, int &dy)
{
  switch (move)
  {
  case 'U':
    dx = -1;
    dy = 0;
    return true;
  case 'D':
    dx = 1;
    dy = 0;
    return true;
  case 'L':
    dx = 0;
    dy = -1;
    return true;
  case 'R':
    dx = 0;
    dy = 1;
    return true;
  default:
    return false;
  }
}