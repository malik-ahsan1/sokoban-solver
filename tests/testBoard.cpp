#include "Board.h"
#include <iostream>
using namespace std;

int main()
{
  Board b;
  bool ok = b.initialize(5, 4);
  if (!ok)
  {
    cout << "Board init failed\n";
    return 1;
  }

  // set a few walls and a target and a door
  b.set_wall(0, 0);
  b.set_wall(1, 1);
  b.set_target(2, 2);
  b.set_door(3, 3, 3);

  cout << "Board layout:\n";
  b.pretty_print();

  cout << "Index(2,2) = " << b.index(2, 2) << " (expected 12)\n";
  int idx13 = 13;
  cout << "Row of 13 = " << b.row_of(idx13) << " (expected 2)\n";
  cout << "Col of 13 = " << b.col_of(idx13) << " (expected 3)\n";
  cout << "Num targets = " << b.num_targets() << " (expected 1)\n";

  return 0;
}
