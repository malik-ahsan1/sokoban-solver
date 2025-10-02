#include "SuccessorGenerator.h"
#include "Board.h"
#include "Door.h"
#include "State.h"
#include "Meta.h"
#include "Zobrist.h"
#include <iostream>
#include <cassert>

void test_single_box_no_doors()
{
  std::cout << "\n=== Testing Single Box, No Doors ===\n";

  // Create a simple 3x3 board
  Board board;
  board.initialize(3, 3);
  Array<Door> doors;
  board.computeTimeModuloL(doors);

  // Create meta information
  Meta meta("test", 3, 3, 100, 1, 5); // energyLimit=100, moveCost=1, pushCost=5

  // Create Zobrist for hashing
  Zobrist zobrist;
  zobrist.init(9, 10, 5, 1); // 9 positions, 10 box types, 5 key types, L=1

  // Create initial state:
  // Player at (0,0), box at (1,1)
  // P . .
  // . B .
  // . . .
  State initial_state(0, -1, 0, 0); // pos=0, no key, no energy, time=0
  initial_state.addBox(4, 0);       // Box at position 4 (center), unlabeled
  initial_state.recompute_hash(zobrist);

  // Generate successors
  Array<State> successors = generateSuccessors(initial_state, board, doors, meta);

  std::cout << "Generated " << successors.getSize() << " successor states\n";

  // Should generate 4 push actions (up, down, left, right from box)
  // But some might be invalid due to player reachability or bounds

  // Let's check each successor
  for (int i = 0; i < successors.getSize(); i++)
  {
    const State &succ = successors[i];
    std::cout << "Successor " << i << ":\n";
    std::cout << "  Player pos: " << succ.getPlayerPos() << "\n";
    std::cout << "  Box pos: " << succ.getBox(0).pos << "\n";
    std::cout << "  Energy used: " << succ.getEnergyUsed() << "\n";
    std::cout << "  Step mod L: " << succ.getStepModL() << "\n";
    std::cout << "  Action: " << succ.getActionFromParent() << "\n";
  }

  // We expect at least some valid pushes
  assert(successors.getSize() > 0);

  std::cout << "✓ Single box test passed!\n";
}

void test_wall_box_collision()
{
  std::cout << "\n=== Testing Wall/Box Collision Prevention ===\n";

  // Create a 3x3 board with walls
  Board board;
  board.initialize(3, 3);

  // Add walls around the box
  // P . .
  // W B W
  // . W .
  board.set_wall(1, 0); // Left of box
  board.set_wall(1, 2); // Right of box
  board.set_wall(2, 1); // Below box

  Array<Door> doors;
  board.computeTimeModuloL(doors);

  Meta meta("test", 3, 3, 100, 1, 5);
  Zobrist zobrist;
  zobrist.init(9, 10, 5, 1);

  // Initial state: player at (0,0), box at (1,1)
  State initial_state(0, -1, 0, 0);
  initial_state.addBox(4, 0); // Box at center
  initial_state.recompute_hash(zobrist);

  Array<State> successors = generateSuccessors(initial_state, board, doors, meta);

  std::cout << "Generated " << successors.getSize() << " successors with walls\n";

  // Should only be able to push up (to position 1)
  // Left, right, down should be blocked by walls
  for (int i = 0; i < successors.getSize(); i++)
  {
    const State &succ = successors[i];
    int box_pos = succ.getBox(0).pos;
    std::cout << "Valid push to position: " << box_pos << "\n";

    // Should not be able to push into walls (positions 3, 5, 7)
    assert(box_pos != 3 && box_pos != 5 && box_pos != 7);
  }

  std::cout << "✓ Wall collision test passed!\n";
}

void test_box_collision()
{
  std::cout << "\n=== Testing Box-to-Box Collision Prevention ===\n";

  Board board;
  board.initialize(3, 3);
  Array<Door> doors;
  board.computeTimeModuloL(doors);

  Meta meta("test", 3, 3, 100, 1, 5);
  Zobrist zobrist;
  zobrist.init(9, 10, 5, 1);

  // Initial state: player at (0,0), box1 at (1,1), box2 at (1,2)
  // P . .
  // . B1 B2
  // . . .
  State initial_state(0, -1, 0, 0);
  initial_state.addBox(4, 0); // Box1 at center
  initial_state.addBox(5, 0); // Box2 to the right
  initial_state.recompute_hash(zobrist);

  Array<State> successors = generateSuccessors(initial_state, board, doors, meta);

  std::cout << "Generated " << successors.getSize() << " successors with multiple boxes\n";

  // Check that no box gets pushed into the other
  for (int i = 0; i < successors.getSize(); i++)
  {
    const State &succ = successors[i];
    std::cout << "Successor " << i << ":\n";
    std::cout << "  Box 0 pos: " << succ.getBox(0).pos << "\n";
    std::cout << "  Box 1 pos: " << succ.getBox(1).pos << "\n";

    // Boxes should not occupy the same position
    assert(succ.getBox(0).pos != succ.getBox(1).pos);
  }

  std::cout << "✓ Box collision test passed!\n";
}

void test_door_timing()
{
  std::cout << "\n=== Testing Door Timing ===\n";

  Board board;
  board.initialize(3, 3);
  Array<Door> doors;

  // Add a door that alternates open/closed
  Door door(1, 1, 1, 0, true); // Door ID 1, open=1, close=1, starts open
  doors.push_back(door);
  board.set_door(0, 2, 1); // Door at position 2 (top-right)
  board.computeTimeModuloL(doors);

  Meta meta("test", 3, 3, 100, 1, 5);
  Zobrist zobrist;
  zobrist.init(9, 10, 5, 2); // L=2 because of door timing

  // Initial state: player at (0,0), box at (0,1)
  // P B ?  (? = door position)
  // . . .
  // . . .
  State initial_state(0, -1, 0, 0); // Start at time 0 (door is open)
  initial_state.addBox(1, 0);       // Box at position 1
  initial_state.recompute_hash(zobrist);

  Array<State> successors = generateSuccessors(initial_state, board, doors, meta);

  std::cout << "Generated " << successors.getSize() << " successors with door timing\n";
  std::cout << "Door open at time 0: " << door.isOpenAtTime(0) << "\n";
  std::cout << "Door open at time 1: " << door.isOpenAtTime(1) << "\n";

  // Check if any successor pushed the box into the door position
  bool pushed_into_door = false;
  for (int i = 0; i < successors.getSize(); i++)
  {
    const State &succ = successors[i];
    if (succ.getBox(0).pos == 2)
    { // Box pushed to door position
      pushed_into_door = true;
      std::cout << "Box pushed into door position at time " << succ.getStepModL() << "\n";
    }
  }

  // The behavior depends on timing - at time 0 door is open,
  // so if player can reach position (0,0) in 0 steps and push at time 1,
  // the door would be closed. Need to check the specific timing.

  std::cout << "✓ Door timing test completed!\n";
}

void test_locked_box()
{
  std::cout << "\n=== Testing Locked Box with Keys ===\n";

  Board board;
  board.initialize(3, 3);
  Array<Door> doors;
  board.computeTimeModuloL(doors);

  Meta meta("test", 3, 3, 100, 1, 5);
  Zobrist zobrist;
  zobrist.init(9, 26, 5, 1); // 26 for A-Z box labels

  // Test 1: Player has no key, box is labeled 'A'
  State state_no_key(0, -1, 0, 0); // No key held (-1)
  state_no_key.addBox(4, 'A');     // Labeled box 'A' at center
  state_no_key.recompute_hash(zobrist);

  Array<State> successors_no_key = generateSuccessors(state_no_key, board, doors, meta);

  std::cout << "Successors with no key for labeled box: " << successors_no_key.getSize() << "\n";

  // Should generate 0 successors since player can't push locked box
  assert(successors_no_key.getSize() == 0);

  // Test 2: Player has key 'A' (key index 0), box is labeled 'A'
  State state_with_key(0, 0, 0, 0); // Key 0 held (corresponds to 'A')
  state_with_key.addBox(4, 'A');    // Labeled box 'A' at center
  state_with_key.recompute_hash(zobrist);

  Array<State> successors_with_key = generateSuccessors(state_with_key, board, doors, meta);

  std::cout << "Successors with correct key for labeled box: " << successors_with_key.getSize() << "\n";

  // Should generate some successors since player can push the box
  assert(successors_with_key.getSize() > 0);

  // Check that the box becomes unlabeled and key is consumed
  for (int i = 0; i < successors_with_key.getSize(); i++)
  {
    const State &succ = successors_with_key[i];
    std::cout << "Successor " << i << ": box id = '" << (char)(succ.getBox(0).id == 0 ? '0' : succ.getBox(0).id) << "', key held = " << succ.getKeyHeld() << "\n";

    // Box should be unlocked (id = 0) and no key should be held
    assert(succ.getBox(0).id == 0);
    assert(succ.getKeyHeld() == -1);
  }

  std::cout << "✓ Locked box test passed!\n";
}

void test_energy_limit()
{
  std::cout << "\n=== Testing Energy Limit ===\n";

  Board board;
  board.initialize(5, 5); // Larger board for more movement
  Array<Door> doors;
  board.computeTimeModuloL(doors);

  // Set very low energy limit
  Meta meta("test", 5, 5, 10, 2, 8); // energyLimit=10, moveCost=2, pushCost=8
  Zobrist zobrist;
  zobrist.init(25, 10, 5, 1);

  // Place player far from box to require significant movement
  // P at (0,0), box at (4,4)
  State initial_state(0, -1, 8, 0); // Already used 8 energy
  initial_state.addBox(24, 0);      // Box at far corner
  initial_state.recompute_hash(zobrist);

  Array<State> successors = generateSuccessors(initial_state, board, doors, meta);

  std::cout << "Generated " << successors.getSize() << " successors with energy limit\n";

  // With energy limit 10, current usage 8, and high costs,
  // most pushes should be prevented
  for (int i = 0; i < successors.getSize(); i++)
  {
    const State &succ = successors[i];
    std::cout << "Successor energy: " << succ.getEnergyUsed() << "\n";
    assert(succ.getEnergyUsed() <= meta.energyLimit);
  }

  std::cout << "✓ Energy limit test passed!\n";
}

int main()
{
  std::cout << "=== SuccessorGenerator Test Suite ===\n";

  test_single_box_no_doors();
  test_wall_box_collision();
  test_box_collision();
  test_door_timing();
  test_locked_box();
  test_energy_limit();

  std::cout << "\n🎉 All SuccessorGenerator tests passed! 🎉\n";
  return 0;
}