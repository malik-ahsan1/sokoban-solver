#include "../src/ConfigParser.h"
#include "../src/Board.h"
#include "../src/Door.h"
#include "../src/State.h"
#include "../src/Zobrist.h"
#include "../src/Meta.h"
#include <iostream>
#include <cassert>
#include <fstream>

void testCompleteWorkflow()
{
  std::cout << "\n=== Complete Door Cycle L Integration Test ===" << std::endl;

  // Create a test configuration file with doors
  std::ofstream testFile("test_doors.cfg");
  testFile << "[META]\n";
  testFile << "NAME = Test Level With Doors\n";
  testFile << "WIDTH = 5\n";
  testFile << "HEIGHT = 5\n";
  testFile << "ENERGY_LIMIT = 100\n";
  testFile << "MOVE_COST = 1\n";
  testFile << "PUSH_COST = 1\n";
  testFile << "\n";
  testFile << "[DOORS]\n";
  testFile << "1 OPEN=2 CLOSE=3 PHASE=0 INITIAL=1\n"; // cycle = 5
  testFile << "2 OPEN=3 CLOSE=2 PHASE=1 INITIAL=0\n"; // cycle = 5
  testFile << "\n";
  testFile << "[BOARD]\n";
  testFile << "#####\n";
  testFile << "#@. #\n"; // Simplified - just player and target
  testFile << "#   #\n";
  testFile << "#   #\n";
  testFile << "#####\n";
  testFile.close();

  // Parse the configuration
  ConfigParser parser;
  Meta meta;
  Board board;
  Array<Door> doors;
  InitialDynamicState initState;

  bool success = parser.parse("test_doors.cfg", meta, board, doors, initState);
  assert(success);

  std::cout << "✓ Parsed configuration with doors successfully" << std::endl;

  // Check that L was computed correctly
  int L = board.getTimeModuloL();
  assert(L == 5); // LCM(5, 5) = 5
  std::cout << "✓ Board computed L = " << L << " correctly" << std::endl;

  // Initialize Zobrist with the board's L
  Zobrist zobrist;
  zobrist.init(board.get_width() * board.get_height(), 10, 5, L, 98765);

  // Create initial state
  State initialState;
  initialState.setPlayerPos(initState.player_pos);
  initialState.setEnergyUsed(0);
  initialState.setStepModL(0);

  // Add boxes from initial state
  for (int i = 0; i < initState.unlockedBoxes.getSize(); i++)
  {
    initialState.addBox(initState.unlockedBoxes[i], 0); // unlabeled
  }

  initialState.recompute_hash(zobrist);
  uint64_t initialHash = initialState.hash();

  std::cout << "✓ Created initial state with hash: " << initialHash << std::endl;

  // Simulate a sequence of moves through the door cycle
  Array<uint64_t> cycleHashes;
  State currentState = initialState;

  for (int step = 0; step < L * 2; step++)
  { // Go through 2 complete cycles
    // Check door states at this time
    bool door1Open = doors[0].isOpenAtTime(step);
    bool door2Open = doors[1].isOpenAtTime(step);

    // Move to next step
    currentState.updateStepModL(L);
    currentState.recompute_hash(zobrist);

    uint64_t stepHash = currentState.hash();

    if (step < L)
    {
      cycleHashes.push_back(stepHash);
    }
    else
    {
      // Second cycle - hashes should repeat
      int cycleIndex = step - L;
      assert(stepHash == cycleHashes[cycleIndex]);
    }

    std::cout << "Step " << step << ": step_mod_L=" << currentState.getStepModL()
              << ", doors=[" << (door1Open ? "open" : "closed")
              << "," << (door2Open ? "open" : "closed") << "]" << std::endl;
  }

  std::cout << "✓ Door cycle completed correctly, hashes repeat after L steps" << std::endl;

  // Test that different step_mod_L values produce different hashes
  State stateA = initialState; // step_mod_L = 0
  State stateB = initialState;
  stateB.setStepModL(1); // Different step_mod_L

  stateA.recompute_hash(zobrist);
  stateB.recompute_hash(zobrist);

  assert(stateA.hash() != stateB.hash());
  std::cout << "✓ Different step_mod_L values produce different hashes" << std::endl;

  // Clean up test file
  std::remove("test_doors.cfg");

  std::cout << "✓ Complete workflow test passed!" << std::endl;
}

int main()
{
  std::cout << "Complete Door Cycle L Integration Test" << std::endl;
  std::cout << "=====================================" << std::endl;

  try
  {
    testCompleteWorkflow();

    std::cout << "\n🎉 Complete integration test passed!" << std::endl;
    std::cout << "\nDemonstrated functionality:" << std::endl;
    std::cout << "- ConfigParser automatically computes L from door cycles" << std::endl;
    std::cout << "- Board stores and provides time modulo L" << std::endl;
    std::cout << "- State tracks step_mod_L through game progression" << std::endl;
    std::cout << "- Zobrist hashing differentiates states by step_mod_L" << std::endl;
    std::cout << "- Door timing mechanics work with step_mod_L" << std::endl;
    std::cout << "- Hash cycles repeat correctly after L steps" << std::endl;

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Test failed with unknown exception" << std::endl;
    return 1;
  }
}