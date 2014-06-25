#pragma once

#include "board.h"
#include <limits.h>

class SearchNode
{
public:
	SearchNode();

	Board board;
	Board* parentBoard;

	BOARD emptyMask;

	unsigned char validMoves;
	Move activeMove;

	TILE newValue;

	char emptyPositions[BOARD_SIZE_SQ];
	char* newPosition;

	bool getNext();

	void initialize();
};
