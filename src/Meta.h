#pragma once
#include <string>

// Meta struct for game mechanics metadata
struct Meta
{
  std::string name; // level name
  int width;
  int height;
  int energyLimit;
  int moveCost;
  int pushCost;

  // Default constructor
  Meta();

  // Parameterized constructor for all fields
  Meta(const std::string& name, int width, int height, int energy, int move, int push);
};
