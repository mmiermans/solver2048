#pragma once

#include "bitmath.h"
#include "direction.h"
#include "boardconstants.h"

/// <summary>
/// Class that represents a game board and can perform operations on this board
/// or get basic information about the board.
/// </summary>
class BoardLogic
{
public:

	/// <summary>
	/// Calculates the position of a tile on a board.
	/// </summary>
	/// <param name="x">x-coordinate in 0..BOARD_SIZE-1</param>
	/// <param name="y">y-coordinate in 0..BOARD_SIZE-1</param>
	/// <returns>The bit position of a tile, where (0,0) is the top-left corner.</returns>
	static inline int getIndex(int x, int y) { return ((y*BOARD_SIZE) + x)*TILE_BITS; }

	static inline void setTile(Board& b, int x, int y, Tile val) { int i = getIndex(x, y); b = (b & ~(TILE_MASK << i)) | (val << i); }
	static inline void setTile(Board& b, int i, Tile val) { b = (b & ~(TILE_MASK << i)) | (val << i); }

	static inline Tile getTile(const Board b, int x, int y) { int i = getIndex(x, y); return (b & (TILE_MASK << i)) >> i; }

	static inline void clearBoard(Board& b) { b = (Board)0; }

	static void printBoard(Board board);

	static unsigned char getValidMoves(Board board);

	static bool hasValidMoves(Board board);

	static Board performMove(Board board, Move::MoveEnum move);

	static inline int sumTiles(Board b) { int score = 0; while (b) { score += (~1) & (1 << (b & TILE_MASK)); b >>= TILE_BITS; } return score; }

	static int calculateScore(Board b, int moveCount);

	static inline int maxTile(Board b) { Tile m = 0; while (b) { Tile t = b & TILE_MASK; if (t > m) m = t; b >>= TILE_BITS; } return (int)((1 << m) & (~1)); }

	/// <summary>
	/// Calculates a board that indicates which tiles are empty
	/// </summary>
	/// <returns>Board with empty tiles equal to TILE_MASK</returns>
	static Board getEmptyMask(Board board);

	static int getEmptyCount(Board board) { return BitMath::popCount(getEmptyMask(board)) / TILE_BITS; }

	/// <summary>
	/// Determines whether a board has empty tiles.
	/// </summary>
	/// <returns>0 if board is contains an empty tile.</returns>
	static Board hasEmptyTile(Board b);

private:

	/// <summary>
	/// Helper function for getEmptyMask()
	/// </summary>
	static Board getEvenColumnsEmptyMask(Board b);

	static Board moveLeft(Board board);
	static Board moveRight(Board board);
	static Board moveUp(Board board);
	static Board moveDown(Board board);
	static Board shiftLeft(Board board);
	static Board shiftRight(Board board);
	static Board shiftUp(Board board);
	static Board shiftDown(Board board);
};

