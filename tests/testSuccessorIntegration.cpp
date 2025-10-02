#include "SuccessorGenerator.h"
#include "ConfigParser.h"
#include "Zobrist.h"
#include <iostream>
#include <cassert>
#include <fstream>

void test_integration_workflow()
{
  std::cout << "\n=== Testing Complete Integration Workflow ===\n";

  // Create a simple level configuration file
  std::string level_config =
      "[META]\n"
      "NAME = Integration Test\n"
      "WIDTH = 4\n"
      "HEIGHT = 4\n"
      "ENERGY_LIMIT = 50\n"
      "MOVE_COST = 1\n"
      "PUSH_COST = 3\n"
      "\n"
      "[DOORS]\n"
      "\n"
      "[BOARD]\n"
      "####\n"
      "#@.#\n"
      "#B.#\n"
      "####\n";

  // Write to temporary file
  std::ofstream temp_file("/tmp/test_config.txt");
  temp_file << level_config;
  temp_file.close();

  // Parse the configuration
  ConfigParser parser;
  Meta meta;
  Board board;
  Array<Door> doors;
  InitialDynamicState init_state;

  bool parse_success = parser.parse("/tmp/test_config.txt", meta, board, doors, init_state);
  assert(parse_success);

  Zobrist zobrist;

  std::cout << "Parsed level: " << meta.name << "\n";
  std::cout << "Board size: " << board.get_width() << "x" << board.get_height() << "\n";
  std::cout << "Energy limit: " << meta.energyLimit << "\n";

  // Initialize Zobrist hashing
  int L = board.getTimeModuloL();
  zobrist.init(board.get_width() * board.get_height(), 26, 10, L);

  // Create initial state using parsed data
  State initial_state(init_state.player_pos, -1, 0, 0);

  // Add unlocked boxes from parsed data
  for (int i = 0; i < init_state.unlockedBoxes.getSize(); i++)
  {
    initial_state.addBox(init_state.unlockedBoxes[i], 0);
  }

  initial_state.recompute_hash(zobrist);

  std::cout << "Initial state:\n";
  std::cout << "  Player at position: " << initial_state.getPlayerPos() << "\n";
  if (initial_state.getBoxCount() > 0)
  {
    std::cout << "  Box at position: " << initial_state.getBox(0).pos << "\n";
  }
  std::cout << "  Energy used: " << initial_state.getEnergyUsed() << "\n";

  // Generate successors
  Array<State> successors = generateSuccessors(initial_state, board, doors, meta);

  std::cout << "Generated " << successors.getSize() << " successor states:\n";

  for (int i = 0; i < successors.getSize(); i++)
  {
    const State &succ = successors[i];
    std::cout << "  Successor " << i << ":\n";
    std::cout << "    Player pos: " << succ.getPlayerPos() << "\n";
    std::cout << "    Box pos: " << succ.getBox(0).pos << "\n";
    std::cout << "    Energy: " << succ.getEnergyUsed() << "\n";
    std::cout << "    Action: " << succ.getActionFromParent() << "\n";

    // Verify energy is within limit
    assert(succ.getEnergyUsed() <= meta.energyLimit);

    if (initial_state.getBoxCount() > 0)
    {
      // Verify player ended up at box's original position
      assert(succ.getPlayerPos() == initial_state.getBox(0).pos);

      // Verify box moved to a different position
      assert(succ.getBox(0).pos != initial_state.getBox(0).pos);
    }
  }

  std::cout << "✓ Integration workflow test passed!\n";
}

void test_simple_manual_setup()
{
  std::cout << "\n=== Testing Simple Manual Setup ===\n";

  // Manually create a simple scenario instead of complex parsing
  Board board;
  board.initialize(4, 3);
  Array<Door> doors;
  board.computeTimeModuloL(doors);

  Meta meta("Simple Test", 4, 3, 100, 2, 5);
  Zobrist zobrist;

  int L = board.getTimeModuloL();
  zobrist.init(12, 26, 10, L); // 4*3=12 positions

  std::cout << "Simple level L = " << L << "\n";

  // Create initial state - simple scenario
  // Player at (1,1), box at (1,2)
  State initial_state(5, -1, 0, 0); // Player at position 5 (1,1)
  initial_state.addBox(6, 0);       // Unlabeled box at position 6 (1,2)
  initial_state.recompute_hash(zobrist);

  std::cout << "Initial simple state:\n";
  std::cout << "  Player at: " << initial_state.getPlayerPos() << "\n";
  std::cout << "  Box at: " << initial_state.getBox(0).pos << "\n";
  std::cout << "  Key held: " << initial_state.getKeyHeld() << "\n";

  // Generate successors
  Array<State> successors = generateSuccessors(initial_state, board, doors, meta);

  std::cout << "Generated " << successors.getSize() << " successors in simple scenario:\n";

  for (int i = 0; i < successors.getSize(); i++)
  {
    const State &succ = successors[i];
    std::cout << "  Successor " << i << ":\n";
    std::cout << "    Player pos: " << succ.getPlayerPos() << "\n";
    std::cout << "    Box pos: " << succ.getBox(0).pos << " (id: " << (char)(succ.getBox(0).id == 0 ? '0' : succ.getBox(0).id) << ")\n";
    std::cout << "    Key held: " << succ.getKeyHeld() << "\n";
    std::cout << "    Energy: " << succ.getEnergyUsed() << "\n";
    std::cout << "    Time: " << succ.getStepModL() << "\n";
  }

  std::cout << "✓ Simple scenario test completed!\n";
}

int main()
{
  std::cout << "=== SuccessorGenerator Integration Tests ===\n";

  test_integration_workflow();
  test_simple_manual_setup();

  std::cout << "\n🎉 All integration tests passed! 🎉\n";
  return 0;
}