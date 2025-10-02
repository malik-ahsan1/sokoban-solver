#pragma once

#include "Board.h"
#include "Door.h"
#include "Array.h"
#include <queue>
#include <climits>

// Node structure for BFS pathfinding
struct Node
{
  int pos;        // Cell index on the board
  int step_mod_L; // Time modulo L
  int dist;       // Steps taken to reach this node

  Node() : pos(-1), step_mod_L(0), dist(0) {}
  Node(int position, int stepMod, int distance) : pos(position), step_mod_L(stepMod), dist(distance) {}
};

// Player pathfinder with door timing awareness
class PlayerPathfinder
{
private:
  const Board *board;
  const Array<Door> *doors;

  // 2D distance array: dist[pos][step_mod_L]
  Array<Array<int>> dist_table;

  // Parent tracking for path reconstruction
  Array<Array<int>> parent_pos;
  Array<Array<int>> parent_step;
  Array<Array<char>> parent_move;

  // Helper methods
  void initializeTables();
  bool isDoorOpen(int pos, int step_mod_L) const;
  bool isPassable(int pos, int step_mod_L) const;
  void expandNeighbors(const Node &current, std::queue<Node> &bfs_queue);

public:
  PlayerPathfinder();
  ~PlayerPathfinder() = default;

  // Initialize pathfinder with board and doors
  void initialize(const Board &b, const Array<Door> &d);

  // Run BFS from starting position and time
  void findPaths(int start_pos, int start_step_mod_L);

  // Query results
  int getDistance(int target_pos) const;                        // Returns minimum distance across all times
  int getDistance(int target_pos, int target_step_mod_L) const; // Specific time

  // Path reconstruction
  Array<char> getPath(int target_pos) const; // Returns path as array of moves ('U','D','L','R')
  Array<char> getPath(int target_pos, int target_step_mod_L) const;

  // Debug methods
  void printDistanceTable() const;
};