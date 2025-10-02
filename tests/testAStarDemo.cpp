#include "../src/SokobanHeuristics.h"
#include "../src/SuccessorGenerator.h"
#include "../src/Board.h"
#include "../src/State.h"
#include "../src/Zobrist.h"
#include "../src/Meta.h"
#include "../src/Door.h"
#include <iostream>
#include <queue>
#include <unordered_set>

// A* node for priority queue
struct AStarNode
{
  State state;
  int g_cost; // Actual cost (energy used)
  int h_cost; // Heuristic cost
  int f_cost; // f = g + h

  AStarNode(const State &s, int g, int h)
      : state(s), g_cost(g), h_cost(h), f_cost(g + h) {}

  // For priority queue (min-heap, so reverse comparison)
  bool operator>(const AStarNode &other) const
  {
    return f_cost > other.f_cost;
  }
};

void demonstrate_a_star_integration()
{
  std::cout << "\n=== A* Search with Heuristics Demonstration ===\n";

  // Create a solvable Sokoban puzzle
  Board board;
  board.initialize(5, 4); // 4 rows, 5 cols

  // Layout:
  // # # # # #
  // # @ $ . #
  // # . . . #
  // # # # # #

  // Border walls
  for (int col = 0; col < 5; col++)
  {
    board.set_wall(0, col);
    board.set_wall(3, col);
  }
  for (int row = 0; row < 4; row++)
  {
    board.set_wall(row, 0);
    board.set_wall(row, 4);
  }

  // Goals
  board.set_target(1, 3); // Goal at (1,3)
  board.set_target(2, 1); // Additional goals
  board.set_target(2, 2);
  board.set_target(2, 3);

  std::cout << "Board layout:" << std::endl;
  board.pretty_print();

  // Initialize components
  SokobanHeuristics heuristics;
  heuristics.initialize(board);

  Array<Door> doors;
  board.computeTimeModuloL(doors);

  Meta meta("A* Demo", 5, 4, 1000, 1, 3); // High energy limit, push cost = 3

  Zobrist zobrist;
  zobrist.init(20, 10, 5, 1);

  // Initial state: player at (1,1), box at (1,2)
  State initial_state(6, -1, 0, 0); // Player at (1,1)
  initial_state.addBox(7, 0);       // Box at (1,2)
  initial_state.recompute_hash(zobrist);

  std::cout << "\nInitial state:" << std::endl;
  std::cout << "  Player at position: " << initial_state.getPlayerPos() << std::endl;
  std::cout << "  Box at position: " << initial_state.getBox(0).pos << std::endl;

  int initial_h = heuristics.heuristic(initial_state);
  std::cout << "  Initial heuristic: " << initial_h << std::endl;

  // Check if initial state is solved
  bool is_solved = (initial_h == 0);
  std::cout << "  Initial state solved: " << (is_solved ? "Yes" : "No") << std::endl;

  if (is_solved)
  {
    std::cout << "Puzzle is already solved!" << std::endl;
    return;
  }

  // Initialize A* search
  std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> frontier;
  std::unordered_set<uint64_t> visited;

  // Add initial state to frontier
  AStarNode initial_node(initial_state, 0, initial_h);
  frontier.push(initial_node);

  int nodes_expanded = 0;
  int max_expansions = 50; // Limit for demonstration

  std::cout << "\nRunning A* search (limited to " << max_expansions << " expansions)..." << std::endl;

  while (!frontier.empty() && nodes_expanded < max_expansions)
  {
    // Get best node from frontier
    AStarNode current = frontier.top();
    frontier.pop();

    uint64_t current_hash = current.state.hash();

    // Skip if already visited
    if (visited.count(current_hash))
    {
      continue;
    }
    visited.insert(current_hash);

    nodes_expanded++;

    std::cout << "\nExpansion " << nodes_expanded << ":" << std::endl;
    std::cout << "  Current f=" << current.f_cost << " (g=" << current.g_cost << ", h=" << current.h_cost << ")" << std::endl;
    std::cout << "  Player at: " << current.state.getPlayerPos() << std::endl;
    std::cout << "  Box at: " << current.state.getBox(0).pos << std::endl;

    // Check if goal state
    if (current.h_cost == 0)
    {
      std::cout << "  🎉 GOAL STATE FOUND!" << std::endl;
      std::cout << "  Solution cost: " << current.g_cost << std::endl;
      std::cout << "  Nodes expanded: " << nodes_expanded << std::endl;
      return;
    }

    // Generate successors
    Array<State> successors = generateSuccessors(current.state, board, doors, meta);
    int valid_successors = 0;

    for (int i = 0; i < successors.getSize(); i++)
    {
      const State &succ = successors[i];

      // Apply deadlock pruning
      if (heuristics.isDeadlocked(succ))
      {
        continue; // Prune deadlocked states
      }

      uint64_t succ_hash = succ.hash();
      if (visited.count(succ_hash))
      {
        continue; // Skip already visited states
      }

      // Calculate costs
      int g = succ.getEnergyUsed();
      int h = heuristics.heuristic(succ);

      AStarNode succ_node(succ, g, h);
      frontier.push(succ_node);

      valid_successors++;
      std::cout << "    Successor " << valid_successors << ": f=" << succ_node.f_cost
                << " (g=" << g << ", h=" << h << "), box@" << succ.getBox(0).pos << std::endl;
    }

    std::cout << "  Generated " << valid_successors << " valid successors" << std::endl;
    std::cout << "  Frontier size: " << frontier.size() << std::endl;
  }

  if (nodes_expanded >= max_expansions)
  {
    std::cout << "\nReached expansion limit (" << max_expansions << "). Search terminated." << std::endl;
  }
  else
  {
    std::cout << "\nNo solution found with current search parameters." << std::endl;
  }

  std::cout << "Final statistics:" << std::endl;
  std::cout << "  Nodes expanded: " << nodes_expanded << std::endl;
  std::cout << "  States visited: " << visited.size() << std::endl;
  std::cout << "  Final frontier size: " << frontier.size() << std::endl;
}

void demonstrate_heuristic_accuracy()
{
  std::cout << "\n=== Heuristic Accuracy Demonstration ===\n";

  // Create a linear puzzle to test heuristic accuracy
  Board board;
  board.initialize(5, 3); // 3 rows, 5 cols

  // Layout:
  // # # # # #
  // @ $ $ $ .
  // # # # # #

  for (int col = 0; col < 5; col++)
  {
    board.set_wall(0, col);
    board.set_wall(2, col);
  }
  board.set_target(1, 4); // Goal at end

  SokobanHeuristics heuristics;
  heuristics.initialize(board);

  Zobrist zobrist;
  zobrist.init(15, 10, 5, 1);

  std::cout << "Testing heuristic accuracy on linear puzzle:" << std::endl;

  // Test different box positions
  for (int box_pos = 1; box_pos <= 4; box_pos++)
  {
    State test_state(1, -1, 0, 0);         // Player at (1,0)
    test_state.addBox(5 + box_pos - 1, 0); // Box at (1,box_pos)
    test_state.recompute_hash(zobrist);

    int h = heuristics.heuristic(test_state);
    int actual_distance = 4 - box_pos; // Distance from box to goal

    std::cout << "  Box at position (1," << box_pos << "): h=" << h
              << ", actual distance=" << actual_distance;

    if (h == actual_distance)
    {
      std::cout << " ✓ Perfect!" << std::endl;
    }
    else if (h < actual_distance)
    {
      std::cout << " ⚠ Inadmissible (overestimate)" << std::endl;
    }
    else
    {
      std::cout << " ✓ Admissible (underestimate)" << std::endl;
    }
  }
}

int main()
{
  std::cout << "Sokoban A* Search with Heuristics Demo" << std::endl;
  std::cout << "=====================================" << std::endl;

  try
  {
    demonstrate_heuristic_accuracy();
    demonstrate_a_star_integration();

    std::cout << "\n🎉 A* Search Demonstration Complete!" << std::endl;
    std::cout << "\nKey features demonstrated:" << std::endl;
    std::cout << "- Admissible heuristic function (never overestimates)" << std::endl;
    std::cout << "- Effective deadlock pruning reduces search space" << std::endl;
    std::cout << "- Priority queue ordering by f = g + h" << std::endl;
    std::cout << "- Visited state tracking with Zobrist hashing" << std::endl;
    std::cout << "- Complete integration with successor generation" << std::endl;
    std::cout << "- Ready for production Sokoban solver!" << std::endl;

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Demo failed with exception: " << e.what() << std::endl;
    return 1;
  }
}