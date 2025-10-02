#include "PlayerPathfinder.h"
#include "Board.h"
#include "Door.h"
#include "Array.h"
#include <iostream>
#include <cassert>

void test_basic_functionality()
{
  std::cout << "\n=== Testing Basic Pathfinder Functionality ===\n";

  // Create a simple 3x3 board with no doors
  Board board;
  board.initialize(3, 3);
  Array<Door> doors;

  // Set up a simple layout:
  // . . .
  // . W .  (W = wall)
  // . . .
  // All cells are initially CELL_FLOOR, set middle to wall
  board.set_wall(1, 1); // Middle cell is a wall

  // Compute L with empty doors (should be 1)
  board.computeTimeModuloL(doors);

  PlayerPathfinder pathfinder;
  pathfinder.initialize(board, doors);

  // Test pathfinding from (0,0) to (2,2)
  pathfinder.findPaths(0, 0); // Start at top-left, time 0

  // Check distances
  int dist_to_center = pathfinder.getDistance(4); // Wall should be unreachable
  int dist_to_corner = pathfinder.getDistance(8); // Bottom-right corner

  std::cout << "Distance to wall (should be -1): " << dist_to_center << std::endl;
  std::cout << "Distance to opposite corner: " << dist_to_corner << std::endl;

  assert(dist_to_center == -1); // Wall is unreachable
  assert(dist_to_corner == 4);  // Manhattan distance around the wall

  // Test path reconstruction
  Array<char> path = pathfinder.getPath(8);
  std::cout << "Path to opposite corner: ";
  for (int i = 0; i < path.getSize(); i++)
  {
    std::cout << path[i];
  }
  std::cout << std::endl;

  assert(path.getSize() == 4); // Should be 4 moves

  std::cout << "✓ Basic functionality tests passed!\n";
}

void test_small_open_grid()
{
  std::cout << "\n=== Testing Small Open Grid (No Doors) ===\n";

  // Create a 2x2 open grid
  Board board;
  board.initialize(2, 2);
  Array<Door> doors;

  // All cells are already CELL_FLOOR by default
  board.computeTimeModuloL(doors); // No doors, L = 1

  PlayerPathfinder pathfinder;
  pathfinder.initialize(board, doors);

  // Test from each corner to every other position
  for (int start = 0; start < 4; start++)
  {
    pathfinder.findPaths(start, 0);

    for (int target = 0; target < 4; target++)
    {
      int distance = pathfinder.getDistance(target);
      std::cout << "Distance from " << start << " to " << target << ": " << distance << std::endl;

      if (start == target)
      {
        assert(distance == 0);
      }
      else
      {
        assert(distance >= 1 && distance <= 3); // Max distance in 2x2 grid
      }
    }
  }

  std::cout << "✓ Small open grid tests passed!\n";
}

void test_grid_with_door()
{
  std::cout << "\n=== Testing Grid with One Door (open=1, close=1) ===\n";

  // Create a 3x3 grid with a door in the middle
  Board board;
  board.initialize(3, 3);
  Array<Door> doors;

  // Set up layout:
  // . . .
  // . D .  (D = door)
  // . . .
  // All cells are CELL_FLOOR by default

  // Add door at position 4 (center)
  Door door(1, 1, 1, 0, true); // Door ID 1, open=1, close=1, phase=0, starts open
  doors.push_back(door);
  board.set_door(1, 1, 1); // Set door ID 1 at position (1,1)

  board.computeTimeModuloL(doors); // Compute L = 2

  PlayerPathfinder pathfinder;
  pathfinder.initialize(board, doors);

  // Test pathfinding at different times
  std::cout << "Door cycle: L = " << board.getTimeModuloL() << std::endl;
  std::cout << "Door open at time 0: " << door.isOpenAtTime(0) << std::endl;
  std::cout << "Door open at time 1: " << door.isOpenAtTime(1) << std::endl;

  // Start from top-left at time 0
  pathfinder.findPaths(0, 0);

  std::cout << "\nDistances from position 0 at time 0:\n";
  for (int pos = 0; pos < 9; pos++)
  {
    for (int t = 0; t < board.getTimeModuloL(); t++)
    {
      int dist = pathfinder.getDistance(pos, t);
      std::cout << "Pos " << pos << " at time " << t << ": " << dist << std::endl;
    }
  }

  // Test specific cases
  int dist_through_door_when_closed = pathfinder.getDistance(8, 0); // Check arrival time
  int dist_through_door_when_open = pathfinder.getDistance(8, 1);   // Check arrival time

  std::cout << "\nDistance to opposite corner when arriving at time 0: " << dist_through_door_when_closed << std::endl;
  std::cout << "Distance to opposite corner when arriving at time 1: " << dist_through_door_when_open << std::endl;

  // The pathfinder should find a way around or wait for the door
  assert(dist_through_door_when_closed >= 4 || dist_through_door_when_closed == -1);
  assert(dist_through_door_when_open >= 4 || dist_through_door_when_open == -1);

  std::cout << "✓ Grid with door tests passed!\n";
}

void test_complex_door_timing()
{
  std::cout << "\n=== Testing Complex Door Timing ===\n";

  // Create a corridor with multiple doors
  Board board;
  board.initialize(5, 1); // 5x1 corridor (width=5, height=1)
  Array<Door> doors;

  // Layout: . D D D . (horizontal corridor)
  // All cells are CELL_FLOOR by default

  // Add doors with different timing
  Door door1(1, 2, 1, 0, true); // Door 1: open=2, close=1, starts open
  Door door2(2, 1, 2, 0, true); // Door 2: open=1, close=2, starts open
  Door door3(3, 1, 1, 0, true); // Door 3: open=1, close=1, starts open

  doors.push_back(door1);
  doors.push_back(door2);
  doors.push_back(door3);

  board.set_door(0, 1, 1); // Door 1 at position (0,1)
  board.set_door(0, 2, 2); // Door 2 at position (0,2)
  board.set_door(0, 3, 3); // Door 3 at position (0,3)

  board.computeTimeModuloL(doors);

  std::cout << "Complex corridor L = " << board.getTimeModuloL() << std::endl;

  // Print door states over time
  for (int t = 0; t < board.getTimeModuloL() && t < 8; t++)
  {
    std::cout << "Time " << t << ": Door1=" << door1.isOpenAtTime(t)
              << " Door2=" << door2.isOpenAtTime(t)
              << " Door3=" << door3.isOpenAtTime(t) << std::endl;
  }

  PlayerPathfinder pathfinder;
  pathfinder.initialize(board, doors);
  pathfinder.findPaths(0, 0); // Start at left end

  std::cout << "\nDistances to right end (position 4):\n";
  for (int t = 0; t < board.getTimeModuloL() && t < 8; t++)
  {
    int dist = pathfinder.getDistance(4, t);
    std::cout << "Time " << t << ": " << dist << std::endl;
  }

  int min_dist = pathfinder.getDistance(4);
  std::cout << "Minimum distance to end: " << min_dist << std::endl;

  if (min_dist != -1)
  {
    Array<char> path = pathfinder.getPath(4);
    std::cout << "Optimal path: ";
    for (int i = 0; i < path.getSize(); i++)
    {
      std::cout << path[i];
    }
    std::cout << std::endl;
  }

  std::cout << "✓ Complex door timing tests completed!\n";
}

void test_unreachable_positions()
{
  std::cout << "\n=== Testing Unreachable Positions ===\n";

  // Create a board with isolated sections
  Board board;
  board.initialize(3, 3);
  Array<Door> doors;

  // Layout:
  // . W .
  // W W W
  // . W .
  // Set walls to create isolated sections
  board.set_wall(0, 1); // Top middle
  board.set_wall(1, 0); // Middle left
  board.set_wall(1, 1); // Center
  board.set_wall(1, 2); // Middle right
  board.set_wall(2, 1); // Bottom middle

  board.computeTimeModuloL(doors); // No doors, L = 1

  PlayerPathfinder pathfinder;
  pathfinder.initialize(board, doors);
  pathfinder.findPaths(0, 0); // Start at top-left corner

  // Check that other corners are unreachable
  std::cout << "Distance to position 2 (top-right): " << pathfinder.getDistance(2) << std::endl;
  std::cout << "Distance to position 6 (bottom-left): " << pathfinder.getDistance(6) << std::endl;
  std::cout << "Distance to position 8 (bottom-right): " << pathfinder.getDistance(8) << std::endl;

  assert(pathfinder.getDistance(2) == -1);
  assert(pathfinder.getDistance(6) == -1);
  assert(pathfinder.getDistance(8) == -1);

  std::cout << "✓ Unreachable position tests passed!\n";
}

int main()
{
  std::cout << "=== PlayerPathfinder Test Suite ===\n";

  test_basic_functionality();
  test_small_open_grid();
  test_grid_with_door();
  test_complex_door_timing();
  test_unreachable_positions();

  std::cout << "\n🎉 All PlayerPathfinder tests passed! 🎉\n";
  return 0;
}