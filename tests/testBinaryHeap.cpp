#include <iostream>
#include "../src/BinaryHeap.h"

int main()
{
  std::cout << "=== Binary Heap Test ===" << std::endl;

  // Create a min-heap of integers
  BinaryHeap<int> heap;

  std::cout << "Initial heap size: " << heap.getSize() << std::endl;
  std::cout << "Is empty: " << (heap.isEmpty() ? "Yes" : "No") << std::endl;

  // Insert test values [5, 3, 8, 1]
  std::cout << "\nInserting values: 5, 3, 8, 1" << std::endl;
  heap.push(5);
  std::cout << "Inserted 5, heap size: " << heap.getSize() << ", min: " << heap.peek() << std::endl;

  heap.push(3);
  std::cout << "Inserted 3, heap size: " << heap.getSize() << ", min: " << heap.peek() << std::endl;

  heap.push(8);
  std::cout << "Inserted 8, heap size: " << heap.getSize() << ", min: " << heap.peek() << std::endl;

  heap.push(1);
  std::cout << "Inserted 1, heap size: " << heap.getSize() << ", min: " << heap.peek() << std::endl;

  // Verify peek() returns minimum (1)
  std::cout << "\nVerification: peek() = " << heap.peek() << " (should be 1)" << std::endl;

  // Pop all elements and print in ascending order
  std::cout << "\nPopping all elements (should be in ascending order):" << std::endl;
  while (!heap.isEmpty())
  {
    int min = heap.pop();
    std::cout << "Popped: " << min << ", remaining size: " << heap.getSize() << std::endl;
  }

  std::cout << "Final heap size: " << heap.getSize() << std::endl;
  std::cout << "Is empty: " << (heap.isEmpty() ? "Yes" : "No") << std::endl;

  // Test with more values to verify heap behavior
  std::cout << "\n=== Additional Test with More Values ===" << std::endl;
  BinaryHeap<int> heap2;

  // Insert values: 10, 4, 15, 20, 25, 12, 7
  int values[] = {10, 4, 15, 20, 25, 12, 7};
  int numValues = sizeof(values) / sizeof(values[0]);

  std::cout << "Inserting values: ";
  for (int i = 0; i < numValues; i++)
  {
    std::cout << values[i] << " ";
    heap2.push(values[i]);
  }
  std::cout << std::endl;

  std::cout << "Heap size: " << heap2.getSize() << std::endl;
  std::cout << "Minimum element: " << heap2.peek() << std::endl;

  std::cout << "\nExtracting all elements in sorted order:" << std::endl;
  while (!heap2.isEmpty())
  {
    std::cout << heap2.pop() << " ";
  }
  std::cout << std::endl;

  // Test with double values
  std::cout << "\n=== Test with Double Values ===" << std::endl;
  BinaryHeap<double> doubleHeap;

  doubleHeap.push(3.14);
  doubleHeap.push(2.71);
  doubleHeap.push(1.41);
  doubleHeap.push(0.57);

  std::cout << "Double heap - extracting in sorted order: ";
  while (!doubleHeap.isEmpty())
  {
    std::cout << doubleHeap.pop() << " ";
  }
  std::cout << std::endl;

  return 0;
}