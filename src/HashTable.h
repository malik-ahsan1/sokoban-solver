#pragma once

template <typename T>
class HashTable
{
private:
  // Internal Node structure for linked list chains
  struct Node
  {
    T data;
    Node *next;

    Node(const T &value) : data(value), next(nullptr) {}
  };

  Node **table; // array of buckets (linked lists)
  int capacity; // number of buckets
  int size;     // total number of elements

  // Hash function (assumes T can be used with % operator)
  int hash(const T &value) const
  {
    return value % capacity;
  }

public:
  // Constructor
  HashTable(int cap);

  // Destructor
  ~HashTable();

  // Copy constructor
  HashTable(const HashTable &other);

  // Assignment operator
  HashTable &operator=(const HashTable &other);

  // Core operations
  void insert(const T &value);     // insert into bucket if not already present
  bool find(const T &value) const; // search bucket, return true if found
  bool remove(const T &value);     // delete node if present, return true/false

  // Utility methods
  int getSize() const;     // return number of stored elements
  int getCapacity() const; // return number of buckets
  bool isEmpty() const;    // check if table is empty

  // Debug method to print table state
  void printTable() const;
};