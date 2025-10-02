#include "HashTable.h"
#include <iostream>

// Constructor
template <typename T>
HashTable<T>::HashTable(int cap) : capacity(cap), size(0)
{
  // Allocate array of Node pointers
  table = new Node *[capacity];

  // Initialize all buckets to nullptr
  for (int i = 0; i < capacity; i++)
  {
    table[i] = nullptr;
  }
}

// Destructor
template <typename T>
HashTable<T>::~HashTable()
{
  // Free all nodes in all buckets
  for (int i = 0; i < capacity; i++)
  {
    Node *current = table[i];
    while (current != nullptr)
    {
      Node *temp = current;
      current = current->next;
      delete temp;
    }
  }

  // Free the table array
  delete[] table;
}

// Copy constructor
template <typename T>
HashTable<T>::HashTable(const HashTable &other) : capacity(other.capacity), size(0)
{
  // Allocate new table
  table = new Node *[capacity];

  // Initialize all buckets to nullptr
  for (int i = 0; i < capacity; i++)
  {
    table[i] = nullptr;
  }

  // Copy all elements from other table
  for (int i = 0; i < capacity; i++)
  {
    Node *current = other.table[i];
    while (current != nullptr)
    {
      insert(current->data);
      current = current->next;
    }
  }
}

// Assignment operator
template <typename T>
HashTable<T> &HashTable<T>::operator=(const HashTable &other)
{
  if (this != &other)
  {
    // Clear current table
    this->~HashTable();

    // Copy from other
    capacity = other.capacity;
    size = 0;

    table = new Node *[capacity];
    for (int i = 0; i < capacity; i++)
    {
      table[i] = nullptr;
    }

    // Copy all elements
    for (int i = 0; i < capacity; i++)
    {
      Node *current = other.table[i];
      while (current != nullptr)
      {
        insert(current->data);
        current = current->next;
      }
    }
  }
  return *this;
}

// Insert value into hash table
template <typename T>
void HashTable<T>::insert(const T &value)
{
  // Get bucket index
  int index = hash(value);

  // Check if value already exists in the bucket
  Node *current = table[index];
  while (current != nullptr)
  {
    if (current->data == value)
    {
      // Value already exists, don't insert duplicate
      return;
    }
    current = current->next;
  }

  // Value doesn't exist, insert at beginning of bucket
  Node *newNode = new Node(value);
  newNode->next = table[index];
  table[index] = newNode;
  size++;
}

// Find value in hash table
template <typename T>
bool HashTable<T>::find(const T &value) const
{
  // Get bucket index
  int index = hash(value);

  // Search in the bucket's linked list
  Node *current = table[index];
  while (current != nullptr)
  {
    if (current->data == value)
    {
      return true;
    }
    current = current->next;
  }

  return false;
}

// Remove value from hash table
template <typename T>
bool HashTable<T>::remove(const T &value)
{
  // Get bucket index
  int index = hash(value);

  // Handle empty bucket
  if (table[index] == nullptr)
  {
    return false;
  }

  // Handle removal from beginning of list
  if (table[index]->data == value)
  {
    Node *temp = table[index];
    table[index] = table[index]->next;
    delete temp;
    size--;
    return true;
  }

  // Handle removal from middle or end of list
  Node *current = table[index];
  while (current->next != nullptr)
  {
    if (current->next->data == value)
    {
      Node *temp = current->next;
      current->next = current->next->next;
      delete temp;
      size--;
      return true;
    }
    current = current->next;
  }

  return false;
}

// Get number of elements
template <typename T>
int HashTable<T>::getSize() const
{
  return size;
}

// Get capacity (number of buckets)
template <typename T>
int HashTable<T>::getCapacity() const
{
  return capacity;
}

// Check if table is empty
template <typename T>
bool HashTable<T>::isEmpty() const
{
  return size == 0;
}

// Debug method to print table state
template <typename T>
void HashTable<T>::printTable() const
{
  std::cout << "HashTable contents:" << std::endl;
  for (int i = 0; i < capacity; i++)
  {
    std::cout << "Bucket " << i << ": ";
    Node *current = table[i];
    if (current == nullptr)
    {
      std::cout << "empty";
    }
    else
    {
      while (current != nullptr)
      {
        std::cout << current->data;
        if (current->next != nullptr)
        {
          std::cout << " -> ";
        }
        current = current->next;
      }
    }
    std::cout << std::endl;
  }
}

// Explicit template instantiations for commonly used types
template class HashTable<int>;
template class HashTable<long>;
template class HashTable<short>;
template class HashTable<char>;