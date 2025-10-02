#include <iostream>
#include "../src/Meta.h"

int main()
{
  std::cout << "=== Meta Test ===" << std::endl;

  // Test Meta default constructor
  std::cout << "\n--- Meta Default Constructor Test ---" << std::endl;
  Meta defaultMeta;
  std::cout << "Default meta - name: '" << defaultMeta.name
            << "', width: " << defaultMeta.width
            << ", height: " << defaultMeta.height
            << ", energyLimit: " << defaultMeta.energyLimit
            << ", moveCost: " << defaultMeta.moveCost
            << ", pushCost: " << defaultMeta.pushCost << std::endl;

  // Test Meta parameterized constructor
  std::cout << "\n--- Meta Parameterized Constructor Test ---" << std::endl;
  Meta gameMeta("Level1", 10, 8, 100, 1, 2);
  std::cout << "Game meta - name: '" << gameMeta.name
            << "', width: " << gameMeta.width
            << ", height: " << gameMeta.height
            << ", energyLimit: " << gameMeta.energyLimit
            << ", moveCost: " << gameMeta.moveCost
            << ", pushCost: " << gameMeta.pushCost << std::endl;

  Meta hardMeta("HardLevel", 15, 12, 50, 2, 5);
  std::cout << "Hard meta - name: '" << hardMeta.name
            << "', width: " << hardMeta.width
            << ", height: " << hardMeta.height
            << ", energyLimit: " << hardMeta.energyLimit
            << ", moveCost: " << hardMeta.moveCost
            << ", pushCost: " << hardMeta.pushCost << std::endl;

  // Test additional Meta scenarios
  std::cout << "\n--- Meta Usage Scenarios ---" << std::endl;

  // Easy mode
  Meta easyMeta("EasyLevel", 8, 6, 200, 1, 1);
  std::cout << "Easy meta - name: '" << easyMeta.name
            << "', size: " << easyMeta.width << "x" << easyMeta.height
            << ", energyLimit: " << easyMeta.energyLimit
            << ", moveCost: " << easyMeta.moveCost
            << ", pushCost: " << easyMeta.pushCost << std::endl;

  // Calculate how many moves are possible
  int possibleMoves = easyMeta.energyLimit / easyMeta.moveCost;
  int possiblePushes = easyMeta.energyLimit / easyMeta.pushCost;
  std::cout << "With easy settings: " << possibleMoves << " moves or "
            << possiblePushes << " pushes possible" << std::endl;

  // Hard mode calculations
  possibleMoves = hardMeta.energyLimit / hardMeta.moveCost;
  possiblePushes = hardMeta.energyLimit / hardMeta.pushCost;
  std::cout << "With hard settings: " << possibleMoves << " moves or "
            << possiblePushes << " pushes possible" << std::endl;

  // Mixed actions calculation
  int energy = gameMeta.energyLimit;
  int moves = 10;
  int pushes = 5;
  int totalCost = (moves * gameMeta.moveCost) + (pushes * gameMeta.pushCost);
  std::cout << "\n--- Energy Calculation Test ---" << std::endl;
  std::cout << "Level '" << gameMeta.name << "' (" << gameMeta.width << "x" << gameMeta.height << "): "
            << moves << " moves + " << pushes << " pushes = " << totalCost
            << " energy (limit: " << energy << ")" << std::endl;
  std::cout << "Action sequence is " << (totalCost <= energy ? "VALID" : "INVALID") << std::endl;

  // Level size calculations
  std::cout << "\n--- Level Size Analysis ---" << std::endl;
  int totalCells = gameMeta.width * gameMeta.height;
  std::cout << "Level '" << gameMeta.name << "' has " << totalCells << " total cells" << std::endl;

  totalCells = hardMeta.width * hardMeta.height;
  std::cout << "Level '" << hardMeta.name << "' has " << totalCells << " total cells" << std::endl;

  return 0;
}