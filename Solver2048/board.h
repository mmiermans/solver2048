#pragma once

#include "bitmath.h"
#include "direction.h"

#define BOARD uint64_t
#define BOARD_SIZE 4
#define BOARD_SIZE_SQ (BOARD_SIZE*BOARD_SIZE)
#define BOARD_MASK (~0)

#define TILE __int64
#define TILE_BITS 4
#define TILE_MAX ((1<<TILE_BITS)-1)
#define TILE_MASK ((__int64)0xF)

#define ROW_BITS (BOARD_SIZE*TILE_BITS)
#define ROW_MASK ((1<<ROW_BITS)-1)

#define BOARD_BITS (BOARD_SIZE_SQ*TILE_BITS)

/// <summary>
/// Class that represents a game board and can perform operations on this board
/// or get basic information about the board.
/// </summary>
class Board
{
public:
	Board();
	Board(const Board& board);
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
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	/// <summary>
	/// Calculates a board that indicates which tiles are empty
	/// </summary>
	/// <returns>BOARD with empty tiles equal to TILE_MASK</returns>
	BOARD getEmptyMask();
	BOARD getEmptyMask(BOARD b);

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
	/// Determines whether a board has empty tiles.
	/// </summary>
	/// <returns>0 if board is contains an empty tile.</returns>
	BOARD hasEmptyTile(BOARD b);

	/// <summary>
	/// Helper function for getEmptyMask()
	/// </summary>
	BOARD getEvenColumnsEmptyMask(BOARD b);

	/// <summary>
	/// Shifts a row left, removing empty tiles and merging equal tiles.
	/// </summary>
	/// <param name="row">Row, where tiles are assumed to be on position 0..BOARD_SIZE-1.</param>
	/// <returns>Row shifted left. Only positions 0..BOARD_SIZE-1 can contain high bits.</returns>
	BOARD shiftRowLeft(const BOARD& row);

	inline BOARD lastColumnMask() { BOARD m = 0; for (int i = 0; i < BOARD_SIZE; i++) m |= TILE_MASK << getIndex(BOARD_SIZE - 1, i); return m; }

	inline BOARD notLastColumnMask() { return (~lastColumnMask()) & BOARD_MASK; }

	inline BOARD firstColumnMask() { BOARD m = 0; for (int i = 0; i < BOARD_SIZE; i++) m |= TILE_MASK << getIndex(0, i); return m; }

	inline BOARD secondColumnMask() { return (firstColumnMask() << TILE_BITS) & BOARD_MASK; }

	inline BOARD notFirstColumnMask() { return (~firstColumnMask()) & BOARD_MASK; }

	inline BOARD evenColumnMask() { BOARD m = 0; for (int i = 0; i < BOARD_SIZE_SQ / 2; i++) m = TILE_MASK | (m << (2 * TILE_BITS)); return m; }

	inline BOARD oddColumnMask() { return (~evenColumnMask()) & BOARD_MASK; }

	inline BOARD firstRowMask() { BOARD m = 0; for (int i = 0; i < BOARD_SIZE; i++) m |= TILE_MASK << getIndex(i, 0); return m; }

	inline BOARD lastRowMask() { BOARD m = 0; for (int i = 0; i < BOARD_SIZE; i++) m |= TILE_MASK << getIndex(i, BOARD_SIZE-1); return m; }

	inline BOARD unityMask() { BOARD m = 0; for (int i = 0; i < BOARD_SIZE_SQ; i++) m = 1 | (m << TILE_BITS); return m; }

	inline BOARD msbMask() { return unityMask() << (TILE_BITS - 1); }
};

