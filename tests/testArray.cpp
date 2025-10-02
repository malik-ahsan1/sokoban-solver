#include <iostream>
#include "../src/Array.h"

int main()
{
  // Create an array of integers
  Array<int> arr;

  std::cout << "Initial capacity: " << arr.getCapacity() << std::endl;
  std::cout << "Initial size: " << arr.getSize() << std::endl;
  std::cout << "Is empty: " << (arr.empty() ? "Yes" : "No") << std::endl;

  // Test push_back - add elements
  std::cout << "\nAdding elements 1, 2, 3, 4, 5..." << std::endl;
  for (int i = 1; i <= 5; i++)
  {
    arr.push_back(i);
    std::cout << "Added " << i << " - Size: " << arr.getSize()
              << ", Capacity: " << arr.getCapacity() << std::endl;
  }

  // Test operator[] - access elements
  std::cout << "\nElements in array: ";
  for (int i = 0; i < arr.getSize(); i++)
  {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;

  // Test pop_back - remove elements
  std::cout << "\nRemoving elements..." << std::endl;
  while (!arr.empty())
  {
    std::cout << "Removing " << arr[arr.getSize() - 1]
              << " - Size becomes: " << arr.getSize() - 1 << std::endl;
    arr.pop_back();
  }

  std::cout << "Final size: " << arr.getSize() << std::endl;
  std::cout << "Is empty: " << (arr.empty() ? "Yes" : "No") << std::endl;

  // Test with different data type
  std::cout << "\nTesting with double values..." << std::endl;
  Array<double> doubleArr;
  doubleArr.push_back(3.14);
  doubleArr.push_back(2.71);
  doubleArr.push_back(1.41);

  std::cout << "Double array elements: ";
  for (int i = 0; i < doubleArr.getSize(); i++)
  {
    std::cout << doubleArr[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}