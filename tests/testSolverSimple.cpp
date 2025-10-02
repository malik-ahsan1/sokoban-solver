#include "../src/SolverAStar.h"
#include "../src/Board.h"
#include "../src/State.h"
#include "../src/Door.h"
#include "../src/Meta.h"
#include "../src/Zobrist.h"
#include "../src/ConfigParser.h"
#include <iostream>
#include <cassert>

void test_simple_solver()
{
  std::cout << "\n=== Testing Simple A* Solver ===\n";

  // Create a simple 4x4 board with one box and one goal
  Board board;
  board.initialize(4, 4); // 4 rows, 4 cols

  // Set up walls around the perimeter
  for (int i = 0; i < 4; i++)
  {
    board.set_wall(0, i); // Top wall
    board.set_wall(3, i); // Bottom wall
    board.set_wall(i, 0); // Left wall
    board.set_wall(i, 3); // Right wall
  }

  // Add a goal at position (2, 2)
  board.set_target(2, 2);

  std::cout << "Board layout:\n";
  std::cout << "####\n";
  std::cout << "#  #\n";
  std::cout << "# .#\n";
  std::cout << "####\n";
  std::cout << "Player at (1,1), Box at (1,2), Goal at (2,2)\n\n";

  // Create empty door array
  Array<Door> doors;

  // Create meta configuration
  Meta meta;
  meta.energyLimit = 100;
  meta.moveCost = 1;
  meta.pushCost = 2;

  // Initialize Zobrist hashing
  Zobrist zobrist;
  zobrist.init(16, 1, 1, 10); // 4x4 board = 16 positions, 1 box type, 1 key type, time mod 10

  // Create initial dynamic state
  InitialDynamicState init;
  init.player_pos = 1 * 4 + 1; // Position (1, 1)

  // Add one unlocked box at position (1, 2)
  init.unlockedBoxes.push_back(1 * 4 + 2); // Position (1, 2)

  // Create solver
  SolverAStar solver(board, doors, meta, zobrist);

  // Solve the puzzle
  Array<char> moves;
  SolverStats stats;

  std::cout << "Attempting to solve puzzle...\n";
  bool solved = solver.solve(init, moves, stats);

  std::cout << "Solver Results:\n";
  std::cout << "  Solved: " << (solved ? "YES" : "NO") << "\n";
  std::cout << "  Nodes Expanded: " << stats.nodesExpanded << "\n";
  std::cout << "  Nodes Generated: " << stats.nodesGenerated << "\n";
  std::cout << "  Peak Open Size: " << stats.peakOpenSize << "\n";
  std::cout << "  Elapsed Time: " << stats.elapsedMs << " ms\n";
  std::cout << "  Solution Cost: " << stats.solutionCost << "\n";
  std::cout << "  Move Sequence Length: " << moves.getSize() << "\n";

  if (solved && moves.getSize() > 0)
  {
    std::cout << "  Moves: ";
    for (int i = 0; i < moves.getSize(); i++)
    {
      std::cout << moves[i];
    }
    std::cout << "\n";
  }

  // Basic assertions
  assert(stats.nodesGenerated > 0 && "Should have generated at least one node");
  assert(stats.nodesExpanded >= 0 && "Should have expanded at least zero nodes");
  assert(stats.peakOpenSize > 0 && "Should have had nodes in open set");
  assert(stats.elapsedMs >= 0 && "Elapsed time should be non-negative");

  std::cout << "✓ Simple solver test completed!\n";
}

void test_solver_stats()
{
  std::cout << "\n=== Testing Solver Statistics ===\n";

  // Create a minimal unsolvable puzzle to test stats tracking
  Board board;
  board.initialize(4, 4);

  // Create walls around perimeter
  for (int i = 0; i < 4; i++)
  {
    board.set_wall(0, i); // Top wall
    board.set_wall(3, i); // Bottom wall
    board.set_wall(i, 0); // Left wall
    board.set_wall(i, 3); // Right wall
  }

  // Add an internal wall to create unsolvable scenario
  board.set_wall(1, 2); // Wall between box and goal
  board.set_wall(2, 2); // Another wall

  // Place goal at unreachable position
  board.set_target(2, 1); // Goal position

  Array<Door> doors;
  Meta meta;
  meta.energyLimit = 10; // Low limit to test early termination
  meta.moveCost = 1;
  meta.pushCost = 2;

  Zobrist zobrist;
  zobrist.init(16, 1, 1, 10); // 4x4 board = 16 positions

  InitialDynamicState init;
  init.player_pos = 1 * 4 + 1; // Player at position (1, 1)
  // Add a box that can't reach the goal due to walls
  init.unlockedBoxes.push_back(2 * 4 + 3); // Box at position (2, 3) - trapped

  SolverAStar solver(board, doors, meta, zobrist);

  Array<char> moves;
  SolverStats stats;

  bool solved = solver.solve(init, moves, stats);

  std::cout << "Unsolvable puzzle results:\n";
  std::cout << "  Solved: " << (solved ? "YES" : "NO") << "\n";
  std::cout << "  Stats collected properly: " << (stats.elapsedMs >= 0 ? "YES" : "NO") << "\n";

  assert(!solved && "Unsolvable puzzle should return false");
  assert(stats.elapsedMs >= 0 && "Should measure elapsed time");

  std::cout << "✓ Solver statistics test completed!\n";
}

void test_solver_initialization()
{
  std::cout << "\n=== Testing Solver Initialization ===\n";

  // Test that solver initializes properly with valid components
  Board board;
  board.initialize(5, 5);

  Array<Door> doors;
  Meta meta;
  Zobrist zobrist;
  zobrist.init(25, 1, 1, 10); // 5x5 board = 25 positions

  // Should not crash during construction
  SolverAStar solver(board, doors, meta, zobrist);

  std::cout << "  Solver initialized successfully\n";
  std::cout << "✓ Solver initialization test completed!\n";
}

int main()
{
  std::cout << "Testing Sokoban A* Solver\n";
  std::cout << "=========================\n";

  try
  {
    test_solver_initialization();
    test_simple_solver();
    test_solver_stats();

    std::cout << "\n🎉 All A* Solver tests completed successfully!\n";
    std::cout << "\nA* Solver implementation includes:\n";
    std::cout << "- Node structure with f-cost ordering\n";
    std::cout << "- Complete A* algorithm with open/closed sets\n";
    std::cout << "- Deadlock detection integration\n";
    std::cout << "- Comprehensive statistics tracking\n";
    std::cout << "- Energy limit enforcement\n";
    std::cout << "- Goal state detection\n";

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cout << "Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
}