#ifndef SOLVER_ASTAR_H
#define SOLVER_ASTAR_H

#include "Array.h"
#include "Board.h"
#include "State.h"
#include "Door.h"
#include "Meta.h"
#include "Zobrist.h"
#include "BinaryHeap.h"
#include "HashTable.h"
#include "SokobanHeuristics.h"
#include "SuccessorGenerator.h"
#include "ConfigParser.h"
#include "SolverStats.h"
#include <chrono>
#include <unordered_map>

// A* search node containing state and cost information
struct AStarNode
{
  State state;
  int g; // Cost from start
  int h; // Heuristic cost to goal
  int f; // Total cost (g + h)
  State *parent;
  Array<char> actionsFromParent;

  AStarNode() : g(0), h(0), f(0), parent(nullptr) {}

  AStarNode(const State &s, int g_cost, int h_cost, State *p = nullptr)
      : state(s), g(g_cost), h(h_cost), f(g_cost + h_cost), parent(p) {}

  // Comparison for BinaryHeap (min-heap by f, tie-break by h)
  bool operator<(const AStarNode &other) const
  {
    if (f != other.f)
    {
      return f < other.f; // Smaller f has higher priority
    }
    return h < other.h; // Tie-break by smaller h
  }

  bool operator>(const AStarNode &other) const
  {
    return other < *this;
  }

  bool operator<=(const AStarNode &other) const
  {
    return !(*this > other);
  }

  bool operator>=(const AStarNode &other) const
  {
    return !(*this < other);
  }
};

class SolverAStar
{
private:
  const Board &board;
  const Array<Door> &doors;
  const Meta &meta;
  Zobrist &zobrist;

  SokobanHeuristics heuristics;

  // Hash table for closed set: maps state hash to best g-cost
  std::unordered_map<uint64_t, int> closedSet;

  // Open set implemented as binary heap
  BinaryHeap<AStarNode> openSet;

  // Node storage to maintain valid parent pointers
  Array<AStarNode *> allNodes;

  // Statistics tracking
  SolverStats currentStats;
  std::chrono::steady_clock::time_point startTime;

  // Helper function to generate successors with move sequences
  void generateSuccessorsWithMoves(const AStarNode *parentNode, Array<AStarNode *> &successorNodes);

  // Helper methods
  bool isGoal(const State &state) const;
  void reconstructPath(const AStarNode &goalNode, Array<char> &outMoves) const;
  int computeMoveCost(const State &from, const State &to) const;
  void updateStats();

public:
  // Constructor
  SolverAStar(const Board &board, const Array<Door> &doors, const Meta &meta, Zobrist &zobrist);

  // Destructor
  ~SolverAStar();

  // Main solving method
  bool solve(const InitialDynamicState &init, Array<char> &outMoves, SolverStats &stats);

  // Utility methods for testing
  const SolverStats &getLastStats() const { return currentStats; }
};

#endif // SOLVER_ASTAR_H