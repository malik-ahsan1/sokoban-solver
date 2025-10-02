#pragma once
#include <cstdint>
#include <random>

// Zobrist hashing class for generating hash values for game states
class Zobrist
{
private:
  // Hash tables for different components
  uint64_t *player_pos_table;    // Hash values for player positions
  uint64_t *box_unlabeled_table; // Hash values for unlabeled box positions
  uint64_t *box_letter_table;    // Hash values for labeled box positions [26][boardSize]
  uint64_t *key_held_table;      // Hash values for held keys
  uint64_t *key_on_board_table;  // Hash values for keys on board [26][boardSize]
  uint64_t *step_mod_table;      // Hash values for step modulo L

  // Dimensions
  int board_size;       // Total board positions (width * height)
  int max_box_id_count; // Number of distinct box ID types
  int max_key_count;    // Number of key types + 1 for none
  int time_modulo_L;    // Step modulo cycle size

  bool initialized; // Flag to check if properly initialized

public:
  // Constructor
  Zobrist();

  // Destructor
  ~Zobrist();

  // Copy constructor and assignment operator (deleted to prevent issues with pointers)
  Zobrist(const Zobrist &) = delete;
  Zobrist &operator=(const Zobrist &) = delete;

  // Initialization method
  void init(int boardSize, int maxBoxIdCount, int maxKeyCount, int timeModuloL, uint64_t seed = 0);

  // Cleanup method
  void cleanup();

  // Hash value getters
  uint64_t player_hash(int pos) const;
  uint64_t box_hash_unlabeled(int pos) const;
  uint64_t box_hash_letter(char id, int pos) const;
  uint64_t key_hash(int keyIndex) const;
  uint64_t key_on_board_hash(char id, int pos) const;  // Hash for keys on board
  uint64_t time_hash(int tmod) const;

  // Utility methods
  bool isInitialized() const { return initialized; }
  int getBoardSize() const { return board_size; }
  int getTimeModuloL() const { return time_modulo_L; }
};