#include "Array.h"
#include "Cell.h"
#include "Door.h"
#include <cstdint>

// Constructor - allocate initial memory (capacity = 4)
template <typename T>
Array<T>::Array() : data(nullptr), capacity(4), size(0)
{
  data = new T[capacity];
}

// Destructor - free memory
template <typename T>
Array<T>::~Array()
{
  delete[] data;
}

// Copy constructor
template <typename T>
Array<T>::Array(const Array &other) : capacity(other.capacity), size(other.size)
{
  data = new T[capacity];
  for (int i = 0; i < size; i++)
  {
    data[i] = other.data[i];
  }
}

// Assignment operator
template <typename T>
Array<T> &Array<T>::operator=(const Array &other)
{
  if (this != &other)
  {
    delete[] data;
    capacity = other.capacity;
    size = other.size;
    data = new T[capacity];
    for (int i = 0; i < size; i++)
    {
      data[i] = other.data[i];
    }
  }
  return *this;
}

// Add new element at end; resize if full
template <typename T>
void Array<T>::push_back(const T &x)
{
  if (size >= capacity)
  {
    resize(capacity * 2);
  }
  data[size] = x;
  size++;
}

// Remove last element, shrink size by 1
template <typename T>
void Array<T>::pop_back()
{
  if (size > 0)
  {
    size--;
  }
}

// Direct access to element i
template <typename T>
T &Array<T>::operator[](int i)
{
  return data[i];
}

// Direct access to element i (const version)
template <typename T>
const T &Array<T>::operator[](int i) const
{
  return data[i];
}

// Grow array when full (double capacity)
template <typename T>
void Array<T>::resize(int newCap)
{
  if (newCap <= capacity)
    return;

  T *newData = new T[newCap];
  for (int i = 0; i < size; i++)
  {
    newData[i] = data[i];
  }
  delete[] data;
  data = newData;
  capacity = newCap;
}

// Return number of elements
template <typename T>
int Array<T>::getSize() const
{
  return size;
}

// Return capacity (debug)
template <typename T>
int Array<T>::getCapacity() const
{
  return capacity;
}

// Check if array is empty
template <typename T>
bool Array<T>::empty() const
{
  return size == 0;
}

// Explicit template instantiations for commonly used types
template class Array<int>;
template class Array<double>;
template class Array<float>;
template class Array<char>;
template class Array<long>;
template class Array<short>;
template class Array<Cell>;
template class Array<Door>;
template class Array<uint64_t>;
template class Array<Array<int>>;  // For 2D arrays in PlayerPathfinder
template class Array<Array<char>>; // For 2D arrays in PlayerPathfinder

// Include BoxInfo definition and instantiate after other templates
#include "State.h"
template class Array<BoxInfo>;
template class Array<State>;

// Template instantiations for SokobanHeuristics
template class Array<bool>;          // For corner detection and goal lookup
template class Array<const State *>; // For path reconstruction

// Include AStarNode definition and instantiate after other templates
#include "SolverAStar.h"
template class Array<AStarNode>; // For BinaryHeap implementation

// Additional template instantiations for simulation
template class Array<Array<char> *>; // For path reconstruction
template class Array<AStarNode *>;   // For node storage in solver