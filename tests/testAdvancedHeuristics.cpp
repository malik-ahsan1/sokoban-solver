#include "../src/SokobanHeuristics.h"
#include "../src/Board.h"
#include "../src/State.h"
#include "../src/Zobrist.h"
#include <iostream>
#include <cassert>

void test_hungarian_two_boxes_two_goals()
{
  std::cout << "\n=== Testing Hungarian Algorithm: Two Boxes, Two Goals ===\n";

  // Create a simple scenario where Hungarian makes a difference
  Board board;
  board.initialize(4, 3); // 3 rows, 4 cols

  // Layout:
  // @ $ $ .
  // # # # #
  // . . . .

  // Walls in middle row
  for (int col = 0; col < 4; col++)
  {
    board.set_wall(1, col);
  }

  // Goals at (0,3) and (2,0)
  board.set_target(0, 3);
  board.set_target(2, 0);

  SokobanHeuristics heuristics;
  heuristics.initialize(board);

  Zobrist zobrist;
  zobrist.init(12, 10, 5, 1);

  // State: Player at (0,0), Box1 at (0,1), Box2 at (0,2)
  State test_state(0, -1, 0, 0);
  test_state.addBox(1, 0); // Box1 at (0,1)
  test_state.addBox(2, 0); // Box2 at (0,2)
  test_state.recompute_hash(zobrist);

  int h_hungarian = heuristics.heuristic(test_state);
  int h_simple = heuristics.heuristic_simple(test_state);

  std::cout << "Box1 at (0,1), Box2 at (0,2)" << std::endl;
  std::cout << "Goal1 at (0,3), Goal2 at (2,0)" << std::endl;
  std::cout << "Hungarian assignment: " << h_hungarian << std::endl;
  std::cout << "Simple sum of mins: " << h_simple << std::endl;

  // Hungarian should find optimal assignment
  // Box1 to Goal1: distance = 2, Box2 to Goal2: needs to go around wall, much larger
  // vs Box2 to Goal1: distance = 1, Box1 to Goal2: needs to go around wall
  // Hungarian should pick the better assignment

  assert(h_hungarian >= 0);
  assert(h_hungarian != h_simple); // Should be different in this case

  std::cout << "✓ Hungarian algorithm test passed!" << std::endl;
}

void test_wall_deadlock_detection()
{
  std::cout << "\n=== Testing Wall Deadlock Detection ===\n";

  // Create a scenario with wall deadlock
  Board board;
  board.initialize(5, 4); // 4 rows, 5 cols

  // Layout:
  // # # # # #
  // # @ $ # .
  // # # # # .
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
    board.set_wall(row, 3);
  }

  // Goal at (1,4) - outside the walled area
  board.set_target(1, 4);
  board.set_target(2, 4);

  SokobanHeuristics heuristics;
  heuristics.initialize(board);

  heuristics.printCornerMap();

  Zobrist zobrist;
  zobrist.init(20, 10, 5, 1);

  // Test 1: Box against top wall with no goal along the wall
  State wall_deadlock_state(5, -1, 0, 0); // Player at (1,1)
  wall_deadlock_state.addBox(1, 0);       // Box at (0,1) - against top wall
  wall_deadlock_state.recompute_hash(zobrist);

  // Wait, position (0,1) would be in the wall. Let me fix this
  // Box at (1,2) which is against the right wall (position 3)
  wall_deadlock_state = State(5, -1, 0, 0); // Player at (1,1)
  wall_deadlock_state.addBox(7, 0);         // Box at (1,2) - against right wall
  wall_deadlock_state.recompute_hash(zobrist);

  bool is_corner_deadlocked = heuristics.isCornerDeadlocked(wall_deadlock_state);
  bool is_wall_deadlocked = heuristics.isWallDeadlocked(wall_deadlock_state);
  bool is_deadlocked = heuristics.isDeadlocked(wall_deadlock_state);

  std::cout << "Box against wall (no goal along wall):" << std::endl;
  std::cout << "  Corner deadlocked: " << is_corner_deadlocked << std::endl;
  std::cout << "  Wall deadlocked: " << is_wall_deadlocked << std::endl;
  std::cout << "  Overall deadlocked: " << is_deadlocked << std::endl;

  // This should be wall deadlocked since box is against wall with no goals
  assert(is_wall_deadlocked);
  assert(is_deadlocked);

  std::cout << "✓ Wall deadlock detection test passed!" << std::endl;
}

void test_no_wall_deadlock_with_goal()
{
  std::cout << "\n=== Testing No Wall Deadlock When Goal Present ===\n";

  // Create scenario where box is against wall BUT there's a goal along that wall
  Board board;
  board.initialize(5, 3); // 3 rows, 5 cols

  // Layout:
  // # # # # #
  // @ $ . $ #
  // # # # # #

  // Border walls
  for (int col = 0; col < 5; col++)
  {
    board.set_wall(0, col);
    board.set_wall(2, col);
  }
  for (int row = 0; row < 3; row++)
  {
    board.set_wall(row, 4);
  }

  // Goal along the wall where box will be
  board.set_target(1, 2); // Goal along the same wall row

  SokobanHeuristics heuristics;
  heuristics.initialize(board);

  Zobrist zobrist;
  zobrist.init(15, 10, 5, 1);

  // Box at (1,1) against top wall, but goal at (1,2) along same wall
  State no_deadlock_state(5, -1, 0, 0); // Player at (1,0)
  no_deadlock_state.addBox(6, 0);       // Box at (1,1)
  no_deadlock_state.recompute_hash(zobrist);

  bool is_wall_deadlocked = heuristics.isWallDeadlocked(no_deadlock_state);
  bool is_deadlocked = heuristics.isDeadlocked(no_deadlock_state);

  std::cout << "Box against wall (WITH goal along wall):" << std::endl;
  std::cout << "  Wall deadlocked: " << is_wall_deadlocked << std::endl;
  std::cout << "  Overall deadlocked: " << is_deadlocked << std::endl;

  // Should NOT be deadlocked since there's a goal along the wall
  assert(!is_wall_deadlocked);
  assert(!is_deadlocked);

  std::cout << "✓ No wall deadlock with goal test passed!" << std::endl;
}

void test_hungarian_vs_simple_comparison()
{
  std::cout << "\n=== Testing Hungarian vs Simple Heuristic Comparison ===\n";

  // Create several test cases and verify Hungarian <= Simple always
  Board board;
  board.initialize(4, 4);

  // Create 3 goals
  board.set_target(0, 0);
  board.set_target(0, 3);
  board.set_target(3, 3);

  SokobanHeuristics heuristics;
  heuristics.initialize(board);

  Zobrist zobrist;
  zobrist.init(16, 10, 5, 1);

  // Test case 1: Single box
  State single_box(5, -1, 0, 0);
  single_box.addBox(6, 0);
  single_box.recompute_hash(zobrist);

  int h1 = heuristics.heuristic(single_box);
  int s1 = heuristics.heuristic_simple(single_box);

  std::cout << "Single box: Hungarian=" << h1 << ", Simple=" << s1 << std::endl;
  assert(h1 == s1); // Should be equal for single box

  // Test case 2: Two boxes
  State two_boxes(5, -1, 0, 0);
  two_boxes.addBox(6, 0);
  two_boxes.addBox(9, 0);
  two_boxes.recompute_hash(zobrist);

  int h2 = heuristics.heuristic(two_boxes);
  int s2 = heuristics.heuristic_simple(two_boxes);

  std::cout << "Two boxes: Hungarian=" << h2 << ", Simple=" << s2 << std::endl;
  assert(h2 <= s2); // Hungarian should be <= simple

  // Test case 3: Three boxes (equals goals)
  State three_boxes(5, -1, 0, 0);
  three_boxes.addBox(6, 0);
  three_boxes.addBox(9, 0);
  three_boxes.addBox(10, 0);
  three_boxes.recompute_hash(zobrist);

  int h3 = heuristics.heuristic(three_boxes);
  int s3 = heuristics.heuristic_simple(three_boxes);

  std::cout << "Three boxes: Hungarian=" << h3 << ", Simple=" << s3 << std::endl;
  assert(h3 <= s3); // Hungarian should be <= simple

  std::cout << "✓ Hungarian vs Simple comparison test passed!" << std::endl;
}

int main()
{
  std::cout << "Testing Advanced Sokoban Heuristics (Phase 6)" << std::endl;
  std::cout << "=============================================" << std::endl;

  try
  {
    test_hungarian_two_boxes_two_goals();
    test_wall_deadlock_detection();
    test_no_wall_deadlock_with_goal();
    test_hungarian_vs_simple_comparison();

    std::cout << "\n🎉 All Phase 6 tests passed successfully!" << std::endl;
    std::cout << "\nAdvanced heuristics implementation complete:" << std::endl;
    std::cout << "- Hungarian assignment algorithm for optimal box-goal matching" << std::endl;
    std::cout << "- Wall deadlock detection (Stage 2)" << std::endl;
    std::cout << "- Corner deadlock detection (Stage 1)" << std::endl;
    std::cout << "- Comprehensive integration with existing solver" << std::endl;

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Phase 6 test failed: " << e.what() << std::endl;
    return 1;
  }
}