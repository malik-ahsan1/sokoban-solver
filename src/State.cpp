#include "State.h"
#include "Zobrist.h"
#include <iostream>

// Default constructor
State::State()
    : player_pos(0), key_held(-1), energy_used(0), step_mod_L(0),
      parent(nullptr), action_from_parent(0), zobrist_hash(0)
{
}

// Parameterized constructor
State::State(int playerPos, int keyHeld, int energyUsed, int stepModL)
    : player_pos(playerPos), key_held(keyHeld), energy_used(energyUsed),
      step_mod_L(stepModL), parent(nullptr), action_from_parent(0), zobrist_hash(0)
{
}

// Copy constructor
State::State(const State &other)
    : player_pos(other.player_pos), boxes(other.boxes), key_held(other.key_held),
      keys_on_board(other.keys_on_board), key_ids(other.key_ids),
      energy_used(other.energy_used), step_mod_L(other.step_mod_L),
      parent(other.parent), action_from_parent(other.action_from_parent),
      zobrist_hash(other.zobrist_hash)
{
}

// Assignment operator
State &State::operator=(const State &other)
{
  if (this != &other)
  {
    player_pos = other.player_pos;
    boxes = other.boxes;
    key_held = other.key_held;
    keys_on_board = other.keys_on_board;
    key_ids = other.key_ids;
    energy_used = other.energy_used;
    step_mod_L = other.step_mod_L;
    parent = other.parent;
    action_from_parent = other.action_from_parent;
    zobrist_hash = other.zobrist_hash;
  }
  return *this;
}

// Canonicalize state by sorting unlabeled boxes by position
void State::canonicalize()
{
  // Simple insertion sort for small arrays (boxes are typically few)
  // Sort boxes: first by ID (unlabeled boxes id=0 come first), then by position
  for (int i = 1; i < boxes.getSize(); i++)
  {
    BoxInfo key = boxes[i];
    int j = i - 1;

    // Move elements that are greater than key one position ahead
    while (j >= 0 && boxes[j].operator<(key) == false && !(key.operator<(boxes[j])))
    {
      // If not less than and not greater than, they're equal - keep order
      if (boxes[j] == key)
        break;
      j--;
    }
    while (j >= 0 && key < boxes[j])
    {
      boxes[j + 1] = boxes[j];
      j--;
    }
    boxes[j + 1] = key;
  }
}

// Full equality check
bool State::equals(const State &other) const
{
  // Check basic fields
  if (player_pos != other.player_pos ||
      key_held != other.key_held ||
      step_mod_L != other.step_mod_L ||
      boxes.getSize() != other.boxes.getSize() ||
      keys_on_board.getSize() != other.keys_on_board.getSize())
  {
    return false;
  }

  // Check all boxes (assumes both states are canonicalized)
  for (int i = 0; i < boxes.getSize(); i++)
  {
    if (!(boxes[i] == other.boxes[i]))
    {
      return false;
    }
  }

  // Check all keys on board
  for (int i = 0; i < keys_on_board.getSize(); i++)
  {
    if (keys_on_board[i] != other.keys_on_board[i] ||
        key_ids[i] != other.key_ids[i])
    {
      return false;
    }
  }

  return true;
}

// Compute and store Zobrist hash
void State::recompute_hash(const Zobrist &Z)
{
  if (!Z.isInitialized())
  {
    std::cerr << "Warning: Zobrist not initialized, hash will be 0" << std::endl;
    zobrist_hash = 0;
    return;
  }

  zobrist_hash = 0;

  // Hash player position
  if (player_pos >= 0 && player_pos < Z.getBoardSize())
  {
    zobrist_hash ^= Z.player_hash(player_pos);
  }

  // Hash boxes
  for (int i = 0; i < boxes.getSize(); i++)
  {
    const BoxInfo &box = boxes[i];
    if (box.pos >= 0 && box.pos < Z.getBoardSize())
    {
      if (box.id == 0)
      {
        // Unlabeled box
        zobrist_hash ^= Z.box_hash_unlabeled(box.pos);
      }
      else if (box.id >= 'A' && box.id <= 'Z')
      {
        // Labeled box
        zobrist_hash ^= Z.box_hash_letter(box.id, box.pos);
      }
    }
  }

  // Hash held key
  if (key_held >= -1) // -1 is valid (no key), positive values are key indices
  {
    zobrist_hash ^= Z.key_hash(key_held + 1); // Shift by 1 to make -1 -> 0, 0 -> 1, etc.
  }

  // Hash keys on board
  for (int i = 0; i < keys_on_board.getSize(); i++)
  {
    int key_pos = keys_on_board[i];
    char key_id = key_ids[i];
    if (key_pos >= 0 && key_pos < Z.getBoardSize() && key_id >= 'a' && key_id <= 'z')
    {
      zobrist_hash ^= Z.key_on_board_hash(key_id, key_pos);
    }
  }

  // Hash step modulo L
  if (step_mod_L >= 0 && step_mod_L < Z.getTimeModuloL())
  {
    zobrist_hash ^= Z.time_hash(step_mod_L);
  }
}

// Box management methods
void State::addBox(int pos, char id)
{
  BoxInfo box(pos, id);
  boxes.push_back(box);
}

void State::removeBox(int index)
{
  if (index >= 0 && index < boxes.getSize())
  {
    // Shift all boxes after index one position left
    for (int i = index; i < boxes.getSize() - 1; i++)
    {
      boxes[i] = boxes[i + 1];
    }
    boxes.pop_back();
  }
}

int State::findBoxAt(int pos) const
{
  for (int i = 0; i < boxes.getSize(); i++)
  {
    if (boxes[i].pos == pos)
    {
      return i;
    }
  }
  return -1; // Not found
}

// Key management methods
void State::addKey(int pos, char id)
{
  keys_on_board.push_back(pos);
  key_ids.push_back(id);
}

void State::removeKey(int index)
{
  if (index >= 0 && index < keys_on_board.getSize())
  {
    // Shift all keys after index one position left
    for (int i = index; i < keys_on_board.getSize() - 1; i++)
    {
      keys_on_board[i] = keys_on_board[i + 1];
      key_ids[i] = key_ids[i + 1];
    }
    keys_on_board.pop_back();
    key_ids.pop_back();
  }
}

int State::findKeyAt(int pos) const
{
  for (int i = 0; i < keys_on_board.getSize(); i++)
  {
    if (keys_on_board[i] == pos)
    {
      return i;
    }
  }
  return -1; // Not found
}