#include "SolverAStar.h"
#include "Simulation.h"
#include "PlayerPathfinder.h"
#include <iostream>

SolverAStar::SolverAStar(const Board &board, const Array<Door> &doors, const Meta &meta, Zobrist &zobrist)
    : board(board), doors(doors), meta(meta), zobrist(zobrist), closedSet(), openSet()
{

  // Initialize heuristics with the board
  heuristics.initialize(board);
}

SolverAStar::~SolverAStar()
{
  // Clean up allocated nodes
  for (int i = 0; i < allNodes.getSize(); i++)
  {
    delete allNodes[i];
  }
}

bool SolverAStar::solve(const InitialDynamicState &init, Array<char> &outMoves, SolverStats &stats)
{
  // Clear previous results
  while (outMoves.getSize() > 0)
  {
    outMoves.pop_back();
  }
  currentStats = SolverStats();
  startTime = std::chrono::steady_clock::now();

  // Clear data structures
  closedSet.clear();
  // Clear previous node storage
  for (int i = 0; i < allNodes.getSize(); i++)
  {
    delete allNodes[i];
  }
  while (allNodes.getSize() > 0)
  {
    allNodes.pop_back();
  }
  // Note: BinaryHeap doesn't have clear(), we'll create a new one if needed

  // Compute L = board.getTimeModuloL() for state creation
  int L = board.getTimeModuloL();

  // Create initial state from InitialDynamicState
  State initialState(init.player_pos, -1, 0, 0); // key_held=-1, energy_used=0, step_mod_L=0

  // Add unlocked boxes to initial state
  for (int i = 0; i < init.unlockedBoxes.getSize(); i++)
  {
    int boxPos = init.unlockedBoxes[i];
    initialState.addBox(boxPos, 0); // unlabeled box
  }

  // Add locked boxes to initial state
  for (int i = 0; i < init.lockedBoxes.getSize(); i++)
  {
    int boxPos = init.lockedBoxes[i];
    char boxId = (i < init.boxIds.getSize()) ? init.boxIds[i] : 'A';
    initialState.addBox(boxPos, boxId);
  }

  // Canonicalize and compute hash
  initialState.canonicalize();
  initialState.recompute_hash(zobrist);

  // Initial state created

  // Compute initial heuristic
  int h_initial = heuristics.heuristic(initialState);

  // Create and store initial node
  AStarNode *initialNodePtr = new AStarNode(initialState, 0, h_initial, nullptr);
  allNodes.push_back(initialNodePtr);
  openSet.push(*initialNodePtr);
  currentStats.nodesGenerated++;

  // Main A* loop
  while (!openSet.isEmpty())
  {
    // Update peak open size
    if (openSet.getSize() > currentStats.peakOpenSize)
    {
      currentStats.peakOpenSize = openSet.getSize();
    }

    // Get node with lowest f-cost
    AStarNode current = openSet.pop();

    // Check if already in closed set with better cost
    uint64_t stateHash = current.state.hash();
    auto it = closedSet.find(stateHash);
    if (it != closedSet.end() && it->second <= current.g)
    {
      continue; // Skip this node - we've seen this state with better cost
    }

    // Check if goal state
    if (isGoal(current.state))
    {
      std::cout << "Goal found!" << std::endl;

      // Find the actual node pointer for this state (need to search allNodes)
      AStarNode *goalNodePtr = nullptr;
      for (int i = 0; i < allNodes.getSize(); i++)
      {
        if (allNodes[i]->state.equals(current.state))
        {
          goalNodePtr = allNodes[i];
          break;
        }
      }

      if (goalNodePtr)
      {
        // Use Simulation's reconstruct_moves function
        Simulation::reconstruct_moves(goalNodePtr, outMoves);
      }
      else
      {
        // Could not find goal node pointer
      }

      currentStats.solutionCost = current.g;
      updateStats();
      stats = currentStats;
      return true;
    }

    // Add to closed set
    closedSet[stateHash] = current.g;
    currentStats.nodesExpanded++;

    // Find current node pointer in allNodes
    AStarNode *currentNodePtr = nullptr;
    for (int i = 0; i < allNodes.getSize(); i++)
    {
      if (allNodes[i]->state.equals(current.state))
      {
        currentNodePtr = allNodes[i];
        break;
      }
    }

    if (!currentNodePtr)
    {
      // Could not find current node pointer
      continue;
    }

    // Generate successors with move tracking
    Array<AStarNode *> successorNodes;
    generateSuccessorsWithMoves(currentNodePtr, successorNodes);

    // Only show progress every 25000 nodes
    if (currentStats.nodesExpanded % 25000 == 0)
    {
      std::cout << "Expanded: " << currentStats.nodesExpanded << ", Generated: " << currentStats.nodesGenerated << std::endl;
    }

    // Stop if we've expanded too many nodes
    if (currentStats.nodesExpanded > 500000)
    {
      std::cout << "Search limit reached" << std::endl;
      break;
    }

    // Process each successor
    for (int i = 0; i < successorNodes.getSize(); i++)
    {
      AStarNode *successorNode = successorNodes[i];
      // Processing successor (debug output reduced for performance)

      // Deadlock detection disabled for performance
      // bool corner_deadlock = heuristics.isCornerDeadlocked(successorNode->state);
      // bool wall_deadlock = heuristics.isWallDeadlocked(successorNode->state);
      // bool multibox_deadlock = heuristics.isMultiboxDeadlocked(successorNode->state);

      // TEMPORARILY DISABLE deadlock detection to test
      // if (heuristics.isDeadlocked(successorNode->state))
      // {
      //   std::cout << "DEBUG: Successor " << (i+1) << " overall deadlocked - skipped" << std::endl;
      //   delete successorNode;
      //   continue;
      // }

      // Check energy limit
      if (successorNode->state.getEnergyUsed() > meta.energyLimit)
      {
        // Energy limit exceeded
        delete successorNode;
        continue;
      }

      // Recompute hash for successor
      successorNode->state.recompute_hash(zobrist);

      // Compute costs
      int g_new = successorNode->state.getEnergyUsed(); // Use total energy as g-cost
      int h_new = heuristics.heuristic(successorNode->state);
      successorNode->g = g_new;
      successorNode->h = h_new;
      successorNode->f = g_new + h_new;

      // Check if already in closed set with better cost
      uint64_t succHash = successorNode->state.hash();
      auto closedIt = closedSet.find(succHash);
      if (closedIt != closedSet.end() && closedIt->second <= g_new)
      {
        // Already in closed set with better cost
        delete successorNode;
        continue; // Skip - already processed with better cost
      }

      // Store node and add to open set
      allNodes.push_back(successorNode);
      openSet.push(*successorNode);
      currentStats.nodesGenerated++;
      // Added to open set
    }
  }

  // No solution found
  updateStats();
  stats = currentStats;
  return false;
}

bool SolverAStar::isGoal(const State &state) const
{
  // Check if all boxes are on target positions
  for (int i = 0; i < state.getBoxCount(); i++)
  {
    const BoxInfo &box = state.getBox(i);
    if (!board.is_target_idx(box.pos))
    {
      return false;
    }
  }
  return true;
}

void SolverAStar::reconstructPath(const AStarNode &goalNode, Array<char> &outMoves) const
{
  // This is a simplified reconstruction - in a full implementation,
  // we would need to track the actual moves that led to each state.
  // For now, we'll use the successor generator to find the moves between states.

  Array<const State *> statePath;
  const State *current = &goalNode.state;

  // Build path from goal to start
  while (current != nullptr)
  {
    statePath.push_back(current);

    // Find parent state in the path (simplified approach)
    // In a full implementation, we'd store parent pointers properly
    current = nullptr; // Break for now - would need proper parent tracking
    break;
  }

  // For now, return empty moves array
  // A full implementation would reconstruct the actual move sequence
  while (outMoves.getSize() > 0)
  {
    outMoves.pop_back();
  }
}

int SolverAStar::computeMoveCost(const State &from, const State &to) const
{
  // Simple cost model - use difference in energy
  return to.getEnergyUsed() - from.getEnergyUsed();
}

void SolverAStar::updateStats()
{
  auto endTime = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
  currentStats.elapsedMs = duration.count();
}

void SolverAStar::generateSuccessorsWithMoves(const AStarNode *parentNode, Array<AStarNode *> &successorNodes)
{
  const State &currentState = parentNode->state;
  int L = board.getTimeModuloL();
  int rows = board.get_height();
  int cols = board.get_width();

  // Reduced debug output for performance  // Generate only atomic single moves (U, D, L, R)
  int player_pos = currentState.getPlayerPos();
  int player_row = player_pos / cols;
  int player_col = player_pos % cols;

  int dr[] = {-1, 1, 0, 0};
  int dc[] = {0, 0, -1, 1};
  char moves[] = {'U', 'D', 'L', 'R'};

  // Try each direction from current player position
  for (int dir = 0; dir < 4; dir++)
  {
    int new_row = player_row + dr[dir];
    int new_col = player_col + dc[dir];

    // Check bounds
    if (new_row < 0 || new_row >= rows || new_col < 0 || new_col >= cols)
    {
      continue;
    }

    int new_pos = new_row * cols + new_col;

    // Check if position is passable (no wall)
    if (board.is_wall_idx(new_pos))
    {
      continue;
    }

    // Check if there's a box at this position
    int box_at_pos = -1;
    for (int b = 0; b < currentState.getBoxCount(); b++)
    {
      if (currentState.getBox(b).pos == new_pos)
      {
        box_at_pos = b;
        break;
      }
    }

    if (box_at_pos == -1)
    {
      // No box - simple player move
      State newState = currentState;
      newState.setPlayerPos(new_pos);

      // Update energy and time
      newState.setEnergyUsed(currentState.getEnergyUsed() + meta.moveCost);
      newState.setStepModL((currentState.getStepModL() + 1) % L);

      // Create successor node
      AStarNode *successorNode = new AStarNode();
      successorNode->state = newState;
      successorNode->parent = reinterpret_cast<State *>(const_cast<AStarNode *>(parentNode));
      successorNode->actionsFromParent.push_back(moves[dir]);

      // Added simple move
      successorNodes.push_back(successorNode);
    }
    else
    {
      // There's a box - try to push it
      const BoxInfo &box = currentState.getBox(box_at_pos);

      // Calculate where the box would go
      int box_new_row = new_row + dr[dir];
      int box_new_col = new_col + dc[dir];

      // Check bounds for box destination
      if (box_new_row < 0 || box_new_row >= rows || box_new_col < 0 || box_new_col >= cols)
      {
        continue;
      }

      int box_new_pos = box_new_row * cols + box_new_col;

      // Check if box destination is passable (no wall, no other box)
      if (board.is_wall_idx(box_new_pos))
      {
        continue;
      }

      // Check if another box is already at destination
      bool blocked_by_box = false;
      for (int b = 0; b < currentState.getBoxCount(); b++)
      {
        if (b != box_at_pos && currentState.getBox(b).pos == box_new_pos)
        {
          blocked_by_box = true;
          break;
        }
      }
      if (blocked_by_box)
      {
        continue;
      }

      // Valid box push - create new state
      State newState = currentState;
      newState.setPlayerPos(new_pos); // Player moves to where box was

      // Move the box
      newState.removeBox(box_at_pos);
      newState.addBox(box_new_pos, box.id);

      // Update energy and time (move cost + push cost)
      newState.setEnergyUsed(currentState.getEnergyUsed() + meta.moveCost + meta.pushCost);
      newState.setStepModL((currentState.getStepModL() + 1) % L);

      // Create successor node
      AStarNode *successorNode = new AStarNode();
      successorNode->state = newState;
      successorNode->parent = reinterpret_cast<State *>(const_cast<AStarNode *>(parentNode));
      successorNode->actionsFromParent.push_back(moves[dir]);

      // Added box push
      successorNodes.push_back(successorNode);
    }
  }
}