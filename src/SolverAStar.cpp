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
      std::cout << "Goal state: player at " << current.state.getPlayerPos() 
                << ", boxes: ";
      for (int i = 0; i < current.state.getBoxCount(); i++)
      {
        std::cout << current.state.getBox(i).pos << " ";
      }
      std::cout << std::endl;

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

    // Only show progress every 50000 nodes
    if (currentStats.nodesExpanded % 50000 == 0)
    {
      std::cout << "Expanded: " << currentStats.nodesExpanded << ", Generated: " << currentStats.nodesGenerated << std::endl;
    }

    // Stop if we've expanded too many nodes (increased limit for complex puzzles)
    if (currentStats.nodesExpanded > 1000000)
    {
      std::cout << "Search limit reached (1,000,000 nodes)" << std::endl;
      break;
    }

    // Process each successor
    for (int i = 0; i < successorNodes.getSize(); i++)
    {
      AStarNode *successorNode = successorNodes[i];
      // Processing successor (debug output reduced for performance)

      // Enable deadlock detection to prune dead-end states
      if (heuristics.isDeadlocked(successorNode->state))
      {
        // Deadlocked state - skip it
        delete successorNode;
        continue;
      }

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

  // Use the proper successor generator that only generates box pushes
  Array<State> successors = generateSuccessors(currentState, board, doors, meta);

  // Create pathfinder to reconstruct the full move sequences
  // The pathfinder needs to avoid box positions
  Array<int> boxPositions;
  for (int i = 0; i < currentState.getBoxCount(); i++)
  {
    boxPositions.push_back(currentState.getBox(i).pos);
  }

  PlayerPathfinder pathfinder;
  pathfinder.initialize(board, doors);
  pathfinder.setBlockedPositions(boxPositions);
  pathfinder.findPaths(currentState.getPlayerPos(), currentState.getStepModL());

  // Direction vectors for movement: up, down, left, right
  static const int dr[] = {-1, 1, 0, 0};
  static const int dc[] = {0, 0, -1, 1};
  static const char moves[] = {'U', 'D', 'L', 'R'};

  int cols = board.get_width();

  // Convert State objects to AStarNode pointers
  for (int i = 0; i < successors.getSize(); i++)
  {
    AStarNode *successorNode = new AStarNode();
    successorNode->state = successors[i];
    successorNode->parent = reinterpret_cast<State *>(const_cast<AStarNode *>(parentNode));
    
    // Get the full move sequence: player path + push direction
    // The successor state has the player at the box's original position after pushing
    // We need to find where the player was BEFORE the push to get the correct path
    
    // Get the push direction
    char pushDirection = successors[i].getActionFromParent();
    
    // Find which direction index this corresponds to
    int dirIndex = -1;
    for (int d = 0; d < 4; d++)
    {
      if (moves[d] == pushDirection)
      {
        dirIndex = d;
        break;
      }
    }
    
    if (dirIndex == -1)
    {
      // No push direction stored, skip this successor
      delete successorNode;
      continue;
    }
    
    // Calculate where the player needed to be to make this push
    // The new player position is where the box was
    int boxOldPos = successors[i].getPlayerPos();
    int boxOldRow = boxOldPos / cols;
    int boxOldCol = boxOldPos % cols;
    
    // Player was on the opposite side of the push direction
    int playerTargetRow = boxOldRow - dr[dirIndex];
    int playerTargetCol = boxOldCol - dc[dirIndex];
    int playerTarget = playerTargetRow * cols + playerTargetCol;
    
    // Get the path from current player position to the push position
    Array<char> pathMoves = pathfinder.getPath(playerTarget);
    
    // DEBUG
    if (false && successors[i].getBox(0).pos == 18) {  // If this leads to goal
      std::cout << "DEBUG: Generating successor that leads to goal (box at 18)" << std::endl;
      std::cout << "  Parent player pos: " << currentState.getPlayerPos() << std::endl;
      std::cout << "  Push direction: " << pushDirection << std::endl;
      std::cout << "  Box old pos: " << boxOldPos << std::endl;
      std::cout << "  Player target pos: " << playerTarget << std::endl;
      std::cout << "  Path: ";
      for (int j = 0; j < pathMoves.getSize(); j++) {
        std::cout << pathMoves[j];
      }
      std::cout << std::endl;
    }
    
    // Add all path moves to actionsFromParent
    for (int j = 0; j < pathMoves.getSize(); j++)
    {
      successorNode->actionsFromParent.push_back(pathMoves[j]);
    }
    
    // Add the push direction
    successorNode->actionsFromParent.push_back(pushDirection);
    
    successorNodes.push_back(successorNode);
  }
}