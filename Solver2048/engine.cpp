#include <iostream>
#include <string>
#include <stdio.h>
#include <limits>
#include <float.h>

#include "engine.h"
#include "searchnode.h"

#define LOOK_AHEAD 4

Engine::Engine()
{
	fastRng = new fastrand;
	BitMath::initRng(fastRng);

	scoreMap.set_empty_key(-1);

	nodes = new SearchNode[LOOK_AHEAD];
}

Engine::~Engine()
{
	delete fastRng;
	delete[] nodes;
}

Move Engine::solve(Board board) {
	double bestScore = -DBL_MAX;
	Move bestMove = (Move)0;

	unsigned char validMoves = BoardLogic::getValidMoves(board);

	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		Move move = (Move)(1 << moveIndex);
		if ((validMoves & move) != 0) {
			Board movedBoard = BoardLogic::performMove(board, move);
			double score = solveRecursive(0, movedBoard);
			if (bestScore < score) {
				bestScore = score;
				bestMove = move;
			}
		}
	}

	return bestMove;
}

// TEST
void printMove(Move m) {
	if (m == Move::Left)
		std::cout << "L";
	else if (m == Move::Right)
		std::cout << "R";
	else if (m == Move::Up)
		std::cout << "U";
	else if (m == Move::Down)
		std::cout << "D";
}

double Engine::solveRecursive(int index, Board b) {
	if (BoardLogic::hasEmptyTile(b) == false) {
		return evaluateBoard(b);
	}
	assert(index < LOOK_AHEAD);

	double scores[BOARD_SIZE_SQ][NEW_VALUE_COUNT];
	for (int i = 0; i < BOARD_SIZE_SQ; i++) {
		for (int j = 0; j < NEW_VALUE_COUNT; j++) {
			scores[i][j] = -DBL_MAX;
		}
	}

	SearchNode& node = nodes[index];
	node.generateChildren(b);

	for (int moveIndex = 0; moveIndex < 4; moveIndex++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			int childCount = node.childCount[moveIndex][v];
			for (int childIndex = 0; childIndex < childCount; childIndex++) {
				ChildNode& childNode = node.children[moveIndex][childIndex][v];
				double score;
				if (index < LOOK_AHEAD - 1) {
					score = solveRecursive(index + 1, childNode.board);
				} else {
					score = evaluateBoard(childNode.board);
				}

				for (int p = 0; p < 4 && childNode.positions[p] >= 0; p++) {
					int position = childNode.positions[p];
					if (position >= 0) {
						assert(position < BOARD_SIZE_SQ);
						double& oldScore = scores[position][v];
						if (oldScore < score)
							oldScore = score;
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
	for (int p = 0; p < BOARD_SIZE_SQ; p++) {
		for (int v = 0; v < NEW_VALUE_COUNT; v++) {
			if (scores[p][v] != -DBL_MAX) {
				result += pValue[v] * scores[p][v];
			}
		}
	}

	result /= node.emptyTileCount;

	return result;
}

int Engine::evaluateBoard(Board b) {
	Board previousTile = 1 << (b & TILE_MASK);
	int score = (int)previousTile;
	int sign = 1;

#if BOARD_SIZE == 4
	b = (b & 0x0000FFFF0000FFFF) |
		((b & 0x000F0000000F0000) << 12) |
		((b & 0x00F0000000F00000) << 4) |
		((b & 0x0F0000000F000000) >> 4) |
		((b & 0xF0000000F0000000) >> 12);
#else
	#error TODO: implement for case != 4
#endif

	int i = 1;
	b >>= TILE_BITS;
	while (i < BOARD_SIZE_SQ) {
		Board tile = 1 << (b & TILE_MASK);

		if (tile <= previousTile) {
			score += 10 * (int)tile;
			b >>= TILE_BITS;
			i++;
		}

		if (tile >= previousTile) {
			break;
		}

		previousTile = tile;
	}

	while (b) {
		b >>= TILE_BITS;
		int tile = (b & TILE_MASK);
		score -= tile * (1 << tile);
	}

	// Subtract penalty for 'game over' board.
	if (BoardLogic::hasEmptyTile(b) == false) {
		score -= 1 << 14;
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
	Tile randomTileValue = (fastRng->res[1] < 0xe6666666) ? 1 : 2;
	BoardLogic::setTile(board, randomEmptyTilePosition, randomTileValue);
}