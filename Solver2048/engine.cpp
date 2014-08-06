#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <limits>
#include <float.h>
#include <math.h>

#include "engine.h"
#include "searchnode.h"

using namespace std;

#define MAX_LOOK_AHEAD 16
#define MAX_COST (FLT_MAX)
#define GAMEOVER_COST (1<<14);

Engine::Engine()
#ifdef GOOGLE_HASHING
	: scoreMap(1<<20)
#endif
{
	hashHits = 0;
	hashMisses = 0;
	nodeCounter = 0;
	cpuTime = 0;
	lastLookAhead = 0;
	costEst = 0;

	fastRng = new fastrand;
	BitMath::initRng(fastRng);

#ifdef GOOGLE_HASHING_DENSE
	scoreMap.set_empty_key(-1);
#endif

	nodes = new SearchNode[MAX_LOOK_AHEAD];
	dfsLookAhead = 2;

	moveCounter[0] = 0;
	moveCounter[1] = 0;
	moveCounter[2] = 0;
	moveCounter[3] = 0;
}

Engine::~Engine()
{
	delete fastRng;
	delete[] nodes;
}

void Engine::clearHash() {
#if defined(CUSTOM_HASHING)
	boardHashTable.clear();
#elif defined(GOOGLE_HASHING_SPARSE)
	scoreMap.clear();
#elif defined(GOOGLE_HASHING_DENSE)
	scoreMap.clear_no_resize();
#endif
}

Move::MoveEnum Engine::solve(Board board) {
	clock_t startTime = clock();
	clock_t endTime;
	unsigned char validMoves = BoardLogic::getValidMoves(board);
	int bestMoveIndex = 0;
	float cost = 0;

	hashHits = 0;
	hashMisses = 0;

	int tileSum = BoardLogic::sumTiles(board);
	int seqSum = sequenceSum(board);

	// Estimate lookahead based on previous cost estimate.
	double baseLookAhead = 4 + (this->costEst / 50);

	double minLookAhead = 4;
	double maxLookAhead = 9;
	if (tileSum > 30000) {
		minLookAhead = 9;
		maxLookAhead = 12;
	} else if (tileSum > 16000) {
		minLookAhead = 7;
		maxLookAhead = 11;
	} else if (tileSum > 8000) {
		minLookAhead = 6;
		maxLookAhead = 11;
	} else if (tileSum > 4000) {
		minLookAhead = 5;
		maxLookAhead = 10;
	}

	if (seqSum < 2048) {
		maxLookAhead = 9;
	}

	// Clamp between min and max.
	baseLookAhead = round(baseLookAhead);
	baseLookAhead = fmax(minLookAhead, baseLookAhead);
	baseLookAhead = fmin(maxLookAhead, baseLookAhead);
	this->dfsLookAhead = (int)baseLookAhead;

	// Determine whether multiple moves are possible.
	if (BitMath::popCount(validMoves) == 1) {
		bestMoveIndex = getMoveIndex((Move::MoveEnum)validMoves);
		this->dfsLookAhead = 0;
	}

	// Perform evaluation
	if (this->dfsLookAhead > 0) {
		evaluateMoves(validMoves, board, cost, bestMoveIndex);

		// Perform evaluation again if the cost drops.
		if (cost - this->costEst > 50 && this->dfsLookAhead < maxLookAhead) {
			this->dfsLookAhead = maxLookAhead;
			evaluateMoves(validMoves, board, cost, bestMoveIndex);
		}
	}

	endTime = clock();

	moveCounter[bestMoveIndex]++;

	this->cpuTime += (endTime - startTime);
	this->costEst = cost;
	this->lastLookAhead = this->dfsLookAhead;

	return (Move::MoveEnum)(1 << bestMoveIndex);
}

void Engine::evaluateMoves(const unsigned char moves, Board b, float& bestCost, int& bestMoveIndex) {
	// Bind lookahead to valid range
	if (dfsLookAhead > MAX_LOOK_AHEAD) {
		dfsLookAhead = MAX_LOOK_AHEAD;
	} else if (dfsLookAhead < 1) {
		dfsLookAhead = 1;
	}

	clearHash();

	bestCost = MAX_COST;
	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move::MoveEnum move = (Move::MoveEnum)(1 << moveIndex);
		if ((moves & move) != 0) {
			Board movedBoard = BoardLogic::performMove(b, move);
			float score = depthFirstSolve(0, movedBoard);
			if (bestCost > score) {
				bestCost = score;
				bestMoveIndex = moveIndex;
			}
		}
	}
}

float Engine::depthFirstSolve(int index, Board b) {
	float scores[BOARD_SIZE_SQ][NEW_VALUE_COUNT];
	for (int i = 0; i < BOARD_SIZE_SQ; i++) {
		for (int j = 0; j < NEW_VALUE_COUNT; j++) {
			scores[i][j] = MAX_COST;
		}
	}

	SearchNode& node = nodes[index];
	node.generateChildren(b);

	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			int childCount = node.childCount[moveIndex][v];

			for (int childIndex = 0; childIndex < childCount; childIndex++) {
				ChildNode& childNode = node.children[moveIndex][childIndex][v];

				// Get score from hashmap, by recursion or from evaluateBoard().
				float score;
				if (BoardLogic::hasValidMoves(childNode.board) == false) {
					score = GAMEOVER_COST;
				}  else if (index >= dfsLookAhead - 2) {
					score = (float)evaluateBoard(childNode.board);
				} else {
#ifdef ENABLE_HASHING
#ifdef CUSTOM_HASHING
					uint64_t hashIndex = boardHashTable.getIndex(childNode.board);
					score = boardHashTable.getValue(hashIndex);
					if (score != boardHashTable.nullValue) {
#elif defined(GOOGLE_HASHING)
					hash_t::const_iterator it = scoreMap.find(childNode.board);
					if (it != scoreMap.end()) {
						score = it->second;
#endif
					} else {
#endif
						score = depthFirstSolve(index + 1, childNode.board);
#ifdef ENABLE_HASHING
#ifdef CUSTOM_HASHING
						boardHashTable.putValue(hashIndex, score);
#elif defined(GOOGLE_HASHING)
						scoreMap.insert(hash_t::value_type(childNode.board, score));
#endif
					}
#endif
				}

				// Update score matrix.
				int8_t* pPos = childNode.positions;
				int8_t* pPosEnd = childNode.positions + 4;
				int8_t p = *pPos;
				do {
					float& oldScore = scores[p][v];
					if (oldScore > score)
						oldScore = score;
					nodeCounter++;
				} while (++pPos != pPosEnd && (p = *pPos) >= 0);
			}
		}
	}

	// Aggregate scores
	float result = 0;
	const float pValue[NEW_VALUE_COUNT] = { 0.9f, 0.1f };
	float totalWeight = 0;
	for (int p = 0; p < BOARD_SIZE_SQ; p++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			if (scores[p][v] != MAX_COST) {
				result += pValue[v] * scores[p][v];
				totalWeight += pValue[v];
			}
		}
	}

	result /= totalWeight;

	return result;
}

uint32_t Engine::evaluateBoard(Board b) {
	Board previousTile = b & TILE_MASK;
	// If first tile is empty, ignore first tile.
	if (previousTile == 0)
		b >>= TILE_BITS;

	b = BoardLogic::flipOddRows(b);

	uint32_t cost = 0;
	// Find first tile that is lower than its previous tile.
	while (true) {
		b >>= TILE_BITS;
		Board tile = b & TILE_MASK;

		if (tile <= previousTile && b != 0) {
			previousTile = tile;
		} else {
			break;
		}
	}

	// Sum all subsequent tiles.
	while (b != 0) {
		cost += (1 << (b & TILE_MASK)) - 1;
		b >>= TILE_BITS;
	}

	return cost;
}

int Engine::maxTileAfterSequence(Board b) {
	int maxTile = 0;
	if (BoardLogic::hasValidMoves(b) == false)
		return 1;

	int seqLen = Engine::sequenceLength(b);
	b = BoardLogic::flipOddRows(b);
	// Remove sequence
	b >>= seqLen * TILE_BITS;

	// Find max tile left on board
	while (b != 0) {
		int tile = (b & TILE_MASK);
		if (tile > maxTile) maxTile = tile;
		b >>= TILE_BITS;
	}

	return maxTile;

}

int Engine::sequenceSum(Board b) {
	int sum = 0;

	int seqLen = Engine::sequenceLength(b);
	b = BoardLogic::flipOddRows(b);
	for (int i = 0; i < seqLen; i++) {
		sum += b & TILE_MASK;
		b >>= TILE_BITS;
	}

	return sum;

}

int Engine::sequenceLength(Board b) {
	b = BoardLogic::flipOddRows(b);

	Board previousTile = b & TILE_MASK;

	// Count sequence length
	int len = previousTile > 0 ? 1 : 0;
	while (true) {
		b >>= TILE_BITS;
		Board tile = b & TILE_MASK;

		if (tile <= 1) {
			break;
		}
		if (tile >= previousTile) {
			len++;
			break;
		}

		previousTile = tile;
		len++;
	}

	return len;
}

int Engine::getMoveIndex(Move::MoveEnum move) {
	switch (move) {
		case Move::Up: return 0;
		case Move::Right: return 1;
		case Move::Down: return 2;
		case Move::Left: return 3;
		default: return -1;
	}
}

void Engine::setRandomTile(Board& board) {
	int position = 0;
	Tile value = 0;
	getRandomTile(board, position, value);
	BoardLogic::setTile(board, position, value);
}

void Engine::getRandomTile(Board board, int& position, Tile& value) {
	// Get random numbers
	FastRand_SSE(fastRng);

	// Get random empty tile.
	Board emptyMask = BoardLogic::getEmptyMask(board);
	int emptyCount = BitMath::popCount(emptyMask) / TILE_BITS;
	int randomEmptyTileNumber = fastRng->res[0] % emptyCount;
	int randomEmptyTilePosition = 0;
	while (true) {
		if (emptyMask & (TILE_MASK << randomEmptyTilePosition)) {
			if (randomEmptyTileNumber-- == 0)
				break;
		}
		randomEmptyTilePosition += TILE_BITS;
	}

	// Get random tile value with P(2|0.9) and P(4|0.1).
	Tile randomTileValue = (fastRng->res[1] < PROBABILITY_2) ? 1 : 2;

	position = randomEmptyTilePosition;
	value = randomTileValue;
}
