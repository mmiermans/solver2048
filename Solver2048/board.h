#pragma once

#include "bitmath.h"
#include "direction.h"
#include "boardconstants.h"

/// <summary>
/// Class that represents a game board and can perform operations on this board
/// or get basic information about the board.
/// </summary>
class Board
{
public:
	/// <summary>
	/// Calculates the position of a tile on a board.
	/// </summary>
	/// <param name="x">x-coordinate in 0..BOARD_SIZE-1</param>
	/// <param name="y">y-coordinate in 0..BOARD_SIZE-1</param>
	/// <returns>The bit position of a tile, where (0,0) is the top-left corner.</returns>
	static inline int getIndex(int x, int y) { return ((y*BOARD_SIZE) + x)*TILE_BITS; }

	static inline void setTile(BOARD& b, int x, int y, TILE val) { int i = getIndex(x, y); b = (b & ~(TILE_MASK << i)) | (val << i); }
	static inline void setTile(BOARD& b, int i, TILE val) { b = (b & ~(TILE_MASK << i)) | (val << i); }

	static inline TILE getTile(const BOARD b, int x, int y) { int i = getIndex(x, y); return (b & (TILE_MASK << i)) >> i; }

	static inline void clearBoard(BOARD& b) { b = (BOARD)0; }

	static unsigned char getValidMoves(BOARD board);

	static BOARD performMove(BOARD board, Move move);

	/// <summary>
	/// Calculates a board that indicates which tiles are empty
	/// </summary>
	/// <returns>BOARD with empty tiles equal to TILE_MASK</returns>
	static BOARD getEmptyMask(BOARD board);

	/// <summary>
	/// Determines whether a board has empty tiles.
	/// </summary>
	/// <returns>0 if board is contains an empty tile.</returns>
	static BOARD hasEmptyTile(BOARD b);

private:
	/// <summary>
	/// Helper function for getEmptyMask()
	/// </summary>
	static BOARD getEvenColumnsEmptyMask(BOARD b);

	static BOARD moveLeft(BOARD board);
	static BOARD moveRight(BOARD board);
	static BOARD moveUp(BOARD board);
	static BOARD moveDown(BOARD board);
	static BOARD shiftLeft(BOARD board);
	static BOARD shiftRight(BOARD board);
	static BOARD shiftUp(BOARD board);
	static BOARD shiftDown(BOARD board);
};

