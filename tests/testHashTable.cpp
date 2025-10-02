#include <iostream>
#include "../src/HashTable.h"

int main()
{
  std::cout << "=== HashTable Test ===" << std::endl;

  // Create HashTable with 10 buckets
  HashTable<int> ht(10);

  std::cout << "Initial table size: " << ht.getSize() << std::endl;
  std::cout << "Initial table capacity: " << ht.getCapacity() << std::endl;
  std::cout << "Is empty: " << (ht.isEmpty() ? "Yes" : "No") << std::endl;

  // Insert test values: 5, 15, 25
  std::cout << "\nInserting values: 5, 15, 25" << std::endl;
  ht.insert(5);
  std::cout << "Inserted 5, size: " << ht.getSize() << std::endl;

  ht.insert(15);
  std::cout << "Inserted 15, size: " << ht.getSize() << std::endl;

  ht.insert(25);
  std::cout << "Inserted 25, size: " << ht.getSize() << std::endl;

  // Print table state
  std::cout << "\nTable state after insertions:" << std::endl;
  ht.printTable();

  // Test find operations
  std::cout << "\n=== Find Tests ===" << std::endl;
  std::cout << "find(15): " << (ht.find(15) ? "true" : "false") << " (should be true)" << std::endl;
  std::cout << "find(99): " << (ht.find(99) ? "true" : "false") << " (should be false)" << std::endl;
  std::cout << "find(5): " << (ht.find(5) ? "true" : "false") << " (should be true)" << std::endl;
  std::cout << "find(25): " << (ht.find(25) ? "true" : "false") << " (should be true)" << std::endl;

  // Test removal
  std::cout << "\n=== Remove Tests ===" << std::endl;
  std::cout << "Size before removal: " << ht.getSize() << std::endl;

  bool removed = ht.remove(15);
  std::cout << "remove(15): " << (removed ? "true" : "false") << " (should be true)" << std::endl;
  std::cout << "Size after removing 15: " << ht.getSize() << std::endl;

  // Check that 15 is no longer found
  std::cout << "find(15) after removal: " << (ht.find(15) ? "true" : "false") << " (should be false)" << std::endl;

  // Try to remove non-existent element
  bool removedNonExistent = ht.remove(99);
  std::cout << "remove(99): " << (removedNonExistent ? "true" : "false") << " (should be false)" << std::endl;
  std::cout << "Size after trying to remove 99: " << ht.getSize() << std::endl;

  // Print final table state
  std::cout << "\nFinal table state:" << std::endl;
  ht.printTable();

  // Test duplicate insertion
  std::cout << "\n=== Duplicate Insertion Test ===" << std::endl;
  std::cout << "Size before inserting duplicate: " << ht.getSize() << std::endl;
  ht.insert(5); // Try to insert 5 again
  std::cout << "Size after trying to insert 5 again: " << ht.getSize() << " (should be same)" << std::endl;

  // Test with more values to show collision handling
  std::cout << "\n=== Collision Test ===" << std::endl;
  HashTable<int> ht2(5); // Smaller table to force collisions

  // Insert values that will hash to same buckets
  int values[] = {1, 6, 11, 16, 21}; // All hash to bucket 1 with capacity 5

  for (int i = 0; i < 5; i++)
  {
    ht2.insert(values[i]);
    std::cout << "Inserted " << values[i] << ", size: " << ht2.getSize() << std::endl;
  }

  std::cout << "\nTable with collisions:" << std::endl;
  ht2.printTable();

  // Test finding elements in collision chain
  std::cout << "\nFinding elements in collision chain:" << std::endl;
  for (int i = 0; i < 5; i++)
  {
    std::cout << "find(" << values[i] << "): " << (ht2.find(values[i]) ? "true" : "false") << std::endl;
  }

  // Remove from middle of collision chain
  std::cout << "\nRemoving " << values[2] << " from collision chain" << std::endl;
  ht2.remove(values[2]);
  ht2.printTable();

  return 0;
}