#pragma once

// Make enum members accessable through Move::Left, etc.
namespace Move {
	enum MoveEnum
	{
		Up = 0x1,
		Right = 0x2,
		Down = 0x4,
		Left = 0x8
	};
}
