// Cell.h
#pragma once

enum CellType
{
  CELL_WALL,
  CELL_FLOOR
};

struct Cell
{
  CellType type;  // WALL or FLOOR
  bool isTarget;  // true if target ('.')
  int doorId;     // 0 = none, 1..9 = door id
  char debugChar; // optional, for printing

  Cell()
  {
    type = CELL_FLOOR;
    isTarget = false;
    doorId = 0;
    debugChar = ' ';
  }
};

