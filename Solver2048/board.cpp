#include "board.h"

/// <summary>
/// Determines which moves are possible
/// </summary>
/// <returns>Bit flags of moves that are possible</returns>
unsigned char Board::getValidMoves(BOARD board) {
	char validMoves = 0;

	// Board where non-empty tiles are 0 and empty tile bits are high.
	BOARD e = getEmptyMask(board);

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
		BOARD eq = MASK_COL_LAST | e | (board ^ (board >> TILE_BITS));
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

BOARD Board::performMove(BOARD board, Move move) {
	switch (move) {
		case Left: return moveLeft(board);
		case Right: return moveRight(board);
		case Up: return moveUp(board);
		case Down: return moveDown(board);
	}
	return (BOARD)board;
}

BOARD Board::moveLeft(BOARD board) {
	board = shiftLeft(board);

	// Merge columns from left to right.
	BOARD oldColumns = 0;
	for (BOARD col = MASK_COL_FIRST; col != MASK_COL_LAST; oldColumns |= col, col <<= TILE_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask(board) | (board ^ (board >> TILE_BITS));
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
}

BOARD Board::moveRight(BOARD board) {
	board = shiftRight(board);

	// Merge columns from right to left.
	BOARD oldColumns = 0;
	for (BOARD col = MASK_COL_LAST; col != MASK_COL_FIRST; oldColumns |= col, col >>= TILE_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask(board) | (board ^ (board << TILE_BITS));
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
}

BOARD Board::moveUp(BOARD board) {
	board = shiftUp(board);

	// Merge columns from top to bottom.
	BOARD oldRows = 0;
	for (BOARD row = MASK_ROW_FIRST; row != MASK_COL_LAST; oldRows |= row, row <<= ROW_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask(board) | (board ^ (board >> ROW_BITS));
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

BOARD Board::moveDown(BOARD board) {
	board = shiftDown(board);

	// Merge columns from bottom to top.
	BOARD oldRows = 0;
	for (BOARD row = MASK_ROW_LAST; row != MASK_COL_FIRST; oldRows |= row, row >>= ROW_BITS) {
		// Calculate mask of tiles to merge.
		BOARD m = getEmptyMask(board) | (board ^ (board << ROW_BITS));
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

BOARD Board::shiftLeft(BOARD board) {
	// Shift tiles to the left.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		BOARD e = getEmptyMask(board);
		BOARD shiftMask = (~e) & (e << TILE_BITS) & MASK_COL_NOT_FIRST;
		if (shiftMask != 0) {
			// Shift an extra column.
			shiftMask |= (shiftMask << TILE_BITS) & MASK_COL_NOT_FIRST;
			// Get tiles to shift.
			BOARD tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles >> TILE_BITS);
		} else {
			break;
		}
	}

	return board;
}

BOARD Board::shiftRight(BOARD board) {
	// Shift tiles to the right.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		BOARD e = getEmptyMask(board);
		BOARD shiftMask = (~e) & (e >> TILE_BITS) & MASK_COL_NOT_LAST;
		if (shiftMask != 0) {
			// Shift an extra column.
			shiftMask |= (shiftMask >> TILE_BITS) & MASK_COL_NOT_LAST;
			// Get tiles to shift.
			BOARD tiles = board & shiftMask;
			board = (board ^ tiles) | (tiles << TILE_BITS);
		} else {
			break;
		}
	}

	return board;
}

BOARD Board::shiftUp(BOARD board) {
	// Shift tiles upward.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		BOARD e = getEmptyMask(board);
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

	return board;
}

BOARD Board::shiftDown(BOARD board) {
	// Shift tiles downward.
	for (int i = 0; i < BOARD_SIZE - 2; i++) {
		BOARD e = getEmptyMask(board);
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

	return board;
}

BOARD Board::getEvenColumnsEmptyMask(BOARD b) {
	// Board is negated, such that empty tiles are 0xF. Odd columns are removed.
	b = (~b) & MASK_COL_EVEN;
	// Even columns are incremented. Only the empty tiles produce a carry.
	b = (b + (MASK_TILES_LSB & MASK_COL_EVEN)) & MASK_COL_ODD;
	// Subtracting the carry that is shifted one tile to the right produced the
	// desired result: empty tiles have high bits.
	return b - (b >> TILE_BITS);
}

BOARD Board::getEmptyMask(BOARD b) {
	return getEvenColumnsEmptyMask(b) |
		(getEvenColumnsEmptyMask(b >> TILE_BITS) << TILE_BITS);
}

/// <returns>true if b has an empty tile.</returns>
BOARD Board::hasEmptyTile(BOARD b) {
	return (b - MASK_TILES_LSB) & (~b) & MASK_TILES_MSB;
}