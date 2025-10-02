#pragma once
#include <string>
#include "Array.h"
#include "Meta.h"
#include "Board.h"
#include "Door.h"

// Forward declaration for dynamic state
struct InitialDynamicState
{
  int player_pos;           // Player position (board index)
  Array<int> unlockedBoxes; // Positions of unlocked boxes
  Array<int> lockedBoxes;   // Positions of locked boxes
  Array<char> boxIds;       // IDs of locked boxes (A-Z)
  Array<int> keys;          // Positions of keys
  Array<char> keyIds;       // IDs of keys (a-z)

  InitialDynamicState() : player_pos(-1) {}
};

class ConfigParser
{
private:
  enum Section
  {
    NONE,
    META,
    DOORS,
    BOARD
  };

  // Helper functions
  std::string trim(const std::string &str);
  Section parseSection(const std::string &line);
  bool parseMeta(const std::string &line, Meta &meta);
  bool parseDoor(const std::string &line, Array<Door> &doors);
  bool parseBoardLine(const std::string &line, int row, Board &board, InitialDynamicState &initState);

  // Validation helpers
  void performValidation(const Meta &meta, const Board &board, const Array<Door> &doors, const InitialDynamicState &initState);

public:
  // Main parsing method
  bool parse(const char *filename, Meta &meta, Board &board, Array<Door> &doors, InitialDynamicState &initState);
};