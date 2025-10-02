#pragma once

#include "Array.h"
#include "Board.h"
#include "State.h"
#include <queue>
#include <climits>

// Precomputed distances structure for Hungarian algorithm
struct PrecomputedDistances
{
  Array<Array<int>> goalDist; // goalDist[goalIndex][cell]
  Array<int> goalPositions;   // List of goal positions

  PrecomputedDistances() {}

  int getNumGoals() const { return goalPositions.getSize(); }
  int getGoalPosition(int idx) const { return goalPositions[idx]; }
  int getDistance(int goalIdx, int cell) const { return goalDist[goalIdx][cell]; }
};

// Sokoban heuristics and deadlock detection
class SokobanHeuristics
{
private:
  const Board *board;

  // Precomputed BFS distances from each goal to all cells
  PrecomputedDistances precomputed;

  // Precomputed corner deadlock detection
  Array<bool> isCorner;       // isCorner[cell]
  Array<bool> isGoalPosition; // Quick lookup: isGoalPosition[cell]

public:
  SokobanHeuristics();
  ~SokobanHeuristics() = default;

  // Initialize heuristics with board
  void initialize(const Board &b);

  // Heuristic functions
  int heuristic(const State &s) const;        // Uses Hungarian assignment
  int heuristic_simple(const State &s) const; // Original sum of minimums
  int heuristic_assignment(const State &s, const Board &board, const PrecomputedDistances &precomp) const;

  // Deadlock detection (All 4 Stages)
  bool isDeadlocked(const State &s) const;
  bool isDeadlocked(const State &s, const Array<Door> &doors) const;          // With door information
  bool isCornerDeadlocked(const State &s) const;                              // Stage 1
  bool isWallDeadlocked(const State &s) const;                                // Stage 2
  bool isMultiboxDeadlocked(const State &s) const;                            // Stage 3
  bool isDoorCycleDeadlocked(const State &s, const Array<Door> &doors) const; // Stage 4

  // Debug methods
  void printGoalDistances() const;
  void printCornerMap() const;

  // Access precomputed data
  const PrecomputedDistances &getPrecomputedDistances() const { return precomputed; }

  // Test accessors (for testing individual stages)
  bool test_isCorridorDeadlock(const State &s) const { return isCorridorDeadlock(s); }
  bool test_isRegionCapacityDeadlock(const State &s) const { return isRegionCapacityDeadlock(s); }

private:
  // Helper methods for initialization
  void precomputeGoalDistances();
  void precomputeCorners();
  void runBFSFromGoal(int goalIndex, int goalPos);
  bool isCellCorner(int pos) const;

  // Hungarian algorithm implementation
  int hungarianAlgorithm(const Array<Array<int>> &costMatrix, int n, int m) const;
  void buildCostMatrix(const State &s, Array<Array<int>> &costMatrix) const;

  // Wall deadlock detection helpers
  bool isWallDeadlock(const State &s) const;
  bool isBoxAgainstWallWithoutGoals(int boxPos) const;
  void checkWallLine(int pos, int direction, bool &hasGoal) const;

  // Multi-box deadlock detection (Stage 3)
  bool is_multibox_deadlock(const State &state, const Board &board) const;
  bool isCorridorDeadlock(const State &s) const;
  bool isRegionCapacityDeadlock(const State &s) const;
  void floodFillRegion(int pos, Array<bool> &visited, Array<int> &region, int &boxCount, int &goalCount) const;
  void floodFillRegionWithBoxes(int pos, const State &state, Array<bool> &visited, Array<int> &region, int &boxCount, int &goalCount) const;
  bool isCorridor(int pos, int &corridorStart, int &corridorEnd, int &direction) const;

  // Door-cycle deadlock detection (Stage 4)
  bool is_door_cycle_deadlock(const State &state, const Board &board, const Array<Door> &doors) const;
  void findUnreachableGoals(const Array<Door> &doors, Array<bool> &unreachable) const;
};