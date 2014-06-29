#pragma once

#include "board.h"

struct ChildNode
{
	/// <summary>
	/// Board after an insert and move.
	/// </summary>
	BOARD board;
	/// <summary>
	/// The positions on which a tile was inserted to obtain the board.
	/// </summary>
	int8_t positions[4];
};
