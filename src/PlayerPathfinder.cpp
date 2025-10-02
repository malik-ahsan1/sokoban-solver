#include "PlayerPathfinder.h"
#include <iostream>

PlayerPathfinder::PlayerPathfinder()
    : board(nullptr), doors(nullptr)
{
}

void PlayerPathfinder::initialize(const Board &b, const Array<Door> &d)
{
  board = &b;
  doors = &d;
  initializeTables();
  // Clear any previously set blocked positions
  while (blockedPositions.getSize() > 0)
  {
    blockedPositions.pop_back();
  }
}

void PlayerPathfinder::setBlockedPositions(const Array<int> &blocked)
{
  // Clear existing blocked positions
  while (blockedPositions.getSize() > 0)
  {
    blockedPositions.pop_back();
  }
  // Copy new blocked positions
  for (int i = 0; i < blocked.getSize(); i++)
  {
    blockedPositions.push_back(blocked[i]);
  }
}

void PlayerPathfinder::initializeTables()
{
  if (!board)
    return;

  int L = board->getTimeModuloL();
  int board_size = board->get_width() * board->get_height();

  // Initialize 2D tables
  dist_table.resize(board_size);
  parent_pos.resize(board_size);
  parent_step.resize(board_size);
  parent_move.resize(board_size);

  for (int pos = 0; pos < board_size; pos++)
  {
    dist_table[pos].resize(L);
    parent_pos[pos].resize(L);
    parent_step[pos].resize(L);
    parent_move[pos].resize(L);

    // Initialize all distances to infinity
    for (int t = 0; t < L; t++)
    {
      dist_table[pos][t] = INT_MAX;
      parent_pos[pos][t] = -1;
      parent_step[pos][t] = -1;
      parent_move[pos][t] = '\0';
    }
  }
}

bool PlayerPathfinder::isDoorOpen(int pos, int step_mod_L) const
{
  if (!board)
    return false;

  // Get door ID at this position
  int door_id = board->get_door_id_idx(pos);
  if (door_id == -1 || door_id == 0) // -1 or 0 means no door
    return true;                     // No door means it's open

  // If we have doors array, find door with this ID
  if (!doors)
    return false; // Door ID exists but no doors array

  for (int i = 0; i < doors->getSize(); i++)
  {
    if ((*doors)[i].id == door_id)
    {
      return (*doors)[i].isOpenAtTime(step_mod_L);
    }
  }

  return false; // Door ID not found
}

bool PlayerPathfinder::isPassable(int pos, int step_mod_L) const
{
  if (!board)
    return false;

  // Check if position is within bounds and not a wall
  if (pos < 0 || pos >= board->get_width() * board->get_height())
    return false;

  if (board->is_wall_idx(pos))
    return false;

  // Check if position is blocked (e.g., by a box)
  for (int i = 0; i < blockedPositions.getSize(); i++)
  {
    if (blockedPositions[i] == pos)
      return false;
  }

  // Check if any door at this position is open
  return isDoorOpen(pos, step_mod_L);
}

void PlayerPathfinder::expandNeighbors(const Node &current, std::queue<Node> &bfs_queue)
{
  if (!board)
    return;

  int rows = board->get_height();
  int cols = board->get_width();
  int L = board->getTimeModuloL();

  // Get current position coordinates
  int row = current.pos / cols;
  int col = current.pos % cols;

  // Direction vectors: up, down, left, right
  int dr[] = {-1, 1, 0, 0};
  int dc[] = {0, 0, -1, 1};
  char moves[] = {'U', 'D', 'L', 'R'};

  for (int i = 0; i < 4; i++)
  {
    int new_row = row + dr[i];
    int new_col = col + dc[i];

    // Check bounds
    if (new_row < 0 || new_row >= rows || new_col < 0 || new_col >= cols)
      continue;

    int neighbor = new_row * cols + new_col;
    int next_step = (current.step_mod_L + 1) % L;

    // Check if neighbor is passable at next time
    if (!isPassable(neighbor, next_step))
      continue;

    // Check if we found a shorter path
    int new_dist = current.dist + 1;
    if (new_dist < dist_table[neighbor][next_step])
    {
      dist_table[neighbor][next_step] = new_dist;
      parent_pos[neighbor][next_step] = current.pos;
      parent_step[neighbor][next_step] = current.step_mod_L;
      parent_move[neighbor][next_step] = moves[i];

      bfs_queue.push(Node(neighbor, next_step, new_dist));
    }
  }
}

void PlayerPathfinder::findPaths(int start_pos, int start_step_mod_L)
{
  if (!board || !isPassable(start_pos, start_step_mod_L))
    return;

  // Reset all distances
  initializeTables();

  // BFS queue
  std::queue<Node> bfs_queue;

  // Start node
  dist_table[start_pos][start_step_mod_L] = 0;
  bfs_queue.push(Node(start_pos, start_step_mod_L, 0));

  while (!bfs_queue.empty())
  {
    Node current = bfs_queue.front();
    bfs_queue.pop();

    // Skip if we've found a better path already
    if (current.dist > dist_table[current.pos][current.step_mod_L])
      continue;

    expandNeighbors(current, bfs_queue);
  }
}

int PlayerPathfinder::getDistance(int target_pos) const
{
  if (!board || target_pos < 0 || target_pos >= board->get_width() * board->get_height())
    return -1;

  int min_dist = INT_MAX;
  int L = board->getTimeModuloL();

  for (int t = 0; t < L; t++)
  {
    if (dist_table[target_pos][t] < min_dist)
    {
      min_dist = dist_table[target_pos][t];
    }
  }

  return (min_dist == INT_MAX) ? -1 : min_dist;
}

int PlayerPathfinder::getDistance(int target_pos, int target_step_mod_L) const
{
  if (!board || target_pos < 0 || target_pos >= board->get_width() * board->get_height())
    return -1;
  if (target_step_mod_L < 0 || target_step_mod_L >= board->getTimeModuloL())
    return -1;

  int dist = dist_table[target_pos][target_step_mod_L];
  return (dist == INT_MAX) ? -1 : dist;
}

Array<char> PlayerPathfinder::getPath(int target_pos) const
{
  Array<char> empty_path;
  if (!board)
    return empty_path;

  // Find the minimum distance time slot
  int min_dist = INT_MAX;
  int best_time = -1;
  int L = board->getTimeModuloL();

  for (int t = 0; t < L; t++)
  {
    if (dist_table[target_pos][t] < min_dist)
    {
      min_dist = dist_table[target_pos][t];
      best_time = t;
    }
  }

  if (best_time == -1 || min_dist == INT_MAX)
    return empty_path;

  return getPath(target_pos, best_time);
}

Array<char> PlayerPathfinder::getPath(int target_pos, int target_step_mod_L) const
{
  Array<char> path;

  if (!board || getDistance(target_pos, target_step_mod_L) == -1)
    return path;

  // Reconstruct path backwards
  Array<char> reverse_path;
  int pos = target_pos;
  int step = target_step_mod_L;

  while (parent_pos[pos][step] != -1)
  {
    reverse_path.push_back(parent_move[pos][step]);
    int next_pos = parent_pos[pos][step];
    int next_step = parent_step[pos][step];
    pos = next_pos;
    step = next_step;
  }

  // Reverse the path to get forward direction
  for (int i = reverse_path.getSize() - 1; i >= 0; i--)
  {
    path.push_back(reverse_path[i]);
  }

  return path;
}

void PlayerPathfinder::printDistanceTable() const
{
  if (!board)
    return;

  std::cout << "Distance Table:\n";
  int rows = board->get_height();
  int cols = board->get_width();
  int L = board->getTimeModuloL();

  for (int t = 0; t < L; t++)
  {
    std::cout << "Time " << t << ":\n";
    for (int r = 0; r < rows; r++)
    {
      for (int c = 0; c < cols; c++)
      {
        int pos = r * cols + c;
        int dist = dist_table[pos][t];
        if (dist == INT_MAX)
        {
          std::cout << "INF ";
        }
        else
        {
          std::cout << dist << " ";
        }
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }
}