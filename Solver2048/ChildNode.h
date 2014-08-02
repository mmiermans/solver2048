#pragma once

#include "board.h"

struct ChildNode
{
	/// <summary>
	/// Board after an insert and move.
	/// </summary>
	Board board;
	/// <summary>
	/// The positions on which a tile was inserted to obtain the board.
	/// </summary>
	int8_t positions[4];
	
	// This struct is currently padded by 4 bytes. Replace this variable
	// if there is ever a use for this space.
	int32_t _padding4bytes;
};
