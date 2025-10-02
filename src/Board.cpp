#include "Board.h"
#include <iostream>
using namespace std;

Board::Board()
{
  width = 0;
  height = 0;
  time_modulo_L = 1; // Default to 1 if no doors
  // cells and targetPositions default-construct
}

bool Board::initialize(int w, int h)
{
  if (w <= 0 || h <= 0)
    return false;
  width = w;
  height = h;

  // Reinitialize arrays (assignment to a new Array uses your assignment operator)
  cells = Array<Cell>();
  targetPositions = Array<int>();

  int total = width * height;
  // Fill the cells array with default-constructed Cell objects
  for (int i = 0; i < total; ++i)
  {
    Cell c; // defaults to FLOOR, isTarget=false, doorId=0
    cells.push_back(c);
  }

  return true;
}

Cell &Board::cell_at_index(int idx)
{
  assert(idx >= 0 && idx < cells.getSize());
  return cells[idx];
}

const Cell &Board::cell_at_index(int idx) const
{
  assert(idx >= 0 && idx < cells.getSize());
  return cells[idx];
}

Cell &Board::cell_at_rc(int row, int col)
{
  assert(in_bounds(row, col));
  return cell_at_index(index(row, col));
}

const Cell &Board::cell_at_rc(int row, int col) const
{
  assert(in_bounds(row, col));
  return cell_at_index(index(row, col));
}

bool Board::is_wall_idx(int idx) const
{
  assert(idx >= 0 && idx < cells.getSize());
  return cells[idx].type == CELL_WALL;
}

bool Board::is_target_idx(int idx) const
{
  assert(idx >= 0 && idx < cells.getSize());
  return cells[idx].isTarget;
}

int Board::get_door_id_idx(int idx) const
{
  assert(idx >= 0 && idx < cells.getSize());
  return cells[idx].doorId;
}

void Board::set_wall(int row, int col)
{
  assert(in_bounds(row, col));
  int idx = index(row, col);
  cells[idx].type = CELL_WALL;
  cells[idx].debugChar = '#';
  // if it was target before, keep isTarget? Usually wall shouldn't be target
  cells[idx].isTarget = false;
  cells[idx].doorId = 0;
}

void Board::set_target(int row, int col)
{
  assert(in_bounds(row, col));
  int idx = index(row, col);
  cells[idx].type = CELL_FLOOR;
  if (!cells[idx].isTarget)
  {
    cells[idx].isTarget = true;
    targetPositions.push_back(idx);
  }
  cells[idx].debugChar = '.';
}

void Board::set_door(int row, int col, int id)
{
  assert(in_bounds(row, col));
  assert(id >= 1 && id <= 9);
  int idx = index(row, col);
  cells[idx].type = CELL_FLOOR;
  cells[idx].doorId = id;
  cells[idx].debugChar = char('0' + id);
  // do not touch isTarget here (digit on top of a target is unusual but allowed if spec permits)
}

void Board::pretty_print() const
{
  if (width == 0 || height == 0)
  {
    cout << "(empty board)\n";
    return;
  }

  for (int r = 0; r < height; ++r)
  {
    for (int c = 0; c < width; ++c)
    {
      int idx = index(r, c);
      const Cell &cell = cells[idx];
      if (cell.type == CELL_WALL)
        cout << '#';
      else if (cell.isTarget)
        cout << '.';
      else if (cell.doorId > 0)
        cout << char('0' + cell.doorId);
      else
        cout << ' ';
    }
    cout << '\n';
  }
}

// Helper functions for LCM calculation
namespace
{
  // Greatest Common Divisor using Euclidean algorithm
  int gcd(int a, int b)
  {
    if (b == 0)
      return a;
    return gcd(b, a % b);
  }

  // Least Common Multiple
  int lcm(int a, int b)
  {
    if (a == 0 || b == 0)
      return 0;
    return (a / gcd(a, b)) * b; // Avoid overflow by dividing first
  }
}

// Compute time modulo L from door cycles
void Board::computeTimeModuloL(const Array<Door> &doors)
{
  if (doors.getSize() == 0)
  {
    time_modulo_L = 1;
    return;
  }

  // Calculate LCM of all door cycles
  time_modulo_L = 1;
  for (int i = 0; i < doors.getSize(); i++)
  {
    const Door &door = doors[i];
    int cycle_length = door.openTime + door.closeTime;

    if (cycle_length <= 0)
    {
      std::cerr << "Warning: Door " << door.id << " has invalid cycle length: "
                << cycle_length << std::endl;
      continue;
    }

    time_modulo_L = lcm(time_modulo_L, cycle_length);

    // Check for overflow or extremely large L values
    if (time_modulo_L <= 0 || time_modulo_L > MAX_SAFE_L)
    {
      if (time_modulo_L > MAX_SAFE_L)
      {
        std::cerr << "Warning: Time modulo L (" << time_modulo_L
                  << ") exceeds MAX_SAFE_L (" << MAX_SAFE_L << ")" << std::endl;
        // Continue with the real L value for now as requested
      }
      else
      {
        std::cerr << "Error: Time modulo L calculation overflow" << std::endl;
        time_modulo_L = MAX_SAFE_L; // Fallback
      }
      break;
    }
  }

  std::cout << "Computed time modulo L = " << time_modulo_L
            << " from " << doors.getSize() << " doors" << std::endl;
}
