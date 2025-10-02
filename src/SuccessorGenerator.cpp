#include "SuccessorGenerator.h"
#include <iostream>

// Direction vectors for movement: up, down, left, right
static const int dr[] = {-1, 1, 0, 0};
static const int dc[] = {0, 0, -1, 1};
static const char moves[] = {'U', 'D', 'L', 'R'};

Array<State> generateSuccessors(const State &s, const Board &board, const Array<Door> &doors, const Meta &meta)
{
  Array<State> successors;

  // Initialize player pathfinder
  PlayerPathfinder pathfinder;
  pathfinder.initialize(board, doors);

  // Block box positions so pathfinder knows player can't walk through them
  Array<int> boxPositions;
  for (int i = 0; i < s.getBoxCount(); i++)
  {
    boxPositions.push_back(s.getBox(i).pos);
  }
  pathfinder.setBlockedPositions(boxPositions);

  // Run BFS from current player position and time
  pathfinder.findPaths(s.getPlayerPos(), s.getStepModL());

  int L = board.getTimeModuloL();
  int rows = board.get_height();
  int cols = board.get_width();

  // For each box, try pushing in each direction
  for (int box_idx = 0; box_idx < s.getBoxCount(); box_idx++)
  {
    const BoxInfo &box = s.getBox(box_idx);
    int box_pos = box.pos;
    char box_id = box.id;

    // Get box coordinates
    int box_row = box_pos / cols;
    int box_col = box_pos % cols;

    // Try each direction
    for (int dir = 0; dir < 4; dir++)
    {
      // Calculate positions
      int player_target_row = box_row - dr[dir]; // Where player needs to be to push
      int player_target_col = box_col - dc[dir];
      int box_dest_row = box_row + dr[dir]; // Where box will end up
      int box_dest_col = box_col + dc[dir];

      // Check bounds for player target position
      if (player_target_row < 0 || player_target_row >= rows ||
          player_target_col < 0 || player_target_col >= cols)
      {
        continue;
      }

      // Check bounds for box destination
      if (box_dest_row < 0 || box_dest_row >= rows ||
          box_dest_col < 0 || box_dest_col >= cols)
      {
        continue;
      }

      int player_target = player_target_row * cols + player_target_col;
      int box_dest = box_dest_row * cols + box_dest_col;

      // Check if player can reach the target position
      int move_distance = pathfinder.getDistance(player_target);
      if (move_distance == -1)
      {
        continue; // Player cannot reach the position to push
      }

      // Check if box destination is valid
      if (board.is_wall_idx(box_dest))
      {
        continue; // Cannot push box into wall
      }

      // Check if another box is already at destination
      bool box_collision = false;
      for (int other_box = 0; other_box < s.getBoxCount(); other_box++)
      {
        if (other_box != box_idx && s.getBox(other_box).pos == box_dest)
        {
          box_collision = true;
          break;
        }
      }
      if (box_collision)
      {
        continue; // Cannot push box into another box
      }

      // Calculate timing for the push
      int total_steps = move_distance + 1; // move to position + push
      int push_time = (s.getStepModL() + total_steps) % L;

      // Check if box destination has a door that must be open at push time
      int door_id = board.get_door_id_idx(box_dest);
      if (door_id > 0)
      { // There is a door at destination
        bool door_open = false;
        for (int door_idx = 0; door_idx < doors.getSize(); door_idx++)
        {
          if (doors[door_idx].id == door_id)
          {
            door_open = doors[door_idx].isOpenAtTime(push_time);
            break;
          }
        }
        if (!door_open)
        {
          continue; // Door is closed at the time of push
        }
      }

      // Check if box is locked and player has the right key
      if (box_id != 0)
      { // Box is labeled (locked)
        // Convert box ID to key requirement (A->0, B->1, etc.)
        int required_key = box_id - 'A';
        if (s.getKeyHeld() != required_key)
        {
          continue; // Player doesn't have the right key
        }
      }

      // Calculate energy cost
      int new_energy = s.getEnergyUsed() + move_distance * meta.moveCost + meta.pushCost;
      if (new_energy > meta.energyLimit)
      {
        continue; // Exceeds energy limit
      }

      // Create successor state
      State new_state = s; // Copy constructor

      // Update player position to box's current position
      new_state.setPlayerPos(box_pos);

      // Update box position
      new_state.getBox(box_idx).pos = box_dest;

      // Update timing
      int new_step_mod_L = (s.getStepModL() + total_steps) % L;
      new_state.setStepModL(new_step_mod_L);

      // Update energy
      new_state.setEnergyUsed(new_energy);

      // Handle key consumption if box was locked
      if (box_id != 0)
      {
        // Unlock the box (set id to 0) and consume the key
        new_state.getBox(box_idx).id = 0;
        new_state.setKeyHeld(-1); // No key held after use
      }

      // Set parent and action (for now, we'll use the push direction)
      // Note: Full action sequence should include walk path + push direction
      new_state.setParent(const_cast<State *>(&s));
      new_state.setActionFromParent(moves[dir]);

      // Canonicalize the state (sort unlabeled boxes)
      new_state.canonicalize();

      // Add to successors
      successors.push_back(new_state);
    }
  }

  return successors;
}