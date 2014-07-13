#include <iostream>
#include <string>
#include <stdio.h>
#include <limits>
#include <float.h>
#include <math.h>

#include "engine.h"
#include "searchnode.h"

#define MAX_LOOK_AHEAD 16
#define MIN_SCORE (-DBL_MAX)

Engine::Engine()
	: scoreMap(1<<20)
{
	fastRng = new fastrand;
	BitMath::initRng(fastRng);

	scoreMap.set_empty_key(-1);

	nodes = new SearchNode[MAX_LOOK_AHEAD];

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

Move Engine::solve(Board board) {
	clock_t startTime = clock();

	scoreMap.clear();
	hashHits = 0;
	hashMisses = 0;

	double bestScore = -DBL_MAX;
	int bestMoveIndex = 0;

	int tileSum = BoardLogic::sumTiles(board);

	int t = tileSum / 1024;
	dfsLookAhead = 2;
	while (t > 0) {
		dfsLookAhead++;
		t /= 2;
	}
	if (dfsLookAhead > MAX_LOOK_AHEAD) {
		dfsLookAhead = MAX_LOOK_AHEAD;
	}

	unsigned char validMoves = BoardLogic::getValidMoves(board);

	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move move = (Move)(1 << moveIndex);
		if ((validMoves & move) != 0) {
			Board movedBoard = BoardLogic::performMove(board, move);
			double score = depthFirstSolve(0, movedBoard, 0);
			if (bestScore < score) {
				bestScore = score;
				bestMoveIndex = moveIndex;
			}
		}
	}

	moveCounter[bestMoveIndex]++;

	clock_t endTime = clock();
	cpuTime += (endTime - startTime);

	return (Move)(1 << bestMoveIndex);
}

double Engine::depthFirstSolve(int index, Board b, double scoreSum) {
	double bScore = evaluateBoard(b);
	if (BoardLogic::hasEmptyTile(b) == false) {
		return bScore;
	}

	double scores[BOARD_SIZE_SQ][NEW_VALUE_COUNT];
	for (int i = 0; i < BOARD_SIZE_SQ; i++) {
		for (int j = 0; j < NEW_VALUE_COUNT; j++) {
			scores[i][j] = MIN_SCORE;
		}
	}

	SearchNode& node = nodes[index];
	node.generateChildren(b);
	double childScoreSum = scoreSum + bScore;

	// Divide the 4 random numbers into 8 parts.
	uint16_t* res16 = (uint16_t*)(fastRng->res);

	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			int childCount = node.childCount[moveIndex][v];
			
			// Initialize sampling variables.
			int sampleCount = 8;
			int samples[MAX_SEARCH_NODE_CHILDREN];
			for (int i = 0; i < childCount; i++) {
				samples[i] = i;
			}
			if (sampleCount < childCount) {
				// Get random numbers
				FastRand_SSE(fastRng);
				for (int i = 0; i < sampleCount; i++) {
					// j is a random integer with i <= j < n.
					int j = ((res16[i] % (childCount - i)) + i);
					// exchange samples[j] and samples[i].
					int t = samples[i];
					samples[i] = samples[j];
					samples[j] = t;
				}
			} else {
				sampleCount = childCount;
			}

			for (int sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++) {
				// Get child node
				int childIndex = samples[sampleIndex];
				ChildNode& childNode = node.children[moveIndex][childIndex][v];

				// Get score from hashmap, by recursion or from evaluateBoard().
				double score;
				hash_t::const_iterator it = scoreMap.find(childNode.board);
				if (it != scoreMap.end()) {
					score = it->second;
					hashHits++;
				} else {
					if (index < dfsLookAhead - 1) {
						score = depthFirstSolve(index + 1, childNode.board, childScoreSum);
					} else {
						score = evaluateBoard(childNode.board);
					}
					scoreMap.insert(hash_t::value_type(childNode.board, score));
					hashMisses++;
				}

				// Update score matrix.
				for (int p = 0; p < 4 && childNode.positions[p] >= 0; p++) {
					int position = childNode.positions[p];
					if (position >= 0) {
						assert(position < BOARD_SIZE_SQ);
						double& oldScore = scores[position][v];
						if (oldScore < score)
							oldScore = score;
						nodeCounter++;
					} else {
						break;
					}
				}
			}
		}
	}

	// Aggregate scores
	double result = 0;
	const double pValue[NEW_VALUE_COUNT] = { 0.9, 0.1 };
	double totalWeight = 0;
	for (int p = 0; p < BOARD_SIZE_SQ; p++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			if (scores[p][v] != MIN_SCORE) {
				result += pValue[v] * scores[p][v];
				totalWeight += pValue[v];
			}
		}
	}

	result /= totalWeight;
	if (node.emptyTileCount - totalWeight > 0.001 || node.emptyTileCount - totalWeight < -0.001) {
		totalWeight = totalWeight + 0;
	}

	return result;
}

int Engine::evaluateBoard(Board b) {
	Board previousTile = b & TILE_MASK;
	int score = 10 * (1 << previousTile);

#if BOARD_SIZE == 4
	b = (b & 0x0000FFFF0000FFFF) |
		((b & 0x000F0000000F0000) << 12) |
		((b & 0x00F0000000F00000) << 4) |
		((b & 0x0F0000000F000000) >> 4) |
		((b & 0xF0000000F0000000) >> 12);
#else
	#error TODO: implement for case != 4
#endif

	while (b != 0) {
		b >>= TILE_BITS;
		Board tile = b & TILE_MASK;

		if (tile > previousTile) {
			break;
		}

		score += 10 * (int)(1 << tile);
		previousTile = tile;
	}

	while (b != 0) {
		int tile = (b & TILE_MASK);
		score -= tile * (1 << tile);
		b >>= TILE_BITS;
	}

	// Subtract penalty for 'game over' board.
	if (BoardLogic::hasEmptyTile(b) == false) {
		score -= 10 * (1 << 16);
	}

	return score;
}

void Engine::setRandomTile(Board& board) {
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
	// 0xe6666666 = 0.9 * 2^32.
	Tile randomTileValue = (fastRng->res[1] < PROBABILITY_2) ? 1 : 2;
	BoardLogic::setTile(board, randomEmptyTilePosition, randomTileValue);
}