#include "board.h"


Board::Board() {
	board = 0;
}

Board::Board(const Board& board) {
	setBoard(board);
}

Board::~Board() {
}

/// <summary>
/// Determines which moves are possible
/// </summary>
/// <returns>Bit flags of moves that are possible</returns>
char Board::getValidMoves() {
	char validMoves = 0;

	// Board where non-empty tiles are 0 and empty tile bits are high.
	BOARD e = getEmptyMask();

	// LEFT: check if an empty tile exists left of non-empty tile. Works as follows:
	//   ~e : Negation converts empty tiles into non-empty tiles
	//   e << TILE_BITS : Shift all rows one position to the left.
	//   & eMask : Do not consider the last column.
	if ((e & (((~e) & notFirstColumnMask()) >> TILE_BITS)) != 0) {
		validMoves |= Move::Left;
	}

	// RIGHT
	if ((e & (((~e) & notLastColumnMask()) << TILE_BITS)) != 0) {
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
		BOARD eq = lastColumnMask() | e | (board ^ (board >> TILE_BITS));
		if (hasEmptyTile(eq) != 0) {
			validMoves |= (Move::Left | Move::Right);
		}
	}

	// If up or down are not yet valid, test on equal neighbouring tiles.
	if ((validMoves & (Move::Up | Move::Down)) != (Move::Up | Move::Down)) {
		// In eq, tiles with a non-empty equal tile below are 0.
		BOARD eq = e | (board ^ (board >> ROW_BITS));
		if (hasEmptyTile(eq) != 0) {
			validMoves |= (Move::Up | Move::Down);
		}
	}

	return validMoves;
}

void Board::performMove(Move move) {
	switch (move) {
		case Left: moveLeft(); break;
		case Right: moveRight(); break;
		case Up: moveUp(); break;
		case Down: moveDown(); break;
	}
}

void Board::moveLeft() {
	// Merge columns from left to right.
	BOARD oldColumns = 0;
	for (BOARD col = firstColumnMask(); col != lastColumnMask(); oldColumns |= col, col <<= TILE_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask() | (board ^ (board >> TILE_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldColumns) != 0) {
			m = getEmptyMask(m) & col;
			if (m) {
				// Merge tiles
				board = (board + (m & unityMask())) & ~(m << TILE_BITS);
			}
		} else {
			break;
		}
	}

	// Shift tiles to the left.
	for (int i = 0; i < BOARD_SIZE - 1; i++) {
		BOARD e = getEmptyMask();
		BOARD shiftMask = (~e) & (e << TILE_BITS) & notFirstColumnMask();
		if (shiftMask != 0) {
			// Shift an extra column.
			shiftMask |= (shiftMask << TILE_BITS) & notFirstColumnMask();
			// Get tiles to shift.
			BOARD tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles >> TILE_BITS);
		} else {
			break;
		}
	}
}

void Board::moveRight() {
	// Merge columns from right to left.
	BOARD oldColumns = 0;
	for (BOARD col = lastColumnMask(); col != firstColumnMask(); oldColumns |= col, col >>= TILE_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask() | (board ^ (board << TILE_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldColumns) != 0) {
			m = getEmptyMask(m) & col;
			if (m) {
				// Merge tiles
				board = (board + (m & unityMask())) & ~(m >> TILE_BITS);
			}
		} else {
			break;
		}
	}

	// Shift tiles to the right.
	for (int i = 0; i < BOARD_SIZE - 1; i++) {
		BOARD e = getEmptyMask();
		BOARD shiftMask = (~e) & (e >> TILE_BITS) & notLastColumnMask();
		if (shiftMask != 0) {
			// Shift an extra column.
			shiftMask |= (shiftMask >> TILE_BITS) & notLastColumnMask();
			// Get tiles to shift.
			BOARD tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles << TILE_BITS);
		} else {
			break;
		}
	}
}

void Board::moveUp() {
	// Merge columns from top to bottom.
	BOARD oldRows = 0;
	for (BOARD row = firstRowMask(); row != lastColumnMask(); oldRows |= row, row <<= ROW_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask() | (board ^ (board >> ROW_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldRows) != 0) {
			m = getEmptyMask(m) & row;
			if (m) {
				// Merge tiles
				board = (board + (m & unityMask())) & ~(m << ROW_BITS);
			}
		} else {
			break;
		}
	}

	// Shift tiles upward.
	for (int i = 0; i < BOARD_SIZE - 1; i++) {
		BOARD e = getEmptyMask();
		BOARD shiftMask = (~e) & (e << ROW_BITS) & BOARD_MASK;
		if (shiftMask != 0) {
			// Shift an extra row.
			shiftMask |= (shiftMask << ROW_BITS) & BOARD_MASK;
			// Get tiles to shift.
			BOARD tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles >> ROW_BITS);
		} else {
			break;
		}
	}
}

void Board::moveDown() {
	// Merge columns from bottom to top.
	BOARD oldRows = 0;
	for (BOARD row = lastRowMask(); row != firstColumnMask(); oldRows |= row, row >>= ROW_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask() | (board ^ (board << ROW_BITS));
		// Only continue if the board has mergable tiles.
		if (hasEmptyTile(m | oldRows) != 0) {
			m = getEmptyMask(m) & row;
			if (m) {
				// Merge tiles
				board = (board + (m & unityMask())) & ~(m >> ROW_BITS);
			}
		} else {
			break;
		}
	}

	// Shift tiles downward.
	for (int i = 0; i < BOARD_SIZE - 1; i++) {
		BOARD e = getEmptyMask();
		BOARD shiftMask = (~e) & (e >> ROW_BITS) & BOARD_MASK;
		if (shiftMask != 0) {
			// Shift an extra row.
			shiftMask |= (shiftMask >> ROW_BITS) & BOARD_MASK;
			// Get tiles to shift.
			BOARD tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles << ROW_BITS);
		} else {
			break;
		}
	}
}

BOARD Board::getEvenColumnsEmptyMask(BOARD b) {
	// Board is negated, such that empty tiles are 0xF. Odd columns are removed.
	b = (~b) & evenColumnMask();
	// Even columns are incremented. Only the empty tiles produce a carry.
	b = (b + (unityMask() & evenColumnMask())) & oddColumnMask();
	// Subtracting the carry that is shifted one tile to the right produced the
	// desired result: empty tiles have high bits.
	return b - (b >> TILE_BITS);
}

BOARD Board::getEmptyMask() {
	return getEmptyMask(board);
}

BOARD Board::getEmptyMask(BOARD b) {
	return getEvenColumnsEmptyMask(b) |
		(getEvenColumnsEmptyMask(b >> TILE_BITS) << TILE_BITS);
}

/// <returns>true if b has an empty tile.</returns>
BOARD Board::hasEmptyTile(BOARD b) {
	return (b - unityMask()) & (~b) & msbMask();
}