#include "Zobrist.h"
#include <iostream>
#include <chrono>

// Constructor
Zobrist::Zobrist()
    : player_pos_table(nullptr), box_unlabeled_table(nullptr),
      box_letter_table(nullptr), key_held_table(nullptr),
      step_mod_table(nullptr), board_size(0), max_box_id_count(0),
      max_key_count(0), time_modulo_L(0), initialized(false)
{
}

// Destructor
Zobrist::~Zobrist()
{
  cleanup();
}

// Initialize Zobrist tables
void Zobrist::init(int boardSize, int maxBoxIdCount, int maxKeyCount, int timeModuloL, uint64_t seed)
{
  // Clean up any existing tables
  cleanup();

  // Store dimensions
  board_size = boardSize;
  max_box_id_count = maxBoxIdCount;
  max_key_count = maxKeyCount;
  time_modulo_L = timeModuloL;

  // Validate parameters
  if (boardSize <= 0 || maxBoxIdCount <= 0 || maxKeyCount <= 0 || timeModuloL <= 0)
  {
    std::cerr << "Error: Invalid Zobrist parameters" << std::endl;
    return;
  }

  // Warn if L is very large
  if (timeModuloL > 5000)
  {
    std::cerr << "Warning: timeModuloL (" << timeModuloL << ") is very large (> 5000)" << std::endl;
  }

  // Initialize random number generator
  std::mt19937_64 rng;
  if (seed != 0)
  {
    rng.seed(seed);
  }
  else
  {
    // Use time-based seed
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    rng.seed(static_cast<uint64_t>(nanoseconds));
  }

  try
  {
    // Allocate tables
    player_pos_table = new uint64_t[boardSize];
    box_unlabeled_table = new uint64_t[boardSize];
    box_letter_table = new uint64_t[26 * boardSize]; // 26 letters * boardSize positions
    key_held_table = new uint64_t[maxKeyCount];
    step_mod_table = new uint64_t[timeModuloL];

    // Fill tables with random values
    for (int i = 0; i < boardSize; i++)
    {
      player_pos_table[i] = rng();
      box_unlabeled_table[i] = rng();

      // Fill letter tables for all 26 letters
      for (int letter = 0; letter < 26; letter++)
      {
        box_letter_table[letter * boardSize + i] = rng();
      }
    }

    for (int i = 0; i < maxKeyCount; i++)
    {
      key_held_table[i] = rng();
    }

    for (int i = 0; i < timeModuloL; i++)
    {
      step_mod_table[i] = rng();
    }

    initialized = true;

    std::cout << "Zobrist initialized: boardSize=" << boardSize
              << ", maxBoxIdCount=" << maxBoxIdCount
              << ", maxKeyCount=" << maxKeyCount
              << ", timeModuloL=" << timeModuloL << std::endl;
  }
  catch (const std::bad_alloc &e)
  {
    std::cerr << "Error: Failed to allocate Zobrist tables: " << e.what() << std::endl;
    cleanup();
  }
}

// Cleanup method
void Zobrist::cleanup()
{
  delete[] player_pos_table;
  delete[] box_unlabeled_table;
  delete[] box_letter_table;
  delete[] key_held_table;
  delete[] step_mod_table;

  player_pos_table = nullptr;
  box_unlabeled_table = nullptr;
  box_letter_table = nullptr;
  key_held_table = nullptr;
  step_mod_table = nullptr;

  initialized = false;
}

// Hash value getters
uint64_t Zobrist::player_hash(int pos) const
{
  if (!initialized || pos < 0 || pos >= board_size)
  {
    return 0;
  }
  return player_pos_table[pos];
}

uint64_t Zobrist::box_hash_unlabeled(int pos) const
{
  if (!initialized || pos < 0 || pos >= board_size)
  {
    return 0;
  }
  return box_unlabeled_table[pos];
}

uint64_t Zobrist::box_hash_letter(char id, int pos) const
{
  if (!initialized || pos < 0 || pos >= board_size || id < 'A' || id > 'Z')
  {
    return 0;
  }
  int letterIndex = id - 'A'; // Convert 'A'-'Z' to 0-25
  return box_letter_table[letterIndex * board_size + pos];
}

uint64_t Zobrist::key_hash(int keyIndex) const
{
  if (!initialized || keyIndex < 0 || keyIndex >= max_key_count)
  {
    return 0;
  }
  return key_held_table[keyIndex];
}

uint64_t Zobrist::time_hash(int tmod) const
{
  if (!initialized || tmod < 0 || tmod >= time_modulo_L)
  {
    return 0;
  }
  return step_mod_table[tmod];
}