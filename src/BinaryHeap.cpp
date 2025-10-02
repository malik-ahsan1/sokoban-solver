#include "BinaryHeap.h"

// Constructor
template <typename T>
BinaryHeap<T>::BinaryHeap() : size(0)
{
  // Array constructor handles its own initialization
}

// Copy constructor
template <typename T>
BinaryHeap<T>::BinaryHeap(const BinaryHeap &other) : data(other.data), size(other.size)
{
}

// Assignment operator
template <typename T>
BinaryHeap<T> &BinaryHeap<T>::operator=(const BinaryHeap &other)
{
  if (this != &other)
  {
    data = other.data;
    size = other.size;
  }
  return *this;
}

// Insert element - O(log n)
template <typename T>
void BinaryHeap<T>::push(const T &value)
{
  // Add element at the end
  data.push_back(value);
  size++;

  // Restore heap property by bubbling up
  bubbleUp(size - 1);
}

// Remove and return minimum element - O(log n)
template <typename T>
T BinaryHeap<T>::pop()
{
  if (isEmpty())
  {
    // In a real implementation, you might throw an exception here
    // For simplicity, we'll assume this won't be called on empty heap
  }

  // Store the minimum element (root)
  T minElement = data[0];

  // Move last element to root
  data[0] = data[size - 1];
  size--;
  data.pop_back();

  // Restore heap property by bubbling down (if heap is not empty)
  if (size > 0)
  {
    bubbleDown(0);
  }

  return minElement;
}

// Return minimum element without removing - O(1)
template <typename T>
const T &BinaryHeap<T>::peek() const
{
  // Assume heap is not empty (in real implementation, check and throw exception)
  return data[0];
}

// Check if heap is empty
template <typename T>
bool BinaryHeap<T>::isEmpty() const
{
  return size == 0;
}

// Return number of elements
template <typename T>
int BinaryHeap<T>::getSize() const
{
  return size;
}

// Restore heap property after insertion (bubble up)
template <typename T>
void BinaryHeap<T>::bubbleUp(int i)
{
  // Continue until we reach root or heap property is satisfied
  while (i > 0)
  {
    int parentIndex = parent(i);

    // If heap property is satisfied, stop
    if (data[parentIndex] <= data[i])
    {
      break;
    }

    // Swap with parent and continue
    T temp = data[i];
    data[i] = data[parentIndex];
    data[parentIndex] = temp;

    i = parentIndex;
  }
}

// Restore heap property after removal (bubble down)
template <typename T>
void BinaryHeap<T>::bubbleDown(int i)
{
  while (true)
  {
    int left = leftChild(i);
    int right = rightChild(i);
    int smallest = i;

    // Find the smallest among node and its children
    if (left < size && data[left] < data[smallest])
    {
      smallest = left;
    }

    if (right < size && data[right] < data[smallest])
    {
      smallest = right;
    }

    // If heap property is satisfied, stop
    if (smallest == i)
    {
      break;
    }

    // Swap with smallest child and continue
    T temp = data[i];
    data[i] = data[smallest];
    data[smallest] = temp;

    i = smallest;
  }
}

// Explicit template instantiations for commonly used types
template class BinaryHeap<int>;
template class BinaryHeap<double>;
template class BinaryHeap<float>;
template class BinaryHeap<char>;
template class BinaryHeap<long>;
template class BinaryHeap<short>;

// Include AStarNode definition and instantiate
#include "SolverAStar.h"
template class BinaryHeap<AStarNode>;