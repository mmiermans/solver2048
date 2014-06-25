#pragma once

#include "board.h"
#include "direction.h"
#include "searchnode.h"

class Engine
{
public:
	bool debug = true;
	int depth = 10;

	Engine();
	~Engine();

	Move solve(Board& board);

private:
	int evaluateBoard(Board& board);
	SearchNode* nodes;

	int solveRecursive(int index);
};

