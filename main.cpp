#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "ConfigParser.h"
#include "SolverAStar.h"
#include "Simulation.h"
#include "SolverStats.h"
#include "Zobrist.h"

class SokobanGame
{
private:
  Board board;
  Array<Door> doors;
  Meta meta;
  InitialDynamicState initialState;
  Zobrist zobrist;

  Array<char> lastSolution;
  SolverStats lastStats;
  bool hasSolution;
  bool hasLoadedBoard;

public:
  SokobanGame() : hasSolution(false), hasLoadedBoard(false) {}

  void run()
  {
    std::cout << "===========================================" << std::endl;
    std::cout << "    Welcome to Sokoban Solver & Player" << std::endl;
    std::cout << "===========================================" << std::endl;

    while (true)
    {
      showMenu();
      int choice = getMenuChoice();

      switch (choice)
      {
      case 1:
        loadBoard();
        break;
      case 2:
        solve();
        break;
      case 3:
        showSolution();
        break;
      case 4:
        playCustomMoves();
        break;
      case 5:
        showStatistics();
        break;
      case 6:
        std::cout << "Thank you for playing Sokoban!" << std::endl;
        return;
      default:
        std::cout << "Invalid choice. Please try again." << std::endl;
      }

      std::cout << std::endl;
    }
  }

private:
  void showMenu()
  {
    std::cout << std::endl;
    std::cout << "========== MAIN MENU ==========" << std::endl;
    std::cout << "1) Load board from file" << std::endl;
    std::cout << "2) Solve current puzzle" << std::endl;
    std::cout << "3) Show solution animation" << std::endl;
    std::cout << "4) Play custom moves" << std::endl;
    std::cout << "5) Show solver statistics" << std::endl;
    std::cout << "6) Exit" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "Enter your choice (1-6): ";
  }

  int getMenuChoice()
  {
    std::string input;
    std::getline(std::cin, input);

    // Trim whitespace
    input.erase(0, input.find_first_not_of(" \t\n\r"));
    input.erase(input.find_last_not_of(" \t\n\r") + 1);

    try
    {
      return std::stoi(input);
    }
    catch (...)
    {
      return -1;
    }
  }

  void loadBoard()
  {
    std::cout << std::endl
              << "=== LOAD BOARD ===" << std::endl;
    std::cout << "Enter filename: ";

    std::string filename;
    std::getline(std::cin, filename);

    // Trim whitespace
    filename.erase(0, filename.find_first_not_of(" \t\n\r"));
    filename.erase(filename.find_last_not_of(" \t\n\r") + 1);

    if (filename.empty())
    {
      std::cout << "No filename provided." << std::endl;
      return;
    }

    ConfigParser parser;

    // Clear previous data
    board = Board();
    doors = Array<Door>();
    meta = Meta();
    initialState = InitialDynamicState();
    hasSolution = false;

    if (parser.parse(filename.c_str(), meta, board, doors, initialState))
    {
      hasLoadedBoard = true;

      // Initialize Zobrist with proper parameters
      zobrist.cleanup();
      int maxBoxTypes = 26; // A-Z
      int maxKeyTypes = 26; // a-z + none
      zobrist.init(board.get_width() * board.get_height(), maxBoxTypes, maxKeyTypes,
                   board.getTimeModuloL());

      std::cout << std::endl
                << "✓ Board loaded successfully!" << std::endl;
      std::cout << "Board info:" << std::endl;
      std::cout << "  Size: " << board.get_width() << "x" << board.get_height() << std::endl;
      std::cout << "  Energy limit: " << meta.energyLimit << std::endl;
      std::cout << "  Move cost: " << meta.moveCost << std::endl;
      std::cout << "  Push cost: " << meta.pushCost << std::endl;
      std::cout << "  Time cycle: " << board.getTimeModuloL() << std::endl;
      std::cout << "  Doors: " << doors.getSize() << std::endl;
      std::cout << "  Unlocked boxes: " << initialState.unlockedBoxes.getSize() << std::endl;
      std::cout << "  Locked boxes: " << initialState.lockedBoxes.getSize() << std::endl;
      std::cout << "  Keys: " << initialState.keys.getSize() << std::endl;

      // Show the initial board state
      std::cout << std::endl
                << "Initial board layout:" << std::endl;
      DynamicState ds(initialState);
      Simulation::pretty_print(board, ds);
    }
    else
    {
      std::cout << "✗ Failed to load board from '" << filename << "'" << std::endl;
    }
  }

  void solve()
  {
    if (!hasLoadedBoard)
    {
      std::cout << "No board loaded. Please load a board first." << std::endl;
      return;
    }

    std::cout << std::endl
              << "=== SOLVING PUZZLE ===" << std::endl;
    std::cout << "Running A* search algorithm..." << std::endl;

    // Clear previous solution
    while (lastSolution.getSize() > 0)
    {
      lastSolution.pop_back();
    }
    lastStats.reset();

    // Create solver
    SolverAStar solver(board, doors, meta, zobrist);

    // Measure solving time
    auto startTime = std::chrono::steady_clock::now();

    // Solve the puzzle
    bool solved = solver.solve(initialState, lastSolution, lastStats);

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    // Update stats with actual timing
    lastStats.elapsedMs = elapsedMs;
    lastStats.solutionLength = lastSolution.getSize();

    if (solved)
    {
      std::cout << "✓ Solution found!" << std::endl;
      std::cout << "  Solution length: " << lastSolution.getSize() << " moves" << std::endl;
      std::cout << "  Elapsed time: " << elapsedMs << " ms" << std::endl;
      std::cout << "  Nodes expanded: " << lastStats.nodesExpanded << std::endl;
      std::cout << "  Nodes generated: " << lastStats.nodesGenerated << std::endl;
      hasSolution = true;

      // Show move sequence
      if (lastSolution.getSize() > 0)
      {
        std::cout << "  Move sequence: ";
        for (int i = 0; i < lastSolution.getSize() && i < 50; i++)
        {
          std::cout << lastSolution[i];
        }
        if (lastSolution.getSize() > 50)
        {
          std::cout << "... (+" << (lastSolution.getSize() - 50) << " more)";
        }
        std::cout << std::endl;
      }
    }
    else
    {
      std::cout << "✗ No solution found." << std::endl;
      std::cout << "  Elapsed time: " << elapsedMs << " ms" << std::endl;
      std::cout << "  Nodes expanded: " << lastStats.nodesExpanded << std::endl;
      std::cout << "  Nodes generated: " << lastStats.nodesGenerated << std::endl;
      hasSolution = false;
    }
  }

  void showSolution()
  {
    if (!hasLoadedBoard)
    {
      std::cout << "No board loaded. Please load a board first." << std::endl;
      return;
    }

    if (!hasSolution || lastSolution.getSize() == 0)
    {
      std::cout << "No solution available. Please solve the puzzle first." << std::endl;
      return;
    }

    std::cout << std::endl
              << "=== SOLUTION ANIMATION ===" << std::endl;
    std::cout << "Playing solution with " << lastSolution.getSize() << " moves..." << std::endl;
    std::cout << "Press Enter to start animation...";
    std::cin.get();

    // Create dynamic state for animation
    DynamicState ds(initialState);

    // Simulate with animation
    std::cout << "\\nStep 0 (Initial state):" << std::endl;
    Simulation::pretty_print(board, ds);
    std::cout << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (int i = 0; i < lastSolution.getSize(); i++)
    {
      int energyUsed, stepInc;
      if (!Simulation::applyPlayerMove(board, ds, lastSolution[i], doors, meta, energyUsed, stepInc))
      {
        std::cout << "Error: Invalid move '" << lastSolution[i] << "' at step " << (i + 1) << std::endl;
        return;
      }

      std::cout << "Step " << (i + 1) << " - Move: " << lastSolution[i]
                << " (Energy: +" << energyUsed << ", Total: " << ds.energy_used << ")" << std::endl;
      Simulation::pretty_print(board, ds);
      std::cout << std::endl;

      // Delay for animation effect
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    if (ds.isWin(board))
    {
      std::cout << "🎉 PUZZLE SOLVED! All boxes are on targets!" << std::endl;
    }
    else
    {
      std::cout << "⚠️  Warning: Not all boxes are on targets." << std::endl;
    }
  }

  void playCustomMoves()
  {
    if (!hasLoadedBoard)
    {
      std::cout << "No board loaded. Please load a board first." << std::endl;
      return;
    }

    std::cout << std::endl
              << "=== PLAY CUSTOM MOVES ===" << std::endl;
    std::cout << "Enter a sequence of moves (U/D/L/R): ";

    std::string moveString;
    std::getline(std::cin, moveString);

    // Trim whitespace
    moveString.erase(0, moveString.find_first_not_of(" \t\n\r"));
    moveString.erase(moveString.find_last_not_of(" \t\n\r") + 1);

    if (moveString.empty())
    {
      std::cout << "No moves provided." << std::endl;
      return;
    }

    // Convert string to move array
    Array<char> moves;
    for (char c : moveString)
    {
      if (c == 'U' || c == 'D' || c == 'L' || c == 'R' ||
          c == 'u' || c == 'd' || c == 'l' || c == 'r')
      {
        moves.push_back(std::toupper(c));
      }
    }

    if (moves.getSize() == 0)
    {
      std::cout << "No valid moves found. Use U/D/L/R characters." << std::endl;
      return;
    }

    std::cout << "Applying " << moves.getSize() << " moves..." << std::endl;
    std::cout << std::endl;

    // Create dynamic state for simulation
    DynamicState ds(initialState);

    // Apply moves one by one
    std::cout << "Initial state:" << std::endl;
    Simulation::pretty_print(board, ds);
    std::cout << std::endl;

    for (int i = 0; i < moves.getSize(); i++)
    {
      int energyUsed, stepInc;
      if (!Simulation::applyPlayerMove(board, ds, moves[i], doors, meta, energyUsed, stepInc))
      {
        std::cout << "✗ Invalid move '" << moves[i] << "' at step " << (i + 1) << std::endl;
        std::cout << "Simulation stopped." << std::endl;
        return;
      }

      std::cout << "After move '" << moves[i] << "' (step " << (i + 1) << "):" << std::endl;
      Simulation::pretty_print(board, ds);
      std::cout << "Energy used: +" << energyUsed << ", Total: " << ds.energy_used
                << "/" << meta.energyLimit << std::endl;
      std::cout << std::endl;

      // Small delay for readability
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "✓ All moves applied successfully!" << std::endl;
    if (ds.isWin(board))
    {
      std::cout << "🎉 PUZZLE SOLVED! All boxes are on targets!" << std::endl;
    }
    else
    {
      std::cout << "Puzzle not yet solved. Keep trying!" << std::endl;
    }
  }

  void showStatistics()
  {
    if (!hasLoadedBoard)
    {
      std::cout << "No board loaded. Please load a board first." << std::endl;
      return;
    }

    std::cout << std::endl
              << "=== SOLVER STATISTICS ===" << std::endl;

    if (!hasSolution)
    {
      std::cout << "No solve attempt made yet. Run 'Solve' first to see statistics." << std::endl;
      return;
    }

    std::cout << "Last solve attempt:" << std::endl;
    std::cout << "  Nodes expanded: " << lastStats.nodesExpanded << std::endl;
    std::cout << "  Nodes generated: " << lastStats.nodesGenerated << std::endl;
    std::cout << "  Peak open set size: " << lastStats.peakOpenSize << std::endl;
    std::cout << "  Peak closed set size: " << lastStats.peakClosedSize << std::endl;
    std::cout << "  Elapsed time: " << lastStats.elapsedMs << " ms" << std::endl;
    std::cout << "  Solution found: " << (hasSolution ? "YES" : "NO") << std::endl;

    if (hasSolution)
    {
      std::cout << "  Solution length: " << lastStats.solutionLength << " moves" << std::endl;
      std::cout << "  Solution cost: " << lastStats.solutionCost << std::endl;
    }

    // Performance metrics
    if (lastStats.elapsedMs > 0)
    {
      double nodesPerSecond = (lastStats.nodesExpanded * 1000.0) / lastStats.elapsedMs;
      std::cout << "  Search speed: " << std::fixed << std::setprecision(1)
                << nodesPerSecond << " nodes/second" << std::endl;
    }
  }
};

int main()
{
  try
  {
    SokobanGame game;
    game.run();
    return 0;
  }
  catch (const std::exception &e)
  {
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cout << "Unknown error occurred." << std::endl;
    return 1;
  }
}