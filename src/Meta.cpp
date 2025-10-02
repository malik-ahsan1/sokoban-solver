// Meta implementations
#include "Meta.h"

// Default constructor
Meta::Meta() : name(""), width(0), height(0), energyLimit(0), moveCost(0), pushCost(0)
{
}

// Parameterized constructor for all fields
Meta::Meta(const std::string& name, int width, int height, int energy, int move, int push)
    : name(name), width(width), height(height), energyLimit(energy), moveCost(move), pushCost(push)
{
}
