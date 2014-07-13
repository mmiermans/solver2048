#pragma once

#include <time.h>

#include <sparsehash/internal/sparseconfig.h>
#include <sparsehash/dense_hash_map>

#include "bitmath.h"
#include "board.h"
#include "direction.h"
#include "searchnode.h"

typedef google::dense_hash_map<Board, double> hash_t;

class Engine
{
public:
	Engine();
	~Engine();

	Move solve(Board board);

	void setRandomTile(Board& board);

	int evaluateBoard(Board b);

	// Debug stats
	int hashHits = 0;
	int hashMisses = 0;
	uint64_t nodeCounter = 0;
	clock_t cpuTime = 0;
	int moveCounter[4];

private:
	fastrand* fastRng;
	SearchNode* nodes;
	hash_t scoreMap;
	int dfsLookAhead;

	double depthFirstSolve(int index, Board b, double scoreSum);
};

