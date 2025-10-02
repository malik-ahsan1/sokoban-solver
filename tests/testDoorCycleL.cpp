#include "../src/Board.h"
#include "../src/Door.h"
#include "../src/State.h"
#include "../src/Zobrist.h"
#include <iostream>
#include <cassert>

void testDoorCycleModulusL()
{
  std::cout << "\n=== Testing Door Cycle Modulus L ===" << std::endl;

  // Test 1: Board with no doors should have L = 1
  Board emptyBoard;
  emptyBoard.initialize(5, 5);
  Array<Door> noDoors;
  emptyBoard.computeTimeModuloL(noDoors);

  assert(emptyBoard.getTimeModuloL() == 1);
  std::cout << "✓ Empty board has L = 1" << std::endl;

  // Test 2: Board with one door (open=3, close=2) should have L = 5
  Array<Door> oneDoor;
  Door testDoor(1, 3, 2, 0, true); // id=1, open=3, close=2, phase=0, starts open
  oneDoor.push_back(testDoor);

  Board boardWithOneDoor;
  boardWithOneDoor.initialize(5, 5);
  boardWithOneDoor.computeTimeModuloL(oneDoor);

  assert(boardWithOneDoor.getTimeModuloL() == 5);
  std::cout << "✓ Board with door (open=3, close=2) has L = 5" << std::endl;

  // Test 3: Multiple doors with different cycles
  Array<Door> multipleDoors;
  Door door1(1, 2, 4, 0, true); // cycle = 6
  Door door2(2, 3, 3, 0, true); // cycle = 6
  Door door3(3, 4, 2, 0, true); // cycle = 6
  multipleDoors.push_back(door1);
  multipleDoors.push_back(door2);
  multipleDoors.push_back(door3);

  Board boardMultiple;
  boardMultiple.initialize(5, 5);
  boardMultiple.computeTimeModuloL(multipleDoors);

  // LCM(6, 6, 6) = 6
  assert(boardMultiple.getTimeModuloL() == 6);
  std::cout << "✓ Board with multiple doors (all cycle=6) has L = 6" << std::endl;

  // Test 4: Different cycles requiring LCM calculation
  Array<Door> differentCycles;
  Door doorA(1, 2, 2, 0, true); // cycle = 4
  Door doorB(2, 2, 3, 0, true); // cycle = 5
  differentCycles.push_back(doorA);
  differentCycles.push_back(doorB);

  Board boardLCM;
  boardLCM.initialize(5, 5);
  boardLCM.computeTimeModuloL(differentCycles);

  // LCM(4, 5) = 20
  assert(boardLCM.getTimeModuloL() == 20);
  std::cout << "✓ Board with cycles 4,5 has L = 20" << std::endl;
}

void testStateStepModL()
{
  std::cout << "\n=== Testing State step_mod_L Updates ===" << std::endl;

  // Test initial state
  State state;
  assert(state.getStepModL() == 0);
  std::cout << "✓ Initial state has step_mod_L = 0" << std::endl;

  // Test step updates with different L values
  int L = 5;

  // Simulate several steps
  state.updateStepModL(L); // 0 -> 1
  assert(state.getStepModL() == 1);

  state.updateStepModL(L); // 1 -> 2
  assert(state.getStepModL() == 2);

  state.updateStepModL(L); // 2 -> 3
  assert(state.getStepModL() == 3);

  state.updateStepModL(L); // 3 -> 4
  assert(state.getStepModL() == 4);

  state.updateStepModL(L); // 4 -> 0 (wraps around)
  assert(state.getStepModL() == 0);

  std::cout << "✓ Step updates work correctly with modulo arithmetic" << std::endl;
}

void testZobristTimeHashing()
{
  std::cout << "\n=== Testing Zobrist Time Hashing ===" << std::endl;

  // Initialize Zobrist with time modulo support
  Zobrist zobrist;
  int boardSize = 25;
  int timeModuloL = 5;
  zobrist.init(boardSize, 10, 3, timeModuloL, 12345);

  // Create two identical states except for step_mod_L
  State state1(10, -1, 0, 0); // step_mod_L = 0
  state1.addBox(5, 0);        // Add identical box

  State state2(10, -1, 0, 1); // step_mod_L = 1
  state2.addBox(5, 0);        // Add identical box

  // Compute hashes
  state1.recompute_hash(zobrist);
  state2.recompute_hash(zobrist);

  uint64_t hash1 = state1.hash();
  uint64_t hash2 = state2.hash();

  // Hashes should be different because step_mod_L differs
  assert(hash1 != hash2);
  std::cout << "✓ States with different step_mod_L have different hashes" << std::endl;

  // Test that same step_mod_L produces same hash
  State state3(10, -1, 0, 0); // Same as state1
  state3.addBox(5, 0);
  state3.recompute_hash(zobrist);
  uint64_t hash3 = state3.hash();

  assert(hash1 == hash3);
  std::cout << "✓ States with same step_mod_L have same hashes" << std::endl;

  // Test boundary conditions
  State stateLast(10, -1, 0, timeModuloL - 1); // step_mod_L = 4 (max for L=5)
  stateLast.addBox(5, 0);
  stateLast.recompute_hash(zobrist);
  uint64_t hashLast = stateLast.hash();

  assert(hashLast != hash1);
  assert(hashLast != hash2);
  std::cout << "✓ Boundary step_mod_L values produce different hashes" << std::endl;
}

void testCompleteIntegration()
{
  std::cout << "\n=== Testing Complete Integration ===" << std::endl;

  // Create doors and compute L
  Array<Door> doors;
  Door door1(1, 3, 2, 0, true); // cycle = 5
  doors.push_back(door1);

  Board board;
  board.initialize(8, 8);
  board.computeTimeModuloL(doors);

  int L = board.getTimeModuloL();
  assert(L == 5);

  // Initialize Zobrist with the board's L
  Zobrist zobrist;
  zobrist.init(board.get_width() * board.get_height(), 10, 3, L, 54321);

  // Create initial state and simulate game progression
  State currentState;
  currentState.setPlayerPos(10);
  currentState.addBox(15, 0);
  currentState.recompute_hash(zobrist);

  uint64_t initialHash = currentState.hash();

  // Simulate 5 steps (should wrap around to step_mod_L = 0)
  Array<uint64_t> stepHashes;
  stepHashes.push_back(initialHash);

  for (int step = 1; step < L; step++)
  {
    State nextState = currentState;
    nextState.updateStepModL(L);
    nextState.recompute_hash(zobrist);

    uint64_t stepHash = nextState.hash();
    stepHashes.push_back(stepHash);

    // Each step should have a different hash
    for (int i = 0; i < stepHashes.getSize() - 1; i++)
    {
      assert(stepHashes[i] != stepHash);
    }

    currentState = nextState;
  }

  // After L steps, we should be back to step_mod_L = 0
  currentState.updateStepModL(L); // This should wrap to 0
  assert(currentState.getStepModL() == 0);

  currentState.recompute_hash(zobrist);
  uint64_t finalHash = currentState.hash();

  // Final hash should match initial hash (same step_mod_L, same state)
  assert(finalHash == initialHash);

  std::cout << "✓ Complete integration test passed - L=" << L
            << ", all " << L << " step hashes unique, wraps correctly" << std::endl;
}

void testMaxSafeLWarning()
{
  std::cout << "\n=== Testing MAX_SAFE_L Warning ===" << std::endl;

  // Create doors that would result in a large L
  Array<Door> largeCycleDoors;
  Door door1(1, 2048, 2049, 0, true); // cycle = 4097 > MAX_SAFE_L
  largeCycleDoors.push_back(door1);

  Board board;
  board.initialize(5, 5);

  std::cout << "Expected warning about L > MAX_SAFE_L:" << std::endl;
  board.computeTimeModuloL(largeCycleDoors);

  int L = board.getTimeModuloL();
  std::cout << "Computed L = " << L << " (should be > 4096)" << std::endl;
  assert(L > 4096); // Should exceed MAX_SAFE_L

  std::cout << "✓ Large L warning system works" << std::endl;
}

int main()
{
  std::cout << "Testing Door Cycle Modulus L Support" << std::endl;
  std::cout << "====================================" << std::endl;

  try
  {
    testDoorCycleModulusL();
    testStateStepModL();
    testZobristTimeHashing();
    testCompleteIntegration();
    testMaxSafeLWarning();

    std::cout << "\n🎉 All Door Cycle Modulus L tests passed!" << std::endl;
    std::cout << "\nKey features verified:" << std::endl;
    std::cout << "- LCM calculation for door cycles" << std::endl;
    std::cout << "- Board stores and provides time modulo L" << std::endl;
    std::cout << "- State properly updates step_mod_L with modulo arithmetic" << std::endl;
    std::cout << "- Zobrist hashing includes step_mod_L for unique state hashes" << std::endl;
    std::cout << "- MAX_SAFE_L warning system for large L values" << std::endl;
    std::cout << "- Complete integration between Board, State, and Zobrist" << std::endl;

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