#include "CppUnitTest.h"

#include "../Solver2048/board.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Solver2048UnitTest {
	TEST_CLASS(PerformMoves) {
public:

	TEST_METHOD(PerformMoveLeftOnEmptyBoard) {
		Board b = 0;
		Board result = 0;
		b = BoardLogic::performMove(b, Move::Left);
		Assert::IsTrue(b == (Board)0);
	}

	TEST_METHOD(PerformMoveLeftOnSingleTile) {
		// Merge upper-left tile (nothing to do)
		Board b = 0;
		Board result = 0;
		BoardLogic::clearBoard(b);
		BoardLogic::setTile(b, 0, 0, 2);
		result = b;
		b = BoardLogic::performMove(b, Move::Left);
		Assert::IsTrue(b == result);
	}

	TEST_METHOD(PerformMoveLeftOnColumn) {
		// Merge left column (nothing to do)
		Board b = 0;
		Board result = 0;
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, 0, i, 2);
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Left);
		Assert::IsTrue(b == result);
	}

	TEST_METHOD(PerformMoveLeftOnRow) {
		// Merge row 1212 (nothing to do)
		Board b = 0;
		Board result = 0;
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, i, 1, 1 + (i % 2));
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Left);
		Assert::IsTrue(b == result);
	}

	TEST_METHOD(PerformMoveLeftWithEmptyTiles) {
		// Merge with empty tiles inbetween
		Board b = 0;
		Board result = 0;
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, 0, i, i+1);
			BoardLogic::setTile(b, BOARD_SIZE-1, i, i + 1);
			BoardLogic::setTile(result, 0, i, i + 2);
		}
		b = BoardLogic::performMove(b, Move::Left);
		Assert::IsTrue(b == result);
	}

	TEST_METHOD(PerformMoveLeftOnFullBoard) {
		// Merge a full board
		Board b = 0;
		Board result = 0;
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				BoardLogic::setTile(b, x, y, TILE_MAX-1);
				if (x < BOARD_SIZE / 2) {
					BoardLogic::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = BoardLogic::performMove(b, Move::Left);
		Assert::IsTrue(b == result);
	}

	TEST_METHOD(PerformMoveLeftWithVariedTileCount) {
#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		Board b = 0;
		Board result = 0;
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x >= y) {
					BoardLogic::setTile(b, x, y, 1);
				}
			}
		}
		BoardLogic::setTile(result, 0, 0, 2);
		BoardLogic::setTile(result, 1, 0, 2);
		BoardLogic::setTile(result, 0, 1, 2);
		BoardLogic::setTile(result, 1, 1, 1);
		BoardLogic::setTile(result, 0, 2, 2);
		BoardLogic::setTile(result, 1, 2, 0);
		BoardLogic::setTile(result, 0, 3, 1);
		b = BoardLogic::performMove(b, Move::Left);
		Assert::IsTrue(b == result);
#endif
	}

	/// <summary>
	/// Test performing a right move.
	/// </summary>
	TEST_METHOD(PerformMoveRight) {
		Board b = 0;
		Board result = 0;

		b = BoardLogic::performMove(b, Move::Right);
		Assert::IsTrue(b == (Board)0);

		// Merge upper-right tile (nothing to do)
		BoardLogic::clearBoard(b);
		BoardLogic::setTile(b, BOARD_SIZE-1, 0, 2);
		result = b;
		b = BoardLogic::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge right column (nothing to do)
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, BOARD_SIZE-1, i, 2);
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge row 1212 (nothing to do)
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, i, 1, 1 + (i % 2));
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge with empty tiles inbetween
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, 0, i, i + 1);
			BoardLogic::setTile(b, BOARD_SIZE - 1, i, i + 1);
			BoardLogic::setTile(result, BOARD_SIZE-1, i, i + 2);
		}
		b = BoardLogic::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge a full board
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				BoardLogic::setTile(b, x, y, TILE_MAX - 1);
				if (x >= BOARD_SIZE / 2) {
					BoardLogic::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = BoardLogic::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x <= y) {
					BoardLogic::setTile(b, x, y, 1);
				}
			}
		}
		BoardLogic::setTile(result, 3, 0, 1);
		BoardLogic::setTile(result, 2, 1, 0);
		BoardLogic::setTile(result, 3, 1, 2);
		BoardLogic::setTile(result, 2, 2, 1);
		BoardLogic::setTile(result, 3, 2, 2);
		BoardLogic::setTile(result, 2, 3, 2);
		BoardLogic::setTile(result, 3, 3, 2);
		b = BoardLogic::performMove(b, Move::Right);
		Assert::IsTrue(b == result);
#endif
	}

	/// <summary>
	/// Test performing an upward move.
	/// </summary>
	TEST_METHOD(PerformMoveUp) {
		Board b = 0;
		Board result = 0;

		b = BoardLogic::performMove(b, Move::Up);
		Assert::IsTrue(b == (Board)0);

		// Merge upper-left tile (nothing to do)
		BoardLogic::clearBoard(b);
		BoardLogic::setTile(b, 0, 0, 2);
		result = b;
		b = BoardLogic::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge first row (nothing to do)
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, i, 0, 2);
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge column 1212 (nothing to do)
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, 1, i, 1 + (i % 2));
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge with empty tiles inbetween
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, i, 0, i + 1);
			BoardLogic::setTile(b, i, BOARD_SIZE - 1, i + 1);
			BoardLogic::setTile(result, i, 0, i + 2);
		}
		b = BoardLogic::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge a full board
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				BoardLogic::setTile(b, x, y, TILE_MAX - 1);
				if (y < BOARD_SIZE / 2) {
					BoardLogic::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = BoardLogic::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x <= y) {
					BoardLogic::setTile(b, x, y, 1);
				}
			}
		}
		BoardLogic::setTile(result, 3, 0, 1);
		BoardLogic::setTile(result, 2, 0, 2);
		BoardLogic::setTile(result, 2, 1, 0);
		BoardLogic::setTile(result, 1, 0, 2);
		BoardLogic::setTile(result, 1, 1, 1);
		BoardLogic::setTile(result, 0, 0, 2);
		BoardLogic::setTile(result, 0, 1, 2);
		b = BoardLogic::performMove(b, Move::Up);
		Assert::IsTrue(b == result);
#endif
	}

	/// <summary>
	/// Test performing an downward move.
	/// </summary>
	TEST_METHOD(PerformMoveDown) {
		Board b = 0;
		Board result = 0;

		b = BoardLogic::performMove(b, Move::Down);
		Assert::IsTrue(b == (Board)0);

		// Merge lower-left tile (nothing to do)
		BoardLogic::clearBoard(b);
		BoardLogic::setTile(b, 0, BOARD_SIZE-1, 2);
		result = b;
		b = BoardLogic::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge last row (nothing to do)
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, i, BOARD_SIZE-1, 2);
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge column 1212 (nothing to do)
		BoardLogic::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, 1, i, 1 + (i % 2));
		}
		result = b;
		b = BoardLogic::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge with empty tiles inbetween
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			BoardLogic::setTile(b, i, 0, i + 1);
			BoardLogic::setTile(b, i, BOARD_SIZE - 1, i + 1);
			BoardLogic::setTile(result, i, BOARD_SIZE - 1, i + 2);
		}
		b = BoardLogic::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge a full board
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				BoardLogic::setTile(b, x, y, TILE_MAX - 1);
				if (y >= BOARD_SIZE / 2) {
					BoardLogic::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = BoardLogic::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		BoardLogic::clearBoard(b);
		BoardLogic::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x >= y) {
					BoardLogic::setTile(b, x, y, 1);
				}
			}
		}
		BoardLogic::setTile(result, 0, 3, 1);
		BoardLogic::setTile(result, 1, 2, 0);
		BoardLogic::setTile(result, 1, 3, 2);
		BoardLogic::setTile(result, 2, 2, 1);
		BoardLogic::setTile(result, 2, 3, 2);
		BoardLogic::setTile(result, 3, 2, 2);
		BoardLogic::setTile(result, 3, 3, 2);
		b = BoardLogic::performMove(b, Move::Down);
		Assert::IsTrue(b == result);
#endif
	}

	};
}