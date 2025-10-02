#include "ConfigParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <sstream>
#include <cctype>

// Utility function to trim whitespace
std::string ConfigParser::trim(const std::string &str)
{
  size_t first = str.find_first_not_of(" \t\r\n");
  if (first == std::string::npos)
    return "";

  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, (last - first + 1));
}

// Parse section headers
ConfigParser::Section ConfigParser::parseSection(const std::string &line)
{
  std::string trimmed = trim(line);
  if (trimmed == "[META]")
    return META;
  if (trimmed == "[DOORS]")
    return DOORS;
  if (trimmed == "[BOARD]")
    return BOARD;
  return NONE;
}

// Parse META section key=value pairs
bool ConfigParser::parseMeta(const std::string &line, Meta &meta)
{
  size_t equalPos = line.find('=');
  if (equalPos == std::string::npos)
    return false;

  std::string key = trim(line.substr(0, equalPos));
  std::string value = trim(line.substr(equalPos + 1));

  if (key == "NAME")
  {
    meta.name = value;
  }
  else if (key == "WIDTH")
  {
    meta.width = std::atoi(value.c_str());
  }
  else if (key == "HEIGHT")
  {
    meta.height = std::atoi(value.c_str());
  }
  else if (key == "ENERGY_LIMIT")
  {
    meta.energyLimit = std::atoi(value.c_str());
  }
  else if (key == "MOVE_COST")
  {
    meta.moveCost = std::atoi(value.c_str());
  }
  else if (key == "PUSH_COST")
  {
    meta.pushCost = std::atoi(value.c_str());
  }
  else
  {
    std::cout << "Warning: Unknown META key: " << key << std::endl;
    return false;
  }

  return true;
}

// Parse DOORS section - format: "id OPEN=k CLOSE=l PHASE=p INITIAL=0/1"
bool ConfigParser::parseDoor(const std::string &line, Array<Door> &doors)
{
  std::istringstream iss(line);
  std::string token;

  // Parse door ID
  if (!(iss >> token))
    return false;
  int doorId = std::atoi(token.c_str());

  // Initialize door with defaults
  int openTime = 0, closeTime = 0, phase = 0;
  bool initialOpen = false;

  // Parse key=value pairs
  while (iss >> token)
  {
    size_t equalPos = token.find('=');
    if (equalPos == std::string::npos)
      continue;

    std::string key = token.substr(0, equalPos);
    std::string value = token.substr(equalPos + 1);

    if (key == "OPEN")
    {
      openTime = std::atoi(value.c_str());
    }
    else if (key == "CLOSE")
    {
      closeTime = std::atoi(value.c_str());
    }
    else if (key == "PHASE")
    {
      phase = std::atoi(value.c_str());
    }
    else if (key == "INITIAL")
    {
      initialOpen = (std::atoi(value.c_str()) == 1);
    }
  }

  // Create and add door
  Door door(doorId, openTime, closeTime, phase, initialOpen);
  doors.push_back(door);

  return true;
}

// Parse BOARD section line
bool ConfigParser::parseBoardLine(const std::string &line, int row, Board &board, InitialDynamicState &initState)
{
  for (int col = 0; col < (int)line.length(); col++)
  {
    char ch = line[col];
    int index = row * board.get_width() + col;

    switch (ch)
    {
    case '#': // Wall
      board.set_wall(row, col);
      break;

    case '.': // Target
      board.set_target(row, col);
      break;

    case '@': // Player
      initState.player_pos = index;
      break;

    case '$': // Unlocked box
      initState.unlockedBoxes.push_back(index);
      break;

    case '*': // Target + unlocked box
      board.set_target(row, col);
      initState.unlockedBoxes.push_back(index);
      break;

    case ' ': // Floor (nothing special to do)
      break;

    default:
      if (ch >= 'A' && ch <= 'Z') // Locked box
      {
        initState.lockedBoxes.push_back(index);
        initState.boxIds.push_back(ch);
      }
      else if (ch >= 'a' && ch <= 'z') // Key
      {
        initState.keys.push_back(index);
        initState.keyIds.push_back(ch);
      }
      else if (ch >= '1' && ch <= '9') // Door cell
      {
        int doorId = ch - '0';
        board.set_door(row, col, doorId);
      }
      else
      {
        std::cout << "Warning: Unknown character '" << ch << "' at (" << row << "," << col << ")" << std::endl;
      }
      break;
    }
  }

  return true;
}

// Perform validation checks
void ConfigParser::performValidation(const Meta &meta, const Board &board, const Array<Door> &doors, const InitialDynamicState &initState)
{
  // Check box count vs target count
  int totalBoxes = initState.unlockedBoxes.getSize() + initState.lockedBoxes.getSize();
  int targetCount = board.num_targets();

  if (totalBoxes != targetCount)
  {
    std::cout << "Warning: Box count (" << totalBoxes << ") ≠ target count (" << targetCount << ")" << std::endl;
  }

  // Verify locked boxes have matching keys
  for (int i = 0; i < initState.lockedBoxes.getSize(); i++)
  {
    char boxId = initState.boxIds[i];
    char expectedKeyId = std::tolower(boxId);

    bool keyFound = false;
    for (int j = 0; j < initState.keys.getSize(); j++)
    {
      if (initState.keyIds[j] == expectedKeyId)
      {
        keyFound = true;
        break;
      }
    }

    if (!keyFound)
    {
      std::cout << "Warning: Locked box '" << boxId << "' has no matching key '" << expectedKeyId << "'" << std::endl;
    }
  }

  // Check door configurations
  for (int i = 0; i < doors.getSize(); i++)
  {
    const Door &door = doors[i];
    if (door.openTime == 0 && door.closeTime > 0)
    {
      std::cout << "Info: Door " << door.id << " is always closed (OPEN=0)" << std::endl;
    }
    else if (door.closeTime == 0 && door.openTime > 0)
    {
      std::cout << "Info: Door " << door.id << " is always open (CLOSE=0)" << std::endl;
    }
  }

  // Validate board dimensions
  std::cout << "Info: Parsed level '" << meta.name << "' (" << meta.width << "x" << meta.height << ")" << std::endl;
  std::cout << "Info: Energy limit: " << meta.energyLimit << ", Move cost: " << meta.moveCost << ", Push cost: " << meta.pushCost << std::endl;
  std::cout << "Info: Found " << doors.getSize() << " doors, " << totalBoxes << " boxes, " << targetCount << " targets" << std::endl;
}

// Main parsing method
bool ConfigParser::parse(const char *filename, Meta &meta, Board &board, Array<Door> &doors, InitialDynamicState &initState)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::cout << "Error: Cannot open file " << filename << std::endl;
    return false;
  }

  std::string line;
  Section currentSection = NONE;
  int boardRow = 0;
  bool metaParsed = false;
  bool boardInitialized = false;

  while (std::getline(file, line))
  {
    // Trim whitespace
    line = trim(line);

    // Skip empty lines
    if (line.empty())
      continue;

    // Check for section headers
    Section newSection = parseSection(line);
    if (newSection != NONE)
    {
      currentSection = newSection;
      if (currentSection == BOARD && metaParsed && !boardInitialized)
      {
        // Initialize board with dimensions from META
        board.initialize(meta.width, meta.height);
        boardInitialized = true;
        boardRow = 0;
      }
      continue;
    }

    // Parse content based on current section
    switch (currentSection)
    {
    case META:
      if (parseMeta(line, meta))
      {
        metaParsed = true;
      }
      break;

    case DOORS:
      parseDoor(line, doors);
      break;

    case BOARD:
      if (!boardInitialized)
      {
        std::cout << "Error: BOARD section found before META section" << std::endl;
        return false;
      }

      if (boardRow >= meta.height)
      {
        std::cout << "Warning: More board lines than HEIGHT specified" << std::endl;
        break;
      }

      if ((int)line.length() != meta.width)
      {
        std::cout << "Warning: Board line " << boardRow << " length (" << line.length()
                  << ") ≠ WIDTH (" << meta.width << ")" << std::endl;
      }

      parseBoardLine(line, boardRow, board, initState);
      boardRow++;
      break;

    case NONE:
      std::cout << "Warning: Content outside of sections: " << line << std::endl;
      break;
    }
  }

  file.close();

  // Validation
  if (!metaParsed)
  {
    std::cout << "Error: No META section found" << std::endl;
    return false;
  }

  if (boardRow != meta.height)
  {
    std::cout << "Warning: Expected " << meta.height << " board lines, got " << boardRow << std::endl;
  }

  if (initState.player_pos == -1)
  {
    std::cout << "Warning: No player position (@) found in board" << std::endl;
  }

  // Compute time modulo L for door cycles
  board.computeTimeModuloL(doors);

  // Perform additional validation
  performValidation(meta, board, doors, initState);

  return true;
}
