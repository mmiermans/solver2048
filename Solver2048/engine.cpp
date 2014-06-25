#include <iostream>
#include <string>
#include "engine.h"
#include "searchnode.h"

#include <sparsehash/internal/sparseconfig.h>
#include <sparsehash/dense_hash_set>

#include <stdio.h>

#define LOOK_AHEAD 3

Engine::Engine()
{
}

Engine::~Engine()
{
}

Move Engine::solve(Board& board) {
	nodes = new SearchNode[LOOK_AHEAD];

	// Set parentNodes
	nodes[0].parentBoard = NULL;
	for (int i = 1; i < LOOK_AHEAD; i++) {
		nodes[i].parentBoard = &(nodes[i - 1].board);
	}

	int bestScore = INT_MIN;
	Move bestMove = (Move)0;

	// Iterate over valid moves from initial board.
	unsigned char validMoves = board.getValidMoves();
	for (Move move = Move::Left; move != 0; move = (Move)((unsigned int)move >> 1)) {
		if ((move & validMoves) != 0) {
			// Initialize root search node.
			Board rootBoard(board);
			rootBoard.performMove(move);
			nodes[0].parentBoard = &rootBoard;
			nodes[0].initialize();

			int score = solveRecursive(0);
			if (bestScore < score) {
				bestScore = score;
				bestMove = move;
			}
		}
	}

	delete [] nodes;

	return bestMove;
}

int Engine::solveRecursive(int index) {
	SearchNode& node = nodes[index];
	bool hasNext = true;

	int currentScore = 0;
	int bestScore = INT_MIN;
	int score;

	do {
		// Get expected score, either by recursing or from evaluation function.
		if (index < LOOK_AHEAD - 1) {
			nodes[index + 1].initialize();
			score = solveRecursive(index + 1);
		} else {
			score = evaluateBoard(node.board);
		}
		currentScore += score;

		Move lastMove = node.activeMove;
		hasNext = node.getNext();

		// If the move has changed during the getNext() call, then check whether
		// this is the best move seen so far.
		if (node.activeMove != lastMove) {
			if (bestScore < currentScore) {
				bestScore = currentScore;
			}
			currentScore = 0;
		}
	} while (hasNext);

	return bestScore;
}

int Engine::evaluateBoard(Board& board) {
	BOARD b = board.getBoard();
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
				score -= (int)tile;
			}
			break;
		}
	}

	for (; i < BOARD_SIZE_SQ; i++) {
		b >>= TILE_BITS;
		score -= 1 << (b & TILE_MASK);
	}

	return score;
}