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

	Move solve(BOARD board);

	void setRandomTile(BOARD& board);

private:
	fastrand* fastRng;
	int evaluateBoard(BOARD b);
	SearchNode* nodes;
	Move activeMove;

	double solveRecursive(int index, BOARD b);
};

