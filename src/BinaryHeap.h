#pragma once
#include "Array.h"

template <typename T>
class BinaryHeap
{
private:
  Array<T> data; // dynamic array for storage
  int size;      // current number of elements

  // Helper methods for heap operations
  void bubbleUp(int i);   // restore heap after insertion
  void bubbleDown(int i); // restore heap after removal

  // Index calculation helpers
  int parent(int i) const { return (i - 1) / 2; }
  int leftChild(int i) const { return 2 * i + 1; }
  int rightChild(int i) const { return 2 * i + 2; }

public:
  // Constructor
  BinaryHeap();

  // Destructor (default is fine since Array handles its own memory)
  ~BinaryHeap() = default;

  // Copy constructor
  BinaryHeap(const BinaryHeap &other);

  // Assignment operator
  BinaryHeap &operator=(const BinaryHeap &other);

  // Core operations
  void push(const T &value); // O(log n) - insert element
  T pop();                   // O(log n) - remove and return min element
  const T &peek() const;     // O(1) - return min element without removing

  // Utility methods
  bool isEmpty() const; // check if heap is empty
  int getSize() const;  // return number of elements
};