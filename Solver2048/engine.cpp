#include <iostream>
#include <string>
#include <stdio.h>

#include <sparsehash/internal/sparseconfig.h>
#include <sparsehash/dense_hash_set>

#include "engine.h"
#include "searchnode.h"

#define LOOK_AHEAD 2

Engine::Engine()
{
	fastRng = new fastrand;
	BitMath::initRng(fastRng);

	nodes = new SearchNode[LOOK_AHEAD];
}

Engine::~Engine()
{
	delete fastRng;
	delete[] nodes;
}

Move Engine::solve(BOARD board) {
	double bestScore = INT_MIN;
	Move bestMove = (Move)0;

#if 0
	for (Move move = Move::Left; move != 0; move = (Move)((unsigned int)move >> 1)) {
		if ((move & validMoves) != 0) {
			activeMove = move;
			// Initialize root search node.
			Board rootBoard(board);
			rootBoard.performMove(move);
			nodes[0].parentBoard = &rootBoard;

			// Test
			int eval = evaluateBoard(rootBoard);

			double score = solveRecursive(0);
			if (bestScore < score) {
				bestScore = score;
				bestMove = move;
			}
		}
	}
#endif

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

double Engine::solveRecursive(int index, BOARD b) {
	if (Board::hasEmptyTile(b) == false) {
		return evaluateBoard(b);
	}

	SearchNode& node = nodes[index];
	node.generateChildren(b);

//	node.initialize();

	//if (node.validMoves == (Move)0) {
	//	return evaluateBoard(node.board) - (1 << 17);
	//}
	double currentScore = 0;
	double bestScore = INT_MIN;
	bool hasNext = true;

	return bestScore;
}

int Engine::evaluateBoard(BOARD b) {
	BOARD previousTile = 1 << (b & TILE_MASK);
	int score = (int)previousTile;
	int sign = 1;

#if BOARD_SIZE == 4
	b = (b & 0x0000FFFF0000FFFF) |
		((b & 0x000F0000000F0000) << 12) |
		((b & 0x00F0000000F00000) << 4) |
		((b & 0x0F0000000F000000) >> 4) |
		((b & 0xF0000000F0000000) >> 12);
#else
	Board temp;
	for (int y = 0; y < BOARD_SIZE; y++) {
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			int x = (y % 2 == 0) ? i : BOARD_SIZE - i - 1;
			temp.setTile(i, y, board.getTile(x, y));
		}
	}
	b = temp.getBoard();
#endif

	int i = 1;
	for (; i < BOARD_SIZE_SQ; i++) {
		b >>= TILE_BITS;
		BOARD tile = 1 << (b & TILE_MASK);

		if (tile < previousTile) {
			score += (int)tile;
			previousTile = tile;
		} else {
			if (tile == previousTile) {
				score += (int)tile;
			} else {
				score -= ((i >> BOARD_LOG_SIZE) + 1) * (int)tile;
			}
			break;
		}
	}

	for (; i < BOARD_SIZE_SQ; i++) {
		b >>= TILE_BITS;
		score -= ((i >> BOARD_LOG_SIZE) + 1) * (1 << (b & TILE_MASK));
	}

	// Subtract penalty for 'game over' board.
	if (Board::hasEmptyTile(b) == false) {
		score -= 2048;
	}

	return score;
}

void Engine::setRandomTile(BOARD& board) {
	// Get random numbers
	FastRand_SSE(fastRng);

	// Get random empty tile.
	BOARD emptyMask = Board::getEmptyMask(board);
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
	TILE randomTileValue = (fastRng->res[1] < 0xe6666666) ? 1 : 2;
	Board::setTile(board, randomEmptyTilePosition, randomTileValue);
}