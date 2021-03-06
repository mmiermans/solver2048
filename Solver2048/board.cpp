#include <string>
#include <iostream>
#include <iomanip>

#include "board.h"
#include "precomputedmoves.h"

using namespace std;

void BoardLogic::printBoard(Board board) {
	ios::fmtflags f(cout.flags());

	char filler = '|';
	int tileWidth = 7;

	const char* outerBorder = "+------+------+------+------+\n";

	for (int y = 0; y < BOARD_SIZE; y++) {
		cout << outerBorder;
		cout << filler;
		for (int x = 0; x < BOARD_SIZE; x++) {
			cout.width(tileWidth - 2);
			cout.fill(' ');
			Tile logTile = BoardLogic::getTile(board, x, y);
			if (logTile > 0)
				cout << std::dec << (1 << logTile);
			else
				cout << ' ';
			cout << ' ' << filler;
		}
		cout << endl;
	}

	cout << "+------+-- 0x";
	cout << hex << setfill('0') << setw(16) << board << endl;

	// Restore cout state
	cout.flags(f);
}

/// <summary>
/// Determines which moves are possible
/// </summary>
/// <returns>Bit flags of moves that are possible</returns>
unsigned char BoardLogic::getValidMoves(Board board) {
	char validMoves = 0;

	// BoardLogic where non-empty tiles are 0 and empty tile bits are high.
	Board e = getEmptyMask(board);

	// LEFT: check if an empty tile exists left of non-empty tile. Works as follows:
	//   ~e : Negation converts empty tiles into non-empty tiles
	//   e << TILE_BITS : Shift all rows one position to the left.
	//   & eMask : Do not consider the last column.
	if ((e & (((~e) & MASK_COL_NOT_FIRST) >> TILE_BITS)) != 0) {
		validMoves |= Move::Left;
	}

	// RIGHT
	if ((e & (((~e) & MASK_COL_NOT_LAST) << TILE_BITS)) != 0) {
		validMoves |= Move::Right;
	}

	// UP
	if ((e & ((~e) >> ROW_BITS) & BOARD_MASK) != 0) {
		validMoves |= Move::Up;
	}

	// DOWN
	if ((e & ((~e) << ROW_BITS) & BOARD_MASK) != 0) {
		validMoves |= Move::Down;
	}

	// If left or right are not yet valid, test on equal neighbouring tiles.
	if ((validMoves & (Move::Left | Move::Right)) != (Move::Left | Move::Right)) {
		// In eq, tiles with a non-empty equal tile to the right are 0.
		Board eq = MASK_COL_LAST | e | (board ^ (board >> TILE_BITS));
		if (hasEmptyTile(eq) != 0) {
			validMoves |= (Move::Left | Move::Right);
		}
	}

	// If up or down are not yet valid, test on equal neighbouring tiles.
	if ((validMoves & (Move::Up | Move::Down)) != (Move::Up | Move::Down)) {
		// In eq, tiles with a non-empty equal tile below are 0.
		Board eq = e | (board ^ (board >> ROW_BITS));
		if (hasEmptyTile(eq) != 0) {
			validMoves |= (Move::Up | Move::Down);
		}
	}

	return validMoves;
}

/// <summary>
/// Determines whether a move is possible
/// </summary>
/// <returns>True if at least one move is possible</returns>
bool BoardLogic::hasValidMoves(Board board) {
	if (hasEmptyTile(board) != 0) {
		return true;
	}

	// BoardLogic where non-empty tiles are 0 and empty tile bits are high.
	Board e = getEmptyMask(board);
	Board eq;

	// In eq, tiles with a non-empty equal tile to the right are 0.
	eq = MASK_COL_LAST | e | (board ^ (board >> TILE_BITS));
	if (hasEmptyTile(eq) != 0) {
		return true;
	}

	// In eq, tiles with a non-empty equal tile below are 0.
	eq = e | (board ^ (board >> ROW_BITS));
	if (hasEmptyTile(eq) != 0) {
		return true;
	}

	return false;
}

Board BoardLogic::performMove(Board board, Move::MoveEnum move) {
	switch (move) {
		case Move::Left: return moveLeft(board);
		case Move::Right: return moveRight(board);
		case Move::Up: return moveUp(board);
		case Move::Down: return moveDown(board);
	}
	return (Board)board;
}

Board BoardLogic::moveLeft(Board board) {
#if 0
	// Initial tests indicate no speed penalty for using for loop: compiler is probably unrolling.
	// TODO: try to request only those rows that are free to move from memory and see if this is faster.
	Board result = 0;
	for (int i = 0; i < 4; i++) {
		result |= (((Board)precomputedMovesLeft[(board >> (i * ROW_BITS)) & MASK_ROW_FIRST]) << (i * ROW_BITS));
	}
	return result;
#elif 1
	Board result =
		(((Board)precomputedMovesLeft[(board >> (0 * ROW_BITS)) & MASK_ROW_FIRST]) << (0 * ROW_BITS)) |
		(((Board)precomputedMovesLeft[(board >> (1 * ROW_BITS)) & MASK_ROW_FIRST]) << (1 * ROW_BITS)) |
		(((Board)precomputedMovesLeft[(board >> (2 * ROW_BITS)) & MASK_ROW_FIRST]) << (2 * ROW_BITS)) |
		(((Board)precomputedMovesLeft[(board >> (3 * ROW_BITS)) & MASK_ROW_FIRST]) << (3 * ROW_BITS));
	return result;
#else

	board = shiftLeft(board);

	// Merge columns from left to right.
	Board oldColumns = 0;
	for (Board col = MASK_COL_FIRST; col != MASK_COL_LAST; oldColumns |= col, col <<= TILE_BITS) {
		// Calculate mask of tiles to merge.
		Board m = getEmptyMask(board) | (board ^ (board >> TILE_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldColumns) != 0) {
			m = getEmptyMask(m) & col;
			if (m) {
				// Merge tiles
				board = (board + (m & MASK_TILES_LSB)) & ~(m << TILE_BITS);
			}
		} else {
			break;
		}
	}

	board = shiftLeft(board);
	return board;
#endif
}

Board BoardLogic::moveRight(Board board) {
#if 1
	Board result =
		(((Board)precomputedMovesRight[(board >> (0 * ROW_BITS)) & MASK_ROW_FIRST]) << (0 * ROW_BITS)) |
		(((Board)precomputedMovesRight[(board >> (1 * ROW_BITS)) & MASK_ROW_FIRST]) << (1 * ROW_BITS)) |
		(((Board)precomputedMovesRight[(board >> (2 * ROW_BITS)) & MASK_ROW_FIRST]) << (2 * ROW_BITS)) |
		(((Board)precomputedMovesRight[(board >> (3 * ROW_BITS)) & MASK_ROW_FIRST]) << (3 * ROW_BITS));
	return result;
#else
	board = shiftRight(board);

	// Merge columns from right to left.
	Board oldColumns = 0;
	for (Board col = MASK_COL_LAST; col != MASK_COL_FIRST; oldColumns |= col, col >>= TILE_BITS) {
		// Calculate mask of tiles to merge.
		Board m = getEmptyMask(board) | (board ^ (board << TILE_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldColumns) != 0) {
			m = getEmptyMask(m) & col;
			if (m) {
				// Merge tiles
				board = (board + (m & MASK_TILES_LSB)) & ~(m >> TILE_BITS);
			}
		} else {
			break;
		}
	}

	board = shiftRight(board);
	return board;
#endif
}

Board BoardLogic::moveUp(Board board) {
	board = shiftUp(board);

	// Merge columns from top to bottom.
	Board oldRows = 0;
	for (Board row = MASK_ROW_FIRST; row != MASK_COL_LAST; oldRows |= row, row <<= ROW_BITS) {
		// Calculate mask of tiles to merge.
		Board m = getEmptyMask(board) | (board ^ (board >> ROW_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldRows) != 0) {
			m = getEmptyMask(m) & row;
			if (m) {
				// Merge tiles
				board = (board + (m & MASK_TILES_LSB)) & ~(m << ROW_BITS);
			}
		} else {
			break;
		}
	}

	board = shiftUp(board);
	return board;
}

Board BoardLogic::moveDown(Board board) {
	board = shiftDown(board);

	// Merge columns from bottom to top.
	Board oldRows = 0;
	for (Board row = MASK_ROW_LAST; row != MASK_COL_FIRST; oldRows |= row, row >>= ROW_BITS) {
		// Calculate mask of tiles to merge.
		Board m = getEmptyMask(board) | (board ^ (board << ROW_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldRows) != 0) {
			m = getEmptyMask(m) & row;
			if (m) {
				// Merge tiles
				board = (board + (m & MASK_TILES_LSB)) & ~(m >> ROW_BITS);
			}
		} else {
			break;
		}
	}

	board = shiftDown(board);
	return board;
}

Board BoardLogic::shiftLeft(Board board) {
	// Shift tiles to the left.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		Board e = getEmptyMask(board);
		Board shiftMask = (~e) & (e << TILE_BITS) & MASK_COL_NOT_FIRST;
		if (shiftMask != 0) {
			// Shift an extra column.
			shiftMask |= (shiftMask << TILE_BITS) & MASK_COL_NOT_FIRST;
			// Get tiles to shift.
			Board tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles >> TILE_BITS);
		} else {
			break;
		}
	}

	return board;
}

Board BoardLogic::shiftRight(Board board) {
	// Shift tiles to the right.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		Board e = getEmptyMask(board);
		Board shiftMask = (~e) & (e >> TILE_BITS) & MASK_COL_NOT_LAST;
		if (shiftMask != 0) {
			// Shift an extra column.
			shiftMask |= (shiftMask >> TILE_BITS) & MASK_COL_NOT_LAST;
			// Get tiles to shift.
			Board tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles << TILE_BITS);
		} else {
			break;
		}
	}

	return board;
}

Board BoardLogic::shiftUp(Board board) {
	// Shift tiles upward.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		Board e = getEmptyMask(board);
		Board shiftMask = (~e) & (e << ROW_BITS) & BOARD_MASK;
		if (shiftMask != 0) {
			// Shift an extra row.
			shiftMask |= (shiftMask << ROW_BITS) & BOARD_MASK;
			// Get tiles to shift.
			Board tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles >> ROW_BITS);
		} else {
			break;
		}
	}

	return board;
}

Board BoardLogic::shiftDown(Board board) {
	// Shift tiles downward.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		Board e = getEmptyMask(board);
		Board shiftMask = (~e) & (e >> ROW_BITS) & BOARD_MASK;
		if (shiftMask != 0) {
			// Shift an extra row.
			shiftMask |= (shiftMask >> ROW_BITS) & BOARD_MASK;
			// Get tiles to shift.
			Board tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles << ROW_BITS);
		} else {
			break;
		}
	}

	return board;
}

Board BoardLogic::getEvenColumnsEmptyMask(Board b) {
	// BoardLogic is negated, such that empty tiles are 0xF. Odd columns are removed.
	b = (~b) & MASK_COL_EVEN;
	// Even columns are incremented. Only the empty tiles produce a carry.
	b = (b + (MASK_TILES_LSB & MASK_COL_EVEN)) & MASK_COL_ODD;
	// Subtracting the carry that is shifted one tile to the right produced the
	// desired result: empty tiles have high bits.
	return b - (b >> TILE_BITS);
}

Board BoardLogic::getEmptyMask(Board b) {
	return getEvenColumnsEmptyMask(b) |
		(getEvenColumnsEmptyMask(b >> TILE_BITS) << TILE_BITS);
}

/// <returns>true if b has an empty tile.</returns>
Board BoardLogic::hasEmptyTile(Board b) {
	return (b - MASK_TILES_LSB) & (~b) & MASK_TILES_MSB;
}

int BoardLogic::calculateScoreIncrease(Board before, Board after) {
	int tileCount[TILE_MAX+1];

	for (int i = 0; i <= TILE_MAX; ++i) {
		tileCount[i] = 0;
	}

	// Count tiles on board after.
	while (after) {
		tileCount[after & TILE_MASK]++;
		after >>= TILE_BITS;
	}

	// Subtract tiles on board before.
	while (before) {
		tileCount[before & TILE_MASK]--;
		before >>= TILE_BITS;
	}

	// Sum new tiles
	int scoreDelta = 0;
	for (int i = TILE_MAX; i > 1; i--) {
		scoreDelta += tileCount[i] * (1 << i);
		// Account for merged tiles.
		tileCount[i-1] += 2 * tileCount[i];
	}

	return scoreDelta;
}
