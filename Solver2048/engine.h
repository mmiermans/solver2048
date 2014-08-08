#pragma once

#define CUSTOM_HASHING
//#define GOOGLE_HASHING_DENSE
//#define GOOGLE_HASHING_SPARSE

#if defined(GOOGLE_HASHING_SPARSE) || defined(GOOGLE_HASHING_DENSE)
#define GOOGLE_HASHING
#endif
#if defined(GOOGLE_HASHING_SPARSE) || defined(GOOGLE_HASHING_DENSE) || defined(CUSTOM_HASHING)
#define ENABLE_HASHING
#endif

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
#ifdef GOOGLE_HASHING_SPARSE
#include <sparsehash/sparse_hash_map>
typedef google::sparse_hash_map<Board, float> hash_t;
#else
#include <sparsehash/dense_hash_map>
typedef google::dense_hash_map<Board, float> hash_t;
#endif
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
	float costEst;

	int dfsLookAhead;
	int lastLookAhead;

private:
	fastrand* fastRng;
	SearchNode* nodes;

#ifdef CUSTOM_HASHING
	BoardHashTable boardHashTable;
#endif
#ifdef GOOGLE_HASHING
	hash_t scoreMap;
#endif
	
	void clearHash();

	void evaluateMoves(const unsigned char moves, Board b, float& bestCost, int& bestMoveIndex);

	float depthFirstSolve(int index, Board b);

	int maxTileAfterSequence(Board b);
	
	int sequenceSum(Board b);

	int sequenceLength(Board b);

	int getMoveIndex(Move::MoveEnum move);
};

