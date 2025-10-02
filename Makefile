# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g

# Directories
SRCDIR = src
BINDIR = bin
TESTDIR = tests

# Source library files (your actual project components)
ARRAY_SRC = $(SRCDIR)/Array.cpp
HEAP_SRC = $(SRCDIR)/BinaryHeap.cpp
HASHTABLE_SRC = $(SRCDIR)/HashTable.cpp
BOARD_SRC = $(SRCDIR)/Board.cpp
DOOR_SRC = $(SRCDIR)/Door.cpp
META_SRC = $(SRCDIR)/Meta.cpp
CONFIGPARSER_SRC = $(SRCDIR)/ConfigParser.cpp
STATE_SRC = $(SRCDIR)/State.cpp
ZOBRIST_SRC = $(SRCDIR)/Zobrist.cpp
PATHFINDER_SRC = $(SRCDIR)/PlayerPathfinder.cpp
SUCCESSOR_SRC = $(SRCDIR)/SuccessorGenerator.cpp
HEURISTICS_SRC = $(SRCDIR)/SokobanHeuristics.cpp
# Add more as you implement them:
# STACK_SRC = $(SRCDIR)/Stack.cpp

# Default target - shows available options
all:
	@echo "Available targets:"
	@echo "  test-array     - Build and run Array tests"
	@echo "  test-heap      - Build and run BinaryHeap tests"
	@echo "  test-hashtable - Build and run HashTable tests"
	@echo "  test-board     - Build and run Board tests"
	@echo "  test-door      - Build and run Door tests"
	@echo "  test-meta      - Build and run Meta tests"
	@echo "  test-config    - Build and run ConfigParser tests"
	@echo "  test-state     - Build and run State and Zobrist tests"
	@echo "  test-door-cycle - Build and run Door Cycle Modulus L tests"
	@echo "  test-pathfinder - Build and run PlayerPathfinder tests"
	@echo "  test-successor - Build and run SuccessorGenerator tests"
	@echo "  test-successor-integration - Build and run SuccessorGenerator integration tests"
	@echo "  test-heuristics - Build and run Sokoban heuristics tests"
	@echo "  test-heuristics-integration - Build and run heuristics integration tests"
	@echo "  test-astar-demo - Build and run A* search demonstration"
	@echo "  test-advanced-heuristics - Build and run advanced heuristics tests (Phase 6)"
	@echo "  test-solver    - Build and run A* solver tests"
	@echo "  test-animation - Build and run animation and simulation tests (Phase 8)"
	@echo "  sokoban        - Build main Sokoban game application"
	@echo "  test-integration - Build and run complete integration test"
	@echo "  clean          - Clean all build files"
	@echo "  help           - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make test-array"
	@echo "  make test-heap"
	@echo "  make test-hashtable"
	@echo "  make test-board"
	@echo "  make test-door"
	@echo "  make test-meta"
	@echo "  make test-config"
	@echo "  make clean"

# Create bin directory if it doesn't exist
$(BINDIR):
	mkdir -p $(BINDIR)

# Array test target
test-array: $(BINDIR)/testArray
	@echo "Running Array tests..."
	./$(BINDIR)/testArray

$(BINDIR)/testArray: $(TESTDIR)/testArray.cpp $(ARRAY_SRC) $(DOOR_SRC) $(SRCDIR)/Array.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testArray.cpp $(ARRAY_SRC) $(DOOR_SRC) -o $(BINDIR)/testArray

# BinaryHeap test target
test-heap: $(BINDIR)/testBinaryHeap
	@echo "Running BinaryHeap tests..."
	./$(BINDIR)/testBinaryHeap

$(BINDIR)/testBinaryHeap: $(TESTDIR)/testBinaryHeap.cpp $(HEAP_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(SRCDIR)/BinaryHeap.h $(SRCDIR)/Array.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testBinaryHeap.cpp $(HEAP_SRC) $(ARRAY_SRC) $(DOOR_SRC) -o $(BINDIR)/testBinaryHeap

# HashTable test target
test-hashtable: $(BINDIR)/testHashTable
	@echo "Running HashTable tests..."
	./$(BINDIR)/testHashTable

$(BINDIR)/testHashTable: $(TESTDIR)/testHashTable.cpp $(HASHTABLE_SRC) $(SRCDIR)/HashTable.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testHashTable.cpp $(HASHTABLE_SRC) -o $(BINDIR)/testHashTable

# Board test target
test-board: $(BINDIR)/testBoard
	@echo "Running Board tests..."
	./$(BINDIR)/testBoard

$(BINDIR)/testBoard: $(TESTDIR)/testBoard.cpp $(BOARD_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(SRCDIR)/Board.h $(SRCDIR)/Cell.h $(SRCDIR)/Array.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testBoard.cpp $(BOARD_SRC) $(ARRAY_SRC) $(DOOR_SRC) -o $(BINDIR)/testBoard

# Door test target
test-door: $(BINDIR)/testDoor
	@echo "Running Door tests..."
	./$(BINDIR)/testDoor

$(BINDIR)/testDoor: $(TESTDIR)/testDoor.cpp $(DOOR_SRC) $(SRCDIR)/Door.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testDoor.cpp $(DOOR_SRC) -o $(BINDIR)/testDoor

# Meta test target
test-meta: $(BINDIR)/testMeta
	@echo "Running Meta tests..."
	./$(BINDIR)/testMeta

$(BINDIR)/testMeta: $(TESTDIR)/testMeta.cpp $(META_SRC) $(SRCDIR)/Meta.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testMeta.cpp $(META_SRC) -o $(BINDIR)/testMeta

# ConfigParser test target
test-config: $(BINDIR)/testConfigParser
	@echo "Running ConfigParser tests..."
	./$(BINDIR)/testConfigParser

$(BINDIR)/testConfigParser: $(TESTDIR)/testConfigParser.cpp $(CONFIGPARSER_SRC) $(BOARD_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) $(SRCDIR)/ConfigParser.h $(SRCDIR)/Board.h $(SRCDIR)/Array.h $(SRCDIR)/Door.h $(SRCDIR)/Meta.h $(SRCDIR)/Cell.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testConfigParser.cpp $(CONFIGPARSER_SRC) $(BOARD_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) -o $(BINDIR)/testConfigParser

# State and Zobrist test target
test-state: $(BINDIR)/testState
	@echo "Running State and Zobrist tests..."
	./$(BINDIR)/testState

$(BINDIR)/testState: $(TESTDIR)/testState.cpp $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(CONFIGPARSER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(META_SRC) $(SRCDIR)/State.h $(SRCDIR)/Zobrist.h $(SRCDIR)/Array.h $(SRCDIR)/ConfigParser.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testState.cpp $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(CONFIGPARSER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(META_SRC) -o $(BINDIR)/testState

# Door Cycle Modulus L test target
test-door-cycle: $(BINDIR)/testDoorCycleL
	@echo "Running Door Cycle Modulus L tests..."
	./$(BINDIR)/testDoorCycleL

$(BINDIR)/testDoorCycleL: $(TESTDIR)/testDoorCycleL.cpp $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(SRCDIR)/Board.h $(SRCDIR)/Door.h $(SRCDIR)/State.h $(SRCDIR)/Zobrist.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testDoorCycleL.cpp $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) -o $(BINDIR)/testDoorCycleL

# PlayerPathfinder test target
test-pathfinder: $(BINDIR)/testPlayerPathfinder
	@echo "Running PlayerPathfinder tests..."
	./$(BINDIR)/testPlayerPathfinder

$(BINDIR)/testPlayerPathfinder: $(TESTDIR)/testPlayerPathfinder.cpp $(PATHFINDER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(SRCDIR)/PlayerPathfinder.h $(SRCDIR)/Board.h $(SRCDIR)/Door.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testPlayerPathfinder.cpp $(PATHFINDER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) -o $(BINDIR)/testPlayerPathfinder

# SuccessorGenerator test target
test-successor: $(BINDIR)/testSuccessorGenerator
	@echo "Running SuccessorGenerator tests..."
	./$(BINDIR)/testSuccessorGenerator

$(BINDIR)/testSuccessorGenerator: $(TESTDIR)/testSuccessorGenerator.cpp $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(META_SRC) $(SRCDIR)/SuccessorGenerator.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testSuccessorGenerator.cpp $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(META_SRC) -o $(BINDIR)/testSuccessorGenerator

# SuccessorGenerator integration test
test-successor-integration: $(BINDIR)/testSuccessorIntegration
	@echo "Running SuccessorGenerator integration tests..."
	./$(BINDIR)/testSuccessorIntegration

$(BINDIR)/testSuccessorIntegration: $(TESTDIR)/testSuccessorIntegration.cpp $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(CONFIGPARSER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(META_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testSuccessorIntegration.cpp $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(CONFIGPARSER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(META_SRC) -o $(BINDIR)/testSuccessorIntegration

# Sokoban Heuristics test target
test-heuristics: $(BINDIR)/testSokobanHeuristics
	@echo "Running Sokoban heuristics tests..."
	./$(BINDIR)/testSokobanHeuristics

$(BINDIR)/testSokobanHeuristics: $(TESTDIR)/testSokobanHeuristics.cpp $(HEURISTICS_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testSokobanHeuristics.cpp $(HEURISTICS_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) -o $(BINDIR)/testSokobanHeuristics

# Heuristics Integration test target  
test-heuristics-integration: $(BINDIR)/testHeuristicsIntegration
	@echo "Running heuristics integration tests..."
	./$(BINDIR)/testHeuristicsIntegration

$(BINDIR)/testHeuristicsIntegration: $(TESTDIR)/testHeuristicsIntegration.cpp $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testHeuristicsIntegration.cpp $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) -o $(BINDIR)/testHeuristicsIntegration

# A* Search Demo target
test-astar-demo: $(BINDIR)/testAStarDemo
	@echo "Running A* search demonstration..."
	./$(BINDIR)/testAStarDemo

$(BINDIR)/testAStarDemo: $(TESTDIR)/testAStarDemo.cpp $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testAStarDemo.cpp $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) -o $(BINDIR)/testAStarDemo

# Advanced Heuristics test target (Phase 6)
test-advanced-heuristics: bin/testAdvancedHeuristics
	@echo "Running advanced heuristics tests..."
	./bin/testAdvancedHeuristics

# Stage 3 & 4 Deadlock Tests
bin/testStage3And4Deadlocks: tests/testStage3And4Deadlocks.cpp src/SokobanHeuristics.cpp src/Board.cpp src/State.cpp src/Zobrist.cpp src/Array.cpp src/Door.cpp
	$(CXX) $(CXXFLAGS) -Isrc $< src/SokobanHeuristics.cpp src/Board.cpp src/State.cpp src/Zobrist.cpp src/Array.cpp src/Door.cpp -o $@

test-stage3-stage4: bin/testStage3And4Deadlocks
	@echo "Running Stage 3 and Stage 4 deadlock detection tests..."
	./bin/testStage3And4Deadlocks

# Complete Deadlock System Demo
bin/testCompleteDeadlockSystem: tests/testCompleteDeadlockSystem.cpp src/SokobanHeuristics.cpp src/Board.cpp src/State.cpp src/Zobrist.cpp src/Array.cpp src/Door.cpp
	$(CXX) $(CXXFLAGS) -Isrc $< src/SokobanHeuristics.cpp src/Board.cpp src/State.cpp src/Zobrist.cpp src/Array.cpp src/Door.cpp -o $@

test-complete-system: bin/testCompleteDeadlockSystem
	@echo "Running complete deadlock detection system demonstration..."
	./bin/testCompleteDeadlockSystem

# A* Solver Tests
bin/testSolverSimple: tests/testSolverSimple.cpp src/SolverAStar.cpp src/Simulation.cpp src/SokobanHeuristics.cpp src/SuccessorGenerator.cpp src/PlayerPathfinder.cpp src/Board.cpp src/State.cpp src/Zobrist.cpp src/Array.cpp src/Door.cpp src/Meta.cpp src/BinaryHeap.cpp src/HashTable.cpp src/ConfigParser.cpp
	$(CXX) $(CXXFLAGS) -Isrc $< src/SolverAStar.cpp src/Simulation.cpp src/SokobanHeuristics.cpp src/SuccessorGenerator.cpp src/PlayerPathfinder.cpp src/Board.cpp src/State.cpp src/Zobrist.cpp src/Array.cpp src/Door.cpp src/Meta.cpp src/BinaryHeap.cpp src/HashTable.cpp src/ConfigParser.cpp -o $@

test-solver: bin/testSolverSimple
	@echo "Running A* Solver tests..."
	./bin/testSolverSimple

# Phase 8: Animation and Menu System
SOLVER_SRC = $(SRCDIR)/SolverAStar.cpp
SIMULATION_SRC = $(SRCDIR)/Simulation.cpp

# Main application
$(BINDIR)/sokoban: main.cpp $(SOLVER_SRC) $(SIMULATION_SRC) $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) $(HEAP_SRC) $(HASHTABLE_SRC) $(CONFIGPARSER_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) main.cpp $(SOLVER_SRC) $(SIMULATION_SRC) $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) $(HEAP_SRC) $(HASHTABLE_SRC) $(CONFIGPARSER_SRC) -o $(BINDIR)/sokoban

sokoban: $(BINDIR)/sokoban
	@echo "Sokoban game built successfully!"
	@echo "Run with: ./$(BINDIR)/sokoban"

# Animation tests
$(BINDIR)/testAnimation: $(TESTDIR)/testAnimation.cpp $(SOLVER_SRC) $(SIMULATION_SRC) $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) $(HEAP_SRC) $(HASHTABLE_SRC) $(CONFIGPARSER_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testAnimation.cpp $(SOLVER_SRC) $(SIMULATION_SRC) $(HEURISTICS_SRC) $(SUCCESSOR_SRC) $(PATHFINDER_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) $(META_SRC) $(HEAP_SRC) $(HASHTABLE_SRC) $(CONFIGPARSER_SRC) -o $(BINDIR)/testAnimation

test-animation: $(BINDIR)/testAnimation
	@echo "Running animation and simulation tests..."
	./$(BINDIR)/testAnimation

$(BINDIR)/testAdvancedHeuristics: $(TESTDIR)/testAdvancedHeuristics.cpp $(HEURISTICS_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testAdvancedHeuristics.cpp $(HEURISTICS_SRC) $(BOARD_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(DOOR_SRC) -o $(BINDIR)/testAdvancedHeuristics

# Complete integration test target
test-integration: $(BINDIR)/testIntegration
	@echo "Running complete integration tests..."
	./$(BINDIR)/testIntegration

$(BINDIR)/testIntegration: $(TESTDIR)/testIntegration.cpp $(CONFIGPARSER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(META_SRC) $(SRCDIR)/ConfigParser.h $(SRCDIR)/Board.h $(SRCDIR)/State.h $(SRCDIR)/Zobrist.h | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(TESTDIR)/testIntegration.cpp $(CONFIGPARSER_SRC) $(BOARD_SRC) $(DOOR_SRC) $(STATE_SRC) $(ZOBRIST_SRC) $(ARRAY_SRC) $(META_SRC) -o $(BINDIR)/testIntegration

# Clean build files
clean:
	rm -rf $(BINDIR)/*

# Help target
help: all

# Debug versions of tests
debug-array: CXXFLAGS += -DDEBUG
debug-array: $(BINDIR)/testArray
	@echo "Running Array tests (Debug mode)..."
	./$(BINDIR)/testArray

debug-heap: CXXFLAGS += -DDEBUG
debug-heap: $(BINDIR)/testBinaryHeap
	@echo "Running BinaryHeap tests (Debug mode)..."
	./$(BINDIR)/testBinaryHeap

debug-hashtable: CXXFLAGS += -DDEBUG
debug-hashtable: $(BINDIR)/testHashTable
	@echo "Running HashTable tests (Debug mode)..."
	./$(BINDIR)/testHashTable

debug-board: CXXFLAGS += -DDEBUG
debug-board: $(BINDIR)/testBoard
	@echo "Running Board tests (Debug mode)..."
	./$(BINDIR)/testBoard

debug-door: CXXFLAGS += -DDEBUG
debug-door: $(BINDIR)/testDoor
	@echo "Running Door tests (Debug mode)..."
	./$(BINDIR)/testDoor

debug-meta: CXXFLAGS += -DDEBUG
debug-meta: $(BINDIR)/testMeta
	@echo "Running Meta tests (Debug mode)..."
	./$(BINDIR)/testMeta

debug-config: CXXFLAGS += -DDEBUG
debug-config: $(BINDIR)/testConfigParser
	@echo "Running ConfigParser tests (Debug mode)..."
	./$(BINDIR)/testConfigParser

.PHONY: all clean help test-array test-heap test-hashtable test-board test-door test-meta test-config test-state test-door-cycle test-pathfinder test-successor test-successor-integration test-heuristics test-heuristics-integration test-astar-demo test-advanced-heuristics test-solver test-animation sokoban test-integration debug-array debug-heap debug-hashtable debug-board debug-door debug-meta debug-config