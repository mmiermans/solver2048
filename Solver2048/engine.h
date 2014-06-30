#pragma once

#include <sparsehash/internal/sparseconfig.h>
#include <sparsehash/dense_hash_map>

#include "bitmath.h"
#include "board.h"
#include "direction.h"
#include "searchnode.h"

class Engine
{
public:
	Engine();
	~Engine();

	Move solve(Board board);

	void setRandomTile(Board& board);

private:
	fastrand* fastRng;
	SearchNode* nodes;
	google::dense_hash_map<Board, double> scoreMap;

	double solveRecursive(int index, Board b);

	int evaluateBoard(Board b);
};

