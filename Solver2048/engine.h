#pragma once

#include <time.h>

#include <sparsehash/internal/sparseconfig.h>
#include <sparsehash/dense_hash_map>

#include "bitmath.h"
#include "board.h"
#include "direction.h"
#include "searchnode.h"
#include "boardhashtable.h"

// TEST
#include <map>
#include <vector>

typedef google::dense_hash_map<Board, float> hash_t;

#define ENABLE_HASHING
//#define CUSTOM_HASHING
//#define ENABLE_SAMPLING

class Engine
{
public:
	Engine();
	~Engine();

	Move solve(Board board);

	void setRandomTile(Board& board);
	
	/// <summary>
	/// Calculates a cost for a given board
	/// </summary>
	/// <param name="b">Board to evaluate</param>
	/// <returns>Positive cost (lower is better)</returns>
	uint32_t evaluateBoard(Board b);

	// Debug stats
	int hashHits = 0;
	int hashMisses = 0;
	uint64_t nodeCounter = 0;
	clock_t cpuTime = 0;
	int moveCounter[4];

	int dfsLookAhead;

private:
	std::map<Board, std::vector<Board>> reverseHashTable;

	fastrand* fastRng;
	SearchNode* nodes;
	hash_t scoreMap;
	BoardHashTable boardHashTable;

	float depthFirstSolve(int index, Board b, float scoreSum);
};

