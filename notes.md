
# NOTES:

WE SEEK COMPLEXITY:
- O(log n)
- O(1)

Searching in an array is O(n) [Binary search], it's inefficient.

# Data structures to use:

## 1. Priority queue (min-heap)
For the open set, used to access the node
with the lowest f-cost in (O(log n))

--> The professor will use Binary Heap -> Min heap specifically
So, a binary heap is one way to build a priority queue,
but other implementations (like Fibonacci heaps) also exist.

- pop() -> O(log n)
- push() -> O(log n)
- find(e) -> O(n)

![Binary Heap Formulas](binary_heap_formulas.png)

Min heap
- State **arr;  //this is what's not on the internet.
- pop()
- push()
- print()





## 2. Hash set
For the closed set, to quickly search if a node
has already been evaluated with an (O(1) average case)







# Class Wed, August 27

If we wanted to move with A* the complexity would be:
- (4n)^n

Because of this, to move the dude, we must use Dijkstra or another A* for fast pathfinding.

For the open we are ready:
- push()
- open()

For the closed:
- push()
- find()

To implement this, the fastest data structure is a HashTable, the professor recommends this O(1).
AVL is also an option, but gives O(log n) complexity


### Thinking about Sokoban
Suppose we have a parameter
Cost = # points without box
The lower the cost, the closer we will be to the solution.

Another line of thought. 
We take a box and a point,
Cost = Sum of distances to reach the point from the box. Is it a good option?

ideas from people from previous weeks:

the professor proposed following a diagonal to do it.
He says he always proposes it and nobody implements it, will it be the best?)

![Idea 1](idea1)

He has also said that people have implemented the cost as:

totalCost = a * cost1 + b * cost2

and through trial and error they have found the best parameters a and b (like machine learning).

### Professor's comments
- You can use string, but not vector.

- Shortest path for player, Dijkstra.

- Shortest path for box, A* Heuristic


# Class Tue, September 2

### Idea 1

Professor strongly recommended separating operations:
To change the way SolverSokoban is currently handling movements, since the current way doesn't allow for easy testing, is not elegant, and can mess up the code.

![alt text](im1.png)

![alt text](im2.png)

Specifically Operation which should be an abstract class
and override with:
- OperationBox, with Operation method (moves box)
- MovePlayer
- unlockABox

Then, in the solverSokoban,
define:
Operation **O;
1. O[0] = unlockABox
2. O[1] = new MovePlayer(x,y)
3. etc.

with a for loop, this is but it's to be able to test operations individually and all this will be in a for loop, inside the solverSokoban.


The professor created the Operation class, inheritance is still missing

### Idea 2


To show off, you could also parameterize with a parameter k

instead of writing:



| dx  | dy |
|-----|----|
| -1  | 0  |
| +1  | 0  |
| 0   | -1 |
| 0   | +1 |
0   +1

The professor suggests doing it with e ^ pi*i
instead of writing
- O[0] = new Operation(-1,0)
- 0[1] = new Operation(1,0)
- 0[2] = new Operation(0,-1)
- 0[3] = new Operation(0,1)


# Class Wed, September 3

### Idea 1

In class we implemented inheritance for operations

# Class Fri, September 5

### Idea 1

Don't mention "lists" in the report, we're working with arrays

You can use Strings

[for each case] We use State, for dynamic things:
```C++
State {
    int i;
    int j;
    int *Box_i;
    int *Box_j;
    bool *blockedBox; //? redundant with int* key_i and j;
    int* key_j;
    int nBox; //! doesn't go, the number of boxes doesn't change
    A* -> float cost;
    /*
    To optimize, it's important what the cost is based on, we can have several parameters to know which state has the lowest "cost" and therefore, the most likely to reach a solution quickly.
    Manhattan distance is a bad heuristic in this case according to the professor.
    */
    int indexMyKey;  // index of the key I carry
    int energy;
    Doors -> int t;   //time (for door cycles)
    State *parent;    //to reconstruct the path
    String op;        //sequence of operations 'UDRL'
    int* key_i;
    int* key_j;
    int nKey;
}
```
[global]
We use Board, for static things:
```C++
    Board {
        // Constructor receives filename to
        // save file data
        Board(String *filename)
        String name;
        int width;
        int height;
        int energyLimits;
        int moveCost;
        int pushCost;
        int *door_i;
        int *door_j;
        int nDoor;
        int *doorDtOpen;
        int *doorDtClose;
        int *doorDtPhase;
        bool *doorInit;
        char **board;
    }
```

### Idea 2
door topic:

max. 9 doors

Door
- id
- openDt
- closeDt
- phaseDt
- initial

We need a counter for each door (state counter)

```C++
int *doorState   //state of each door
int *doorDtState //time it was in that state
```

# Class Tue, September 9

### Idea 1
the professor recommended I use an array with linked lists for the HashMap (Open Hashing)

Link of interest:
https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/OpenHash.html

I need to implement linked lists to implement the hash

### Idea 2
make several A* (to move), the professor mentioned 3.
- Move player
- Shortest path from a box to a point
- The general A* of states.

### Idea 3
Actually the door issue is automatically solved by the algorithm (maybe not in the most efficient way), but due to the cost and the way it tests things, the program should automatically pass when a door opens.