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
	Board();
	Board(const Board& board);
	Board(BOARD board);
	~Board();
	

	inline BOARD getBoard() { return board; }

	inline void setTile(int x, int y, TILE val) { int i = getIndex(x, y); board = (board & ~(TILE_MASK << i)) | (val << i); }
	inline void setTile(int i, TILE val) { board = (board & ~(TILE_MASK << i)) | (val << i); }

	inline int getTile(int x, int y) { int i = getIndex(x, y); return (board & (TILE_MASK << i)) >> i; }

	inline void setBoard(const Board& board) { this->board = board.board; }
	inline void setBoard(BOARD board) { this->board = board; }

	inline void clearBoard() { board = (BOARD)0; }

	unsigned char getValidMoves();

	void performMove(Move move);

	/// <summary>
	/// Calculates a board that indicates which tiles are empty
	/// </summary>
	/// <returns>BOARD with empty tiles equal to TILE_MASK</returns>
	BOARD getEmptyMask();
	BOARD getEmptyMask(BOARD b);

	/// <summary>
	/// Determines whether a board has empty tiles.
	/// </summary>
	/// <returns>0 if board is contains an empty tile.</returns>
	BOARD hasEmptyTile(BOARD b);

private:
	BOARD board;

	/// <summary>
	/// Calculates the position of a tile on a board.
	/// </summary>
	/// <param name="x">x-coordinate in 0..BOARD_SIZE-1</param>
	/// <param name="y">y-coordinate in 0..BOARD_SIZE-1</param>
	/// <returns>The bit position of a tile, where (0,0) is the top-left corner.</returns>
	inline int getIndex(int x, int y) { return ((y*BOARD_SIZE) + x)*TILE_BITS; }

	/// <summary>
	/// Helper function for getEmptyMask()
	/// </summary>
	BOARD getEvenColumnsEmptyMask(BOARD b);

	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void shiftLeft();
	void shiftRight();
	void shiftUp();
	void shiftDown();
};

