#pragma once

#include "bitmath.h"
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

	void setRandomTile(Board& board);

private:
	fastrand* fastRng;
	int evaluateBoard(Board& board);
	SearchNode* nodes;
	Move activeMove;

	double solveRecursive(int index);
};

