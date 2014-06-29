#pragma once

#include "board.h"
#include "ChildNode.h"

class SearchNode
{
public:

	/// <summary>
	/// Child nodes. Each node stores the resulting board and the positions
	/// at which a tile was inserted to obtain the board.
	/// Array indices: children[moveIndex][n'th child][value]
	/// </summary>
	ChildNode children[4][BOARD_SIZE_SQ / 2][NEW_VALUE_COUNT];
	unsigned char childCount[4][2];

	SearchNode();

	/// <summary>
	/// Generates all combinations of a tile insert followed by a move. See 'children'.
	/// </summary>
	/// <param name="board">Board before tile insert should take place.</param>
	void generateChildren(BOARD board);
};
