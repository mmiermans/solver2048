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
	float bestCost = 0;

	hashHits = 0;
	hashMisses = 0;

	int maxBadTile = maxTileAfterSequence(board);
	int sequenceLen = sequenceLength(board);
	Tile firstTile = (1 << BoardLogic::getTile(board, 0, 0)) & ~1;

	double freeFactor = (float)(BOARD_SIZE_SQ - sequenceLen) / (float)BOARD_SIZE_SQ;
	double boundedMaxTile = fmin(7, maxBadTile);
	double baseLookAhead = 2 + (double)boundedMaxTile / (3 * freeFactor + 0.1);

	// Based on empty tiles after moves.
	int maxEmptyTileCount = 0;
	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move::MoveEnum move = (Move::MoveEnum)(1 << moveIndex);
		Board movedBoard = BoardLogic::performMove(board, move);
		int emptyTileCount = BoardLogic::getEmptyCount(movedBoard);
		if (emptyTileCount > maxEmptyTileCount) {
			maxEmptyTileCount = emptyTileCount;
		}
	}
	if (maxEmptyTileCount <= 2 && firstTile >= 4096) {
		baseLookAhead += 3;
	} else if (maxEmptyTileCount <= 3) {
		baseLookAhead += 1;
	} else if (maxEmptyTileCount <= 2) {
		baseLookAhead += 2;
	} else if (maxEmptyTileCount <= 1) {
		baseLookAhead += 3;
	}

	// Clamp lookahead depending on sequence length and first tile.
	float preliminaryEval = MAX_COST;
	if (firstTile >= 8192) {
		if (this->lastLookAhead > 4) {
			preliminaryEval = costEst;
		} else {
			this->dfsLookAhead = 4;
			evaluateMoves(validMoves, board, preliminaryEval, bestMoveIndex);
		}

		if (firstTile >= 16384 && preliminaryEval > 1000 && maxEmptyTileCount <= 3) {
			if (sequenceLen >= 10) {
				baseLookAhead = 13;
			} else if (sequenceLen >= 7) {
				baseLookAhead = 12;
			} else {
				baseLookAhead = 11;
			}
		} else if (preliminaryEval > 500 && maxEmptyTileCount <= 3) {
			baseLookAhead++;
			baseLookAhead = fmax(10, baseLookAhead);
			baseLookAhead = fmin(11, baseLookAhead);
		} else if (preliminaryEval > 100) {
			baseLookAhead = fmax(9, baseLookAhead);
			baseLookAhead = fmin(11, baseLookAhead);
		} else {
			baseLookAhead = fmax(6, baseLookAhead);
			baseLookAhead = fmin(10, baseLookAhead);
		}
	} else {
		// This board is not good, restrict the look ahead.
		baseLookAhead = fmax(4, baseLookAhead);
		baseLookAhead = fmin(6, baseLookAhead);
	}
	this->dfsLookAhead = (int)baseLookAhead;

	// Determine whether multiple moves are possible.
	if (BitMath::popCount(validMoves) == 1) {
		bestMoveIndex = getMoveIndex((Move::MoveEnum)validMoves);
		this->dfsLookAhead = 0;
	}

#ifdef ENABLE_STDOUT
	// Engine performance statistics
	cout << "LookAhead: " << this->dfsLookAhead << "\t";
	if (preliminaryEval < MAX_COST)
		cout << "preEval: " << (int)preliminaryEval << "\t";
#endif

	if (this->dfsLookAhead > 0) {

		clock_t evalTime = 0;
		do {
			clock_t startEval = clock();
			evaluateMoves(validMoves, board, bestCost, bestMoveIndex);
			endTime = clock();
			evalTime += (endTime - startEval);
		} while ((bestCost > 1000) && (evalTime < CLOCKS_PER_SEC) && ++dfsLookAhead < MAX_LOOK_AHEAD);
	}

#ifdef ENABLE_STDOUT
	cout << "Eval: " << bestCost << endl;
#endif

	moveCounter[bestMoveIndex]++;

	this->cpuTime += (endTime - startTime);
	this->costEst = bestCost;
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
#ifdef CUSTOM_HASHING
					uint64_t hashIndex = boardHashTable.getIndex(childNode.board);
					score = boardHashTable.getValue(hashIndex);
					if (score != boardHashTable.nullValue) {
#else
					hash_t::const_iterator it = scoreMap.find(childNode.board);
					if (it != scoreMap.end()) {
						score = it->second;
#endif
						hashHits++;
					} else {
						hashMisses++;
						score = depthFirstSolve(index + 1, childNode.board);
#ifdef CUSTOM_HASHING
						boardHashTable.putValue(hashIndex, score);
#else
						scoreMap.insert(hash_t::value_type(childNode.board, score));
#endif
					}
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
