#include <iostream>
#include "../src/ConfigParser.h"

int main()
{
  std::cout << "=== ConfigParser Test ===" << std::endl;

  // Create objects to populate
  Meta meta;
  Board board;
  Array<Door> doors;
  InitialDynamicState initState;

  // Create parser
  ConfigParser parser;

  // Test parsing
  std::cout << "\n--- Parsing Easy-1.txt ---" << std::endl;
  bool success = parser.parse("configs/Easy-1.txt", meta, board, doors, initState);

  if (!success)
  {
    std::cout << "Error: Failed to parse configuration file" << std::endl;
    return 1;
  }

  // Display parsed META information
  std::cout << "\n--- Parsed META Information ---" << std::endl;
  std::cout << "Name: " << meta.name << std::endl;
  std::cout << "Dimensions: " << meta.width << " x " << meta.height << std::endl;
  std::cout << "Energy Limit: " << meta.energyLimit << std::endl;
  std::cout << "Move Cost: " << meta.moveCost << std::endl;
  std::cout << "Push Cost: " << meta.pushCost << std::endl;

  // Display parsed DOORS information
  std::cout << "\n--- Parsed DOORS Information ---" << std::endl;
  std::cout << "Number of doors: " << doors.getSize() << std::endl;
  for (int i = 0; i < doors.getSize(); i++)
  {
    const Door &door = doors[i];
    std::cout << "Door " << door.id << ": OPEN=" << door.openTime
              << " CLOSE=" << door.closeTime << " PHASE=" << door.phase
              << " INITIAL=" << (door.initialOpen ? "1" : "0") << std::endl;
  }

  // Display initial dynamic state
  std::cout << "\n--- Initial Dynamic State ---" << std::endl;
  std::cout << "Player position: " << initState.player_pos;
  if (initState.player_pos >= 0)
  {
    int row = initState.player_pos / meta.width;
    int col = initState.player_pos % meta.width;
    std::cout << " (row " << row << ", col " << col << ")";
  }
  std::cout << std::endl;

  std::cout << "Unlocked boxes: " << initState.unlockedBoxes.getSize() << " positions: ";
  for (int i = 0; i < initState.unlockedBoxes.getSize(); i++)
  {
    std::cout << initState.unlockedBoxes[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Locked boxes: " << initState.lockedBoxes.getSize() << " - ";
  for (int i = 0; i < initState.lockedBoxes.getSize(); i++)
  {
    std::cout << "'" << initState.boxIds[i] << "'@" << initState.lockedBoxes[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Keys: " << initState.keys.getSize() << " - ";
  for (int i = 0; i < initState.keys.getSize(); i++)
  {
    std::cout << "'" << initState.keyIds[i] << "'@" << initState.keys[i] << " ";
  }
  std::cout << std::endl;

  // Display board layout
  std::cout << "\n--- Board Layout ---" << std::endl;
  board.pretty_print();

  // Test door timing at different times
  std::cout << "\n--- Door Timing Test ---" << std::endl;
  for (int t = 0; t < 8; t++)
  {
    std::cout << "Time " << t << ": ";
    for (int i = 0; i < doors.getSize(); i++)
    {
      const Door &door = doors[i];
      bool isOpen = door.isOpenAtTime(t);
      std::cout << "Door" << door.id << "=" << (isOpen ? "O" : "C") << " ";
    }
    std::cout << std::endl;
  }

  // Test with invalid file
  std::cout << "\n--- Error Handling Test ---" << std::endl;
  Meta meta2;
  Board board2;
  Array<Door> doors2;
  InitialDynamicState initState2;

  bool result = parser.parse("nonexistent.cfg", meta2, board2, doors2, initState2);
  std::cout << "Parsing nonexistent file result: " << (result ? "SUCCESS" : "FAILED") << " (expected FAILED)" << std::endl;

  // Test parsing a more complex level with locked boxes and keys
  std::cout << "\n--- Parsing Medium-1.txt (with locked boxes) ---" << std::endl;
  Meta meta3;
  Board board3;
  Array<Door> doors3;
  InitialDynamicState initState3;

  bool success3 = parser.parse("configs/Medium-1.txt", meta3, board3, doors3, initState3);
  if (success3)
  {
    std::cout << "Successfully parsed " << meta3.name << " (" << meta3.width << "x" << meta3.height << ")" << std::endl;
    std::cout << "Unlocked boxes: " << initState3.unlockedBoxes.getSize() << std::endl;
    std::cout << "Locked boxes: " << initState3.lockedBoxes.getSize() << std::endl;
    std::cout << "Keys: " << initState3.keys.getSize() << std::endl;
    std::cout << "Targets: " << board3.num_targets() << std::endl;
  }
  else
  {
    std::cout << "Failed to parse Medium-1.txt" << std::endl;
  }

  return 0;

  return 0;
}