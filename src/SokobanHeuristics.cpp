#include "SokobanHeuristics.h"
#include <iostream>
#include <iomanip>

SokobanHeuristics::SokobanHeuristics()
    : board(nullptr)
{
}

void SokobanHeuristics::initialize(const Board &b)
{
  board = &b;

  int board_size = board->get_width() * board->get_height();

  // Initialize arrays
  precomputed.goalPositions = Array<int>();
  isGoalPosition = Array<bool>();
  isCorner = Array<bool>();

  // Initialize boolean arrays for board positions
  for (int i = 0; i < board_size; i++)
  {
    isGoalPosition.push_back(false);
    isCorner.push_back(false);
  }

  // Find all goal positions
  for (int pos = 0; pos < board_size; pos++)
  {
    if (board->is_target_idx(pos))
    {
      precomputed.goalPositions.push_back(pos);
      isGoalPosition[pos] = true;
    }
  }

  std::cout << "Found " << precomputed.goalPositions.getSize() << " goal positions" << std::endl;

  // Precompute goal distances and corner detection
  precomputeGoalDistances();
  precomputeCorners();
}

void SokobanHeuristics::precomputeGoalDistances()
{
  if (!board || precomputed.goalPositions.getSize() == 0)
  {
    std::cerr << "Warning: Cannot precompute goal distances - no board or goals" << std::endl;
    return;
  }

  int board_size = board->get_width() * board->get_height();
  int num_goals = precomputed.goalPositions.getSize();

  // Initialize goalDist array
  precomputed.goalDist = Array<Array<int>>();

  // Create goalDist[g] arrays for each goal
  for (int g = 0; g < num_goals; g++)
  {
    Array<int> goal_array;
    // Initialize all distances to infinity
    for (int pos = 0; pos < board_size; pos++)
    {
      goal_array.push_back(INT_MAX);
    }
    precomputed.goalDist.push_back(goal_array);
  }

  // Run BFS from each goal
  for (int g = 0; g < num_goals; g++)
  {
    runBFSFromGoal(g, precomputed.goalPositions[g]);
  }

  std::cout << "Precomputed BFS distances from " << num_goals << " goals" << std::endl;
}

void SokobanHeuristics::runBFSFromGoal(int goalIndex, int goalPos)
{
  if (!board)
    return;

  int width = board->get_width();
  int height = board->get_height();

  std::queue<int> bfs_queue;

  // Start BFS from goal position
  precomputed.goalDist[goalIndex][goalPos] = 0;
  bfs_queue.push(goalPos);

  // Direction vectors: up, down, left, right
  int dr[] = {-1, 1, 0, 0};
  int dc[] = {0, 0, -1, 1};

  while (!bfs_queue.empty())
  {
    int current_pos = bfs_queue.front();
    bfs_queue.pop();

    int current_dist = precomputed.goalDist[goalIndex][current_pos];

    // Get current coordinates
    int row = current_pos / width;
    int col = current_pos % width;

    // Explore all 4 directions
    for (int dir = 0; dir < 4; dir++)
    {
      int new_row = row + dr[dir];
      int new_col = col + dc[dir];

      // Check bounds
      if (new_row < 0 || new_row >= height || new_col < 0 || new_col >= width)
        continue;

      int new_pos = new_row * width + new_col;

      // Check if position is passable (not a wall)
      if (board->is_wall_idx(new_pos))
        continue;

      // Check if we found a shorter path
      int new_dist = current_dist + 1;
      if (new_dist < precomputed.goalDist[goalIndex][new_pos])
      {
        precomputed.goalDist[goalIndex][new_pos] = new_dist;
        bfs_queue.push(new_pos);
      }
    }
  }
}

void SokobanHeuristics::precomputeCorners()
{
  if (!board)
    return;

  int board_size = board->get_width() * board->get_height();

  // Check each position to see if it's a corner
  for (int pos = 0; pos < board_size; pos++)
  {
    // Skip walls - they can't be corners where boxes get stuck
    if (board->is_wall_idx(pos))
      continue;

    isCorner[pos] = isCellCorner(pos);
  }

  std::cout << "Precomputed corner detection for deadlock analysis" << std::endl;
}

bool SokobanHeuristics::isCellCorner(int pos) const
{
  if (!board || board->is_wall_idx(pos))
    return false;

  int width = board->get_width();
  int height = board->get_height();
  int row = pos / width;
  int col = pos % width;

  // Check walls in perpendicular directions
  // A corner has walls in two perpendicular directions

  bool wall_up = (row == 0) || board->is_wall_idx((row - 1) * width + col);
  bool wall_down = (row == height - 1) || board->is_wall_idx((row + 1) * width + col);
  bool wall_left = (col == 0) || board->is_wall_idx(row * width + (col - 1));
  bool wall_right = (col == width - 1) || board->is_wall_idx(row * width + (col + 1));

  // Check for corners (walls in perpendicular directions)
  if ((wall_up || wall_down) && (wall_left || wall_right))
  {
    return true;
  }

  return false;
}

int SokobanHeuristics::heuristic(const State &s) const
{
  // Use Hungarian assignment heuristic (more accurate)
  return heuristic_assignment(s, *board, precomputed);
}

int SokobanHeuristics::heuristic_simple(const State &s) const
{
  if (!board || precomputed.goalPositions.getSize() == 0)
    return 0;

  int total_heuristic = 0;

  // For each box, find minimum distance to any goal
  for (int box_idx = 0; box_idx < s.getBoxCount(); box_idx++)
  {
    const BoxInfo &box = s.getBox(box_idx);
    int box_pos = box.pos;

    int min_dist_to_goal = INT_MAX;

    // Check distance to each goal
    for (int g = 0; g < precomputed.goalPositions.getSize(); g++)
    {
      if (precomputed.goalDist[g][box_pos] < min_dist_to_goal)
      {
        min_dist_to_goal = precomputed.goalDist[g][box_pos];
      }
    }

    // If box is unreachable to any goal, heuristic is infinite (inadmissible but catches impossible states)
    if (min_dist_to_goal == INT_MAX)
    {
      // Return large value but not INT_MAX to avoid overflow
      return 999999;
    }

    total_heuristic += min_dist_to_goal;
  }

  return total_heuristic;
}

int SokobanHeuristics::heuristic_assignment(const State &s, const Board &board, const PrecomputedDistances &precomp) const
{
  int num_boxes = s.getBoxCount();
  int num_goals = precomp.getNumGoals();

  if (num_boxes == 0)
    return 0;

  if (num_goals == 0)
    return 999999; // No goals available

  // If more boxes than goals, return large penalty
  if (num_boxes > num_goals)
    return 999999;

  // Build cost matrix: cost[i][j] = distance from box[i] to goal[j]
  Array<Array<int>> costMatrix;
  buildCostMatrix(s, costMatrix);

  // Use Hungarian algorithm to find optimal assignment
  int optimal_cost = hungarianAlgorithm(costMatrix, num_boxes, num_goals);

  return optimal_cost;
}

bool SokobanHeuristics::isDeadlocked(const State &s) const
{
  if (!board)
    return false;

  // Stage 1: Corner deadlock detection
  if (isCornerDeadlocked(s))
    return true;

  // Stage 2: Wall deadlock detection
  if (isWallDeadlocked(s))
    return true;

  // Stage 3: Multi-box deadlock detection
  if (isMultiboxDeadlocked(s))
    return true;

  return false;
}

bool SokobanHeuristics::isDeadlocked(const State &s, const Array<Door> &doors) const
{
  if (!board)
    return false;

  // Stage 1: Corner deadlock detection
  if (isCornerDeadlocked(s))
    return true;

  // Stage 2: Wall deadlock detection
  if (isWallDeadlocked(s))
    return true;

  // Stage 3: Multi-box deadlock detection
  if (isMultiboxDeadlocked(s))
    return true;

  // Stage 4: Door-cycle deadlock detection
  if (isDoorCycleDeadlocked(s, doors))
    return true;

  return false;
}

bool SokobanHeuristics::isCornerDeadlocked(const State &s) const
{
  if (!board)
    return false;

  // Stage 1: Corner deadlock detection
  for (int box_idx = 0; box_idx < s.getBoxCount(); box_idx++)
  {
    const BoxInfo &box = s.getBox(box_idx);
    int box_pos = box.pos;

    // Check if box is in a corner
    if (isCorner[box_pos])
    {
      // Check if this corner position is a goal
      if (!isGoalPosition[box_pos])
      {
        // Box is in a non-goal corner - this is a deadlock!
        return true;
      }
    }
  }

  return false;
}

bool SokobanHeuristics::isWallDeadlocked(const State &s) const
{
  if (!board)
    return false;

  // Stage 2: Wall deadlock detection
  // Only detect wall deadlock if box is in a corner (handled by corner deadlock)
  // or if box is provably unreachable from all goals
  for (int box_idx = 0; box_idx < s.getBoxCount(); box_idx++)
  {
    const BoxInfo &box = s.getBox(box_idx);
    int box_pos = box.pos;

    // Skip if box is already on a goal
    if (isGoalPosition[box_pos])
      continue;

    // Check if box has infinite distance to all goals
    // This means it's unreachable from any goal
    bool unreachable_from_all_goals = true;
    for (int goal_idx = 0; goal_idx < precomputed.getNumGoals(); goal_idx++)
    {
      int distance = precomputed.getDistance(goal_idx, box_pos);
      if (distance != INT_MAX)
      {
        unreachable_from_all_goals = false;
        break;
      }
    }

    if (unreachable_from_all_goals)
    {
      return true; // Box cannot reach any goal
    }
  }

  return false;
}

void SokobanHeuristics::printGoalDistances() const
{
  if (!board || precomputed.goalPositions.getSize() == 0)
  {
    std::cout << "No goal distances to print" << std::endl;
    return;
  }

  int width = board->get_width();
  int height = board->get_height();

  for (int g = 0; g < precomputed.goalPositions.getSize(); g++)
  {
    std::cout << "\nDistances from goal " << g << " (position " << precomputed.goalPositions[g] << "):" << std::endl;

    for (int row = 0; row < height; row++)
    {
      for (int col = 0; col < width; col++)
      {
        int pos = row * width + col;
        if (board->is_wall_idx(pos))
        {
          std::cout << "## ";
        }
        else if (precomputed.goalDist[g][pos] == INT_MAX)
        {
          std::cout << "∞  ";
        }
        else
        {
          std::cout << std::setfill('0') << std::setw(2) << precomputed.goalDist[g][pos] << " ";
        }
      }
      std::cout << std::endl;
    }
  }
}

void SokobanHeuristics::printCornerMap() const
{
  if (!board)
  {
    std::cout << "No board to print corner map" << std::endl;
    return;
  }

  int width = board->get_width();
  int height = board->get_height();

  std::cout << "\nCorner map (C = corner, . = goal, # = wall, space = floor):" << std::endl;

  for (int row = 0; row < height; row++)
  {
    for (int col = 0; col < width; col++)
    {
      int pos = row * width + col;

      if (board->is_wall_idx(pos))
      {
        std::cout << '#';
      }
      else if (isGoalPosition[pos])
      {
        std::cout << '.';
      }
      else if (isCorner[pos])
      {
        std::cout << 'C';
      }
      else
      {
        std::cout << ' ';
      }
    }
    std::cout << std::endl;
  }
}

// Build cost matrix for Hungarian algorithm
void SokobanHeuristics::buildCostMatrix(const State &s, Array<Array<int>> &costMatrix) const
{
  int num_boxes = s.getBoxCount();
  int num_goals = precomputed.getNumGoals();

  costMatrix = Array<Array<int>>();

  for (int box_idx = 0; box_idx < num_boxes; box_idx++)
  {
    Array<int> row;
    const BoxInfo &box = s.getBox(box_idx);
    int box_pos = box.pos;

    for (int goal_idx = 0; goal_idx < num_goals; goal_idx++)
    {
      int distance = precomputed.getDistance(goal_idx, box_pos);
      if (distance == INT_MAX)
      {
        distance = 999999; // Use large but finite value
      }
      row.push_back(distance);
    }
    costMatrix.push_back(row);
  }
}

// Simplified Hungarian algorithm implementation
int SokobanHeuristics::hungarianAlgorithm(const Array<Array<int>> &costMatrix, int n, int m) const
{
  if (n == 0)
    return 0;

  // For small problems, use brute force approach
  if (n <= 3 && m <= 3)
  {
    // Try all possible assignments and find minimum
    int min_cost = INT_MAX;

    // Generate all permutations of goal assignments
    Array<int> assignment;
    for (int i = 0; i < n; i++)
    {
      assignment.push_back(i);
    }

    // Simple brute force for small cases
    if (n == 1)
    {
      min_cost = costMatrix[0][0];
      for (int j = 1; j < m; j++)
      {
        if (costMatrix[0][j] < min_cost)
          min_cost = costMatrix[0][j];
      }
    }
    else if (n == 2 && m >= 2)
    {
      // Try both assignments: (0,0)(1,1) vs (0,1)(1,0)
      for (int j1 = 0; j1 < m; j1++)
      {
        for (int j2 = 0; j2 < m; j2++)
        {
          if (j1 != j2) // Different goals
          {
            int cost = costMatrix[0][j1] + costMatrix[1][j2];
            if (cost < min_cost)
              min_cost = cost;
          }
        }
      }
    }
    else if (n == 3 && m >= 3)
    {
      // Try all 3! = 6 permutations
      for (int j1 = 0; j1 < m; j1++)
      {
        for (int j2 = 0; j2 < m; j2++)
        {
          for (int j3 = 0; j3 < m; j3++)
          {
            if (j1 != j2 && j1 != j3 && j2 != j3)
            {
              int cost = costMatrix[0][j1] + costMatrix[1][j2] + costMatrix[2][j3];
              if (cost < min_cost)
                min_cost = cost;
            }
          }
        }
      }
    }

    return (min_cost == INT_MAX) ? 999999 : min_cost;
  }

  // For larger problems, fall back to greedy approximation
  // This is not optimal but provides a reasonable bound
  Array<bool> used_goals;
  for (int i = 0; i < m; i++)
  {
    used_goals.push_back(false);
  }

  int total_cost = 0;

  for (int box_idx = 0; box_idx < n; box_idx++)
  {
    int best_cost = INT_MAX;
    int best_goal = -1;

    // Find cheapest available goal for this box
    for (int goal_idx = 0; goal_idx < m; goal_idx++)
    {
      if (!used_goals[goal_idx] && costMatrix[box_idx][goal_idx] < best_cost)
      {
        best_cost = costMatrix[box_idx][goal_idx];
        best_goal = goal_idx;
      }
    }

    if (best_goal >= 0)
    {
      used_goals[best_goal] = true;
      total_cost += best_cost;
    }
    else
    {
      return 999999; // No valid assignment
    }
  }

  return total_cost;
}

// Wall deadlock detection helpers
bool SokobanHeuristics::isBoxAgainstWallWithoutGoals(int boxPos) const
{
  int width = board->get_width();
  int height = board->get_height();
  int row = boxPos / width;
  int col = boxPos % width;

  // Check each direction for walls
  // Direction 0: up, 1: down, 2: left, 3: right
  int dr[] = {-1, 1, 0, 0};
  int dc[] = {0, 0, -1, 1};

  for (int dir = 0; dir < 4; dir++)
  {
    int wall_row = row + dr[dir];
    int wall_col = col + dc[dir];

    // Check if adjacent to wall or border
    bool adjacent_to_wall = false;

    if (wall_row < 0 || wall_row >= height || wall_col < 0 || wall_col >= width)
    {
      adjacent_to_wall = true; // Border acts as wall
    }
    else
    {
      int wall_pos = wall_row * width + wall_col;
      if (board->is_wall_idx(wall_pos))
      {
        adjacent_to_wall = true;
      }
    }

    if (adjacent_to_wall)
    {
      // Check along the wall line in both perpendicular directions
      bool has_goal_along_wall = false;
      checkWallLine(boxPos, dir, has_goal_along_wall);

      if (!has_goal_along_wall)
      {
        return true; // Deadlock: box against wall with no goals
      }
    }
  }

  return false;
}

void SokobanHeuristics::checkWallLine(int pos, int wall_direction, bool &hasGoal) const
{
  int width = board->get_width();
  int height = board->get_height();
  int row = pos / width;
  int col = pos % width;

  hasGoal = false;

  // Determine perpendicular directions for wall line checking
  int perp_dirs[2][2];

  if (wall_direction == 0 || wall_direction == 1) // Horizontal wall (up/down)
  {
    perp_dirs[0][0] = 0;
    perp_dirs[0][1] = -1; // Left
    perp_dirs[1][0] = 0;
    perp_dirs[1][1] = 1; // Right
  }
  else // Vertical wall (left/right)
  {
    perp_dirs[0][0] = -1;
    perp_dirs[0][1] = 0; // Up
    perp_dirs[1][0] = 1;
    perp_dirs[1][1] = 0; // Down
  }

  // Check both directions along the wall
  for (int dir_idx = 0; dir_idx < 2; dir_idx++)
  {
    int check_row = row;
    int check_col = col;

    // Follow the wall line until we hit a corner or find a goal
    while (true)
    {
      check_row += perp_dirs[dir_idx][0];
      check_col += perp_dirs[dir_idx][1];

      // Check bounds
      if (check_row < 0 || check_row >= height || check_col < 0 || check_col >= width)
        break;

      int check_pos = check_row * width + check_col;

      // If we hit a wall, stop (this is a corner)
      if (board->is_wall_idx(check_pos))
        break;

      // Check if there's still a wall adjacent in the wall_direction
      int adj_row = check_row;
      int adj_col = check_col;

      if (wall_direction == 0)
        adj_row--; // Up wall
      else if (wall_direction == 1)
        adj_row++; // Down wall
      else if (wall_direction == 2)
        adj_col--; // Left wall
      else if (wall_direction == 3)
        adj_col++; // Right wall

      bool still_adjacent_to_wall = false;
      if (adj_row < 0 || adj_row >= height || adj_col < 0 || adj_col >= width)
      {
        still_adjacent_to_wall = true;
      }
      else
      {
        int adj_pos = adj_row * width + adj_col;
        if (board->is_wall_idx(adj_pos))
          still_adjacent_to_wall = true;
      }

      if (!still_adjacent_to_wall)
        break; // Wall ended

      // Check if current position is a goal
      if (isGoalPosition[check_pos])
      {
        hasGoal = true;
        return;
      }
    }
  }
}

// ========== STAGE 3: Multi-box Deadlock Detection ==========

bool SokobanHeuristics::isMultiboxDeadlocked(const State &s) const
{
  if (!board)
    return false;

  // Disable multibox deadlock for now as it has false positives
  // TODO: Improve multibox deadlock detection accuracy
  return false;
  
  // return is_multibox_deadlock(s, *board);
}

bool SokobanHeuristics::is_multibox_deadlock(const State &state, const Board &board) const
{
  // Check corridor deadlocks
  if (isCorridorDeadlock(state))
    return true;

  // Check region capacity deadlocks
  if (isRegionCapacityDeadlock(state))
    return true;

  return false;
}

bool SokobanHeuristics::isCorridorDeadlock(const State &s) const
{
  if (!board)
    return false;

  int width = board->get_width();
  int height = board->get_height();

  Array<bool> visited;
  for (int i = 0; i < width * height; i++)
  {
    visited.push_back(false);
  }

  // Check each box to see if it's in a problematic corridor
  for (int box_idx = 0; box_idx < s.getBoxCount(); box_idx++)
  {
    const BoxInfo &box = s.getBox(box_idx);
    int box_pos = box.pos;

    if (visited[box_pos])
      continue;

    // Check if this position is part of a corridor
    int corridor_start, corridor_end, direction;
    if (isCorridor(box_pos, corridor_start, corridor_end, direction))
    {
      // Count boxes and goals in this corridor
      int boxes_in_corridor = 0;
      bool has_goal_in_corridor = false;

      // Iterate through all positions between corridor_start and corridor_end
      if (direction == 0)
      {
        // Vertical corridor - iterate from start row to end row
        int start_row = corridor_start / width;
        int end_row = corridor_end / width;
        int col = corridor_start % width;

        for (int row = start_row; row <= end_row; row++)
        {
          int pos = row * width + col;
          visited[pos] = true;

          // Count boxes at this position
          for (int b = 0; b < s.getBoxCount(); b++)
          {
            if (s.getBox(b).pos == pos)
              boxes_in_corridor++;
          }

          // Check for goals
          if (isGoalPosition[pos])
            has_goal_in_corridor = true;
        }
      }
      else if (direction == 2)
      {
        // Horizontal corridor - iterate from start col to end col
        int start_col = corridor_start % width;
        int end_col = corridor_end % width;
        int row = corridor_start / width;

        for (int col = start_col; col <= end_col; col++)
        {
          int pos = row * width + col;
          visited[pos] = true;

          // Count boxes at this position
          for (int b = 0; b < s.getBoxCount(); b++)
          {
            if (s.getBox(b).pos == pos)
              boxes_in_corridor++;
          }

          // Check for goals
          if (isGoalPosition[pos])
            has_goal_in_corridor = true;
        }
      }

      // Deadlock if multiple boxes in corridor with no goals
      if (boxes_in_corridor > 1 && !has_goal_in_corridor)
      {
        return true;
      }
    }
  }

  return false;
}

bool SokobanHeuristics::isCorridor(int pos, int &corridorStart, int &corridorEnd, int &direction) const
{
  if (!board)
    return false;

  int width = board->get_width();
  int height = board->get_height();
  int row = pos / width;
  int col = pos % width;

  // Check if this is a corridor (walls on two opposite sides)
  bool wall_up = (row == 0) || board->is_wall_idx((row - 1) * width + col);
  bool wall_down = (row == height - 1) || board->is_wall_idx((row + 1) * width + col);
  bool wall_left = (col == 0) || board->is_wall_idx(row * width + (col - 1));
  bool wall_right = (col == width - 1) || board->is_wall_idx(row * width + (col + 1));

  // Horizontal corridor (walls above and below)
  if (wall_up && wall_down && !wall_left && !wall_right)
  {
    direction = 2; // Left-right corridor

    // Find start of corridor (leftmost passable cell)
    corridorStart = pos;
    while (col > 0)
    {
      int left_pos = row * width + (col - 1);
      if (board->is_wall_idx(left_pos))
        break;
      col--;
      corridorStart = left_pos;
    }

    // Find end of corridor (rightmost passable cell)
    col = pos % width;
    corridorEnd = pos;
    while (col < width - 1)
    {
      int right_pos = row * width + (col + 1);
      if (board->is_wall_idx(right_pos))
        break;
      col++;
      corridorEnd = right_pos;
    }

    return true;
  }

  // Vertical corridor (walls left and right)
  if (wall_left && wall_right && !wall_up && !wall_down)
  {
    direction = 0; // Up-down corridor

    // Find start of corridor (topmost passable cell)
    corridorStart = pos;
    while (row > 0)
    {
      int up_pos = (row - 1) * width + col;
      if (board->is_wall_idx(up_pos))
        break;
      row--;
      corridorStart = up_pos;
    }

    // Find end of corridor (bottommost passable cell)
    row = pos / width;
    corridorEnd = pos;
    while (row < height - 1)
    {
      int down_pos = (row + 1) * width + col;
      if (board->is_wall_idx(down_pos))
        break;
      row++;
      corridorEnd = down_pos;
    }

    return true;
  }

  return false;
}

bool SokobanHeuristics::isRegionCapacityDeadlock(const State &s) const
{
  if (!board)
    return false;

  int board_size = board->get_width() * board->get_height();
  Array<bool> visited;

  for (int i = 0; i < board_size; i++)
  {
    visited.push_back(false);
  }

  // Check each unvisited passable cell
  for (int pos = 0; pos < board_size; pos++)
  {
    if (visited[pos] || board->is_wall_idx(pos))
      continue;

    // Flood fill to find connected region
    Array<int> region;
    int boxCount = 0;
    int goalCount = 0;

    floodFillRegionWithBoxes(pos, s, visited, region, boxCount, goalCount);

    // Check if boxes exceed goals in this region
    if (boxCount > goalCount)
    {
      return true; // More boxes than goals in isolated region
    }
  }

  return false;
}

void SokobanHeuristics::floodFillRegion(int pos, Array<bool> &visited, Array<int> &region, int &boxCount, int &goalCount) const
{
  if (!board)
    return;

  int width = board->get_width();
  int height = board->get_height();

  Array<int> stack;
  stack.push_back(pos);

  while (!stack.empty())
  {
    int current = stack[stack.getSize() - 1];
    stack.pop_back();

    if (visited[current] || board->is_wall_idx(current))
      continue;

    visited[current] = true;
    region.push_back(current);

    // Count goals in this cell
    if (isGoalPosition[current])
      goalCount++;

    // Get coordinates
    int row = current / width;
    int col = current % width;

    // Add neighbors to stack
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int dir = 0; dir < 4; dir++)
    {
      int new_row = row + dr[dir];
      int new_col = col + dc[dir];

      if (new_row >= 0 && new_row < height && new_col >= 0 && new_col < width)
      {
        int new_pos = new_row * width + new_col;
        if (!visited[new_pos] && !board->is_wall_idx(new_pos))
        {
          stack.push_back(new_pos);
        }
      }
    }
  }
}

void SokobanHeuristics::floodFillRegionWithBoxes(int pos, const State &state, Array<bool> &visited, Array<int> &region, int &boxCount, int &goalCount) const
{
  if (!board)
    return;

  int width = board->get_width();
  int height = board->get_height();

  Array<int> stack;
  stack.push_back(pos);

  while (!stack.empty())
  {
    int current = stack[stack.getSize() - 1];
    stack.pop_back();

    if (visited[current] || board->is_wall_idx(current))
      continue;

    visited[current] = true;
    region.push_back(current);

    // Count goals in this cell
    if (isGoalPosition[current])
      goalCount++;

    // Count boxes in this cell
    for (int box_idx = 0; box_idx < state.getBoxCount(); box_idx++)
    {
      if (state.getBox(box_idx).pos == current)
        boxCount++;
    }

    // Get coordinates
    int row = current / width;
    int col = current % width;

    // Add neighbors to stack
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int dir = 0; dir < 4; dir++)
    {
      int new_row = row + dr[dir];
      int new_col = col + dc[dir];

      if (new_row >= 0 && new_row < height && new_col >= 0 && new_col < width)
      {
        int new_pos = new_row * width + new_col;
        if (!visited[new_pos] && !board->is_wall_idx(new_pos))
        {
          stack.push_back(new_pos);
        }
      }
    }
  }
}

// ========== STAGE 4: Door-cycle Deadlock Detection ==========

bool SokobanHeuristics::isDoorCycleDeadlocked(const State &s, const Array<Door> &doors) const
{
  if (!board)
    return false;

  return is_door_cycle_deadlock(s, *board, doors);
}

bool SokobanHeuristics::is_door_cycle_deadlock(const State &state, const Board &board, const Array<Door> &doors) const
{
  // Find unreachable goals due to permanently closed doors
  Array<bool> unreachable;
  int board_size = board.get_width() * board.get_height();

  for (int i = 0; i < board_size; i++)
  {
    unreachable.push_back(false);
  }

  findUnreachableGoals(doors, unreachable);

  // Check if any box is assigned to an unreachable goal
  for (int box_idx = 0; box_idx < state.getBoxCount(); box_idx++)
  {
    const BoxInfo &box = state.getBox(box_idx);
    int box_pos = box.pos;

    // For simplicity, we'll check if the box can only reach unreachable goals
    bool has_reachable_goal = false;

    for (int goal_idx = 0; goal_idx < precomputed.getNumGoals(); goal_idx++)
    {
      int goal_pos = precomputed.getGoalPosition(goal_idx);
      if (!unreachable[goal_pos])
      {
        // Check if box can reach this reachable goal
        int distance = precomputed.getDistance(goal_idx, box_pos);
        if (distance != INT_MAX)
        {
          has_reachable_goal = true;
          break;
        }
      }
    }

    if (!has_reachable_goal)
    {
      return true; // Box can only reach unreachable goals
    }
  }

  return false;
}

void SokobanHeuristics::findUnreachableGoals(const Array<Door> &doors, Array<bool> &unreachable) const
{
  if (!board)
    return;

  // Mark goals as unreachable if they are behind permanently closed doors
  for (int door_idx = 0; door_idx < doors.getSize(); door_idx++)
  {
    const Door &door = doors[door_idx];

    // If openTime == 0, door is permanently closed
    if (door.openTime == 0)
    {
      // Find all goals that are only accessible through this door
      // This is a simplified implementation - a full implementation would
      // require complex reachability analysis

      int width = board->get_width();
      int height = board->get_height();

      // Find door positions on the board
      for (int pos = 0; pos < width * height; pos++)
      {
        if (board->get_door_id_idx(pos) == door.id)
        {
          // This is a simplified approach: mark adjacent goals as unreachable
          // A full implementation would do flood-fill to find isolated regions
          int row = pos / width;
          int col = pos % width;

          int dr[] = {-1, 1, 0, 0};
          int dc[] = {0, 0, -1, 1};

          for (int dir = 0; dir < 4; dir++)
          {
            int new_row = row + dr[dir];
            int new_col = col + dc[dir];

            if (new_row >= 0 && new_row < height && new_col >= 0 && new_col < width)
            {
              int adj_pos = new_row * width + new_col;
              if (isGoalPosition[adj_pos])
              {
                unreachable[adj_pos] = true;
              }
            }
          }
        }
      }
    }
  }
}