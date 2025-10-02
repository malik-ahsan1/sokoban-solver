#pragma once

// Door struct for managing door timing mechanics
struct Door
{
  int id;           // door ID, 1..9
  int openTime;     // cycles door stays open
  int closeTime;    // cycles door stays closed
  int phase;        // phase offset in cycle
  bool initialOpen; // true if door starts open

  // Default constructor
  Door();

  // Parameterized constructor for all fields
  Door(int doorId, int openCycles, int closeCycles, int phaseOffset, bool startsOpen);

  // Check if door is open at given time
  bool isOpenAtTime(int t) const;
};

