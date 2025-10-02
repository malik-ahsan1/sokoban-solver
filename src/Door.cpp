#include "Door.h"

// Door implementations

// Default constructor
Door::Door() : id(0), openTime(0), closeTime(0), phase(0), initialOpen(false)
{
}

// Parameterized constructor for all fields
Door::Door(int doorId, int openCycles, int closeCycles, int phaseOffset, bool startsOpen)
    : id(doorId), openTime(openCycles), closeTime(closeCycles), phase(phaseOffset), initialOpen(startsOpen)
{
}

// Check if door is open at given time
bool Door::isOpenAtTime(int t) const
{
  // Handle edge case: if door never opens or closes
  if (openTime <= 0 && closeTime <= 0)
  {
    return initialOpen;
  }

  // Compute cycle length
  int cycleLength = openTime + closeTime;

  // Handle edge case: zero cycle length
  if (cycleLength <= 0)
  {
    return initialOpen;
  }

  // Adjust with phase: effective time in cycle
  int effective = (t + phase) % cycleLength;

  // Ensure effective is non-negative (handle negative modulo)
  if (effective < 0)
  {
    effective += cycleLength;
  }

  // Determine if door is open based on initial state
  if (initialOpen)
  {
    // Door starts open: open when effective < openTime
    return effective < openTime;
  }
  else
  {
    // Door starts closed: open when effective >= closeTime
    return effective >= closeTime;
  }
}
