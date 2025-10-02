#pragma once
#include "Array.h"
#include <cstdint>

// Forward declaration
class Zobrist;

// BoxInfo struct for representing boxes with optional IDs
struct BoxInfo
{
  int pos; // Position on the board (index)
  char id; // 0 for unlabeled box, 'A'-'Z' for labeled boxes

  BoxInfo() : pos(-1), id(0) {}
  BoxInfo(int position, char boxId) : pos(position), id(boxId) {}

  // For sorting in canonicalize()
  bool operator<(const BoxInfo &other) const
  {
    if (id != other.id)
      return id < other.id;
    return pos < other.pos;
  }

  bool operator==(const BoxInfo &other) const
  {
    return pos == other.pos && id == other.id;
  }
};

// State class representing a game state
class State
{
private:
  int player_pos;          // Player position on board
  Array<BoxInfo> boxes;    // Array of box positions and IDs
  int key_held;            // -1 for none, or key index/ASCII value
  Array<int> keys_on_board;   // Positions of keys still on the board
  Array<char> key_ids;        // IDs of keys on board (a-z)
  int energy_used;         // Energy consumed so far
  int step_mod_L;          // Current step modulo L (0 to L-1)
  State *parent;           // Parent state (for path reconstruction)
  char action_from_parent; // Action that led to this state ('U','D','L','R' or 0)
  uint64_t zobrist_hash;   // Cached Zobrist hash value

public:
  // Constructors
  State();
  State(int playerPos, int keyHeld, int energyUsed, int stepModL);

  // Destructor
  ~State() = default;

  // Copy constructor and assignment operator
  State(const State &other);
  State &operator=(const State &other);

  // Core methods
  void canonicalize();                           // Sort unlabeled boxes for canonical ordering
  bool equals(const State &other) const;         // Full equality check
  void recompute_hash(const Zobrist &Z);         // Compute and store Zobrist hash
  uint64_t hash() const { return zobrist_hash; } // Get cached hash

  // Accessors
  int getPlayerPos() const { return player_pos; }
  void setPlayerPos(int pos) { player_pos = pos; }

  int getKeyHeld() const { return key_held; }
  void setKeyHeld(int key) { key_held = key; }

  int getEnergyUsed() const { return energy_used; }
  void setEnergyUsed(int energy) { energy_used = energy; }

  int getStepModL() const { return step_mod_L; }
  void setStepModL(int step) { step_mod_L = step; }

  State *getParent() const { return parent; }
  void setParent(State *p) { parent = p; }

  char getActionFromParent() const { return action_from_parent; }
  void setActionFromParent(char action) { action_from_parent = action; }

  // Box management
  void addBox(int pos, char id);
  void removeBox(int index);
  int getBoxCount() const { return boxes.getSize(); }
  const BoxInfo &getBox(int index) const { return boxes[index]; }
  BoxInfo &getBox(int index) { return boxes[index]; }

  // Find box at position
  int findBoxAt(int pos) const;

  // Key management
  void addKey(int pos, char id);
  void removeKey(int index);
  int getKeyCount() const { return keys_on_board.getSize(); }
  int getKeyPos(int index) const { return keys_on_board[index]; }
  char getKeyId(int index) const { return key_ids[index]; }
  int findKeyAt(int pos) const;

  // Step generation for successor states
  void updateStepModL(int timeModuloL) { step_mod_L = (step_mod_L + 1) % timeModuloL; }

  // Hash manipulation (for testing)
  void setZobristHash(uint64_t hash) { zobrist_hash = hash; }
};