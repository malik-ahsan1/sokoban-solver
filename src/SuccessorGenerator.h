#pragma once

#include "State.h"
#include "Board.h"
#include "Door.h"
#include "Array.h"
#include "PlayerPathfinder.h"
#include "Meta.h"

// Phase 5: Push-based neighbor generation
// Generates successor states by considering all possible box pushes
Array<State> generateSuccessors(const State &s, const Board &board, const Array<Door> &doors, const Meta &meta);