#pragma once

template <typename T>
class Array
{
private:
  T *data;      // pointer to raw array
  int capacity; // allocated size
  int size;     // number of elements stored

public:
  // Constructor - allocate initial memory (capacity = 4)
  Array();

  // Destructor - free memory
  ~Array();

  // Copy constructor
  Array(const Array &other);

  // Assignment operator
  Array &operator=(const Array &other);

  // Add new element at end; resize if full
  void push_back(const T &x);

  // Remove last element, shrink size by 1
  void pop_back();

  // Direct access to element i
  T &operator[](int i);

  // Direct access to element i (const version)
  const T &operator[](int i) const;

  // Grow array when full (double capacity)
  void resize(int newCap);

  // Return number of elements
  int getSize() const;

  // Return capacity (debug)
  int getCapacity() const;

  // Check if array is empty
  bool empty() const;
};