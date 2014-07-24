#pragma once

#define ENABLE_HASHING
#define CUSTOM_HASHING

#ifdef ENABLE_HASHING
#ifndef CUSTOM_HASHING
#define GOOGLE_HASHING
#endif
#endif

//#define ENABLE_SAMPLING

#include <time.h>

#include "bitmath.h"
#include "board.h"
#include "direction.h"
#include "searchnode.h"

// TEST
#include <map>
#include <vector>

#ifdef CUSTOM_HASHING
#include "boardhashtable.h"
#else
#include <sparsehash/internal/sparseconfig.h>
#include <sparsehash/dense_hash_map>
typedef google::dense_hash_map<Board, float> hash_t;
#endif

class Engine
{
public:
	Engine();
	~Engine();

	Move::MoveEnum solve(Board board);

	void setRandomTile(Board& board);

	void getRandomTile(Board board, int& position, Tile& value);
	
	/// <summary>
	/// Calculates a cost for a given board
	/// </summary>
	/// <param name="b">Board to evaluate</param>
	/// <returns>Positive cost (lower is better)</returns>
	uint32_t evaluateBoard(Board b);

	// Debug stats
	int hashHits;
	int hashMisses;
	uint64_t nodeCounter;
	clock_t cpuTime;
	int moveCounter[4];

	int dfsLookAhead;

private:
	std::map< Board, std::vector<Board> > reverseHashTable;

	fastrand* fastRng;
	SearchNode* nodes;

#ifdef CUSTOM_HASHING
	BoardHashTable boardHashTable;
#else
	hash_t scoreMap;
#endif

	float depthFirstSolve(int index, Board b);

	int maxTileAfterSequence(Board b);
};

