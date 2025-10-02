#include <iostream>
#include "../src/Door.h"

int main()
{
  std::cout << "=== Door Test ===" << std::endl;

  // Test Door default constructor
  std::cout << "\n--- Door Default Constructor Test ---" << std::endl;
  Door defaultDoor;
  std::cout << "Default door - id: " << defaultDoor.id
            << ", openTime: " << defaultDoor.openTime
            << ", closeTime: " << defaultDoor.closeTime
            << ", phase: " << defaultDoor.phase
            << ", initialOpen: " << (defaultDoor.initialOpen ? "true" : "false") << std::endl;

  // Test Door parameterized constructor
  std::cout << "\n--- Door Parameterized Constructor Test ---" << std::endl;
  Door door1(1, 3, 2, 0, true); // Door 1: open 3 cycles, closed 2 cycles, starts open
  std::cout << "Door1 - id: " << door1.id
            << ", openTime: " << door1.openTime
            << ", closeTime: " << door1.closeTime
            << ", phase: " << door1.phase
            << ", initialOpen: " << (door1.initialOpen ? "true" : "false") << std::endl;

  Door door2(2, 4, 3, 1, false); // Door 2: open 4 cycles, closed 3 cycles, phase 1, starts closed
  std::cout << "Door2 - id: " << door2.id
            << ", openTime: " << door2.openTime
            << ", closeTime: " << door2.closeTime
            << ", phase: " << door2.phase
            << ", initialOpen: " << (door2.initialOpen ? "true" : "false") << std::endl;

  // Test Door timing logic
  std::cout << "\n--- Door Timing Test (Door1: starts open, 3 open + 2 closed = 5 cycle) ---" << std::endl;
  for (int t = 0; t <= 10; t++)
  {
    bool isOpen = door1.isOpenAtTime(t);
    std::cout << "Time " << t << ": " << (isOpen ? "OPEN" : "CLOSED") << std::endl;
  }

  std::cout << "\n--- Door Timing Test (Door2: starts closed, phase 1, 4 open + 3 closed = 7 cycle) ---" << std::endl;
  for (int t = 0; t <= 14; t++)
  {
    bool isOpen = door2.isOpenAtTime(t);
    std::cout << "Time " << t << ": " << (isOpen ? "OPEN" : "CLOSED") << std::endl;
  }

  // Test edge cases
  std::cout << "\n--- Edge Case Tests ---" << std::endl;

  // Door that never cycles (both times are 0)
  Door staticDoor(3, 0, 0, 0, true);
  std::cout << "Static door (always open): Time 5 = " << (staticDoor.isOpenAtTime(5) ? "OPEN" : "CLOSED") << std::endl;

  Door staticClosedDoor(4, 0, 0, 0, false);
  std::cout << "Static door (always closed): Time 5 = " << (staticClosedDoor.isOpenAtTime(5) ? "OPEN" : "CLOSED") << std::endl;

  // Door with phase offset
  Door phaseDoor(5, 2, 3, -1, true); // Phase -1 should work correctly
  std::cout << "Phase door (phase -1): ";
  for (int t = 0; t < 5; t++)
  {
    std::cout << "T" << t << "=" << (phaseDoor.isOpenAtTime(t) ? "O" : "C") << " ";
  }
  std::cout << std::endl;

  return 0;
}