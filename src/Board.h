#pragma once

#include "Array.h"
#include "Cell.h"
#include "Door.h"
#include <cassert>

class Board
{
private:
  int width;
  int height;

  Array<Cell> cells;          // length = width * height (filled via push_back)
  Array<int> targetPositions; // indices of targets
  int time_modulo_L;          // LCM of all door cycles

  // Constants
  static const int MAX_SAFE_L = 4096;

public:
  Board();

  // initialize allocates and fills the cells array with default Cells
  bool initialize(int w, int h);

  // Coordinate helpers
  inline int index(int row, int col) const { return row * width + col; }
  inline int row_of(int idx) const { return idx / width; }
  inline int col_of(int idx) const { return idx % width; }
  inline bool in_bounds(int row, int col) const { return row >= 0 && row < height && col >= 0 && col < width; }

  // Cell accessors (non-const + const)
  Cell &cell_at_index(int idx);
  const Cell &cell_at_index(int idx) const;
  Cell &cell_at_rc(int row, int col);
  const Cell &cell_at_rc(int row, int col) const;

  // Queries
  bool is_wall_idx(int idx) const;
  bool is_target_idx(int idx) const;
  int get_door_id_idx(int idx) const;

  // Mutators
  void set_wall(int row, int col);
  void set_target(int row, int col);
  void set_door(int row, int col, int id);

  // Info
  int num_targets() const { return targetPositions.getSize(); }

  // Debug printing (prints static layout only)
  void pretty_print() const;

  // Door cycle support
  void computeTimeModuloL(const Array<Door> &doors);

  // Getters
  int get_width() const { return width; }
  int get_height() const { return height; }
  int getTimeModuloL() const { return time_modulo_L; }
};
