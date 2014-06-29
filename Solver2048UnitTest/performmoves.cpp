#include "CppUnitTest.h"

#include "../Solver2048/board.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Solver2048UnitTest {
	TEST_CLASS(PerformMoves) {
public:

	/// <summary>
	/// Test performing a left move.
	/// </summary>
	TEST_METHOD(PerformMoveLeft) {
		BOARD b = 0;
		BOARD result = 0;

		b = Board::performMove(b, Move::Left);
		Assert::IsTrue(b == (BOARD)0);

		// Merge upper-left tile (nothing to do)
		Board::clearBoard(b);
		Board::setTile(b, 0, 0, 2);
		result = b;
		b = Board::performMove(b, Move::Left);
		Assert::IsTrue(b == result);

		// Merge left column (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, 0, i, 2);
		}
		result = b;
		b = Board::performMove(b, Move::Left);
		Assert::IsTrue(b == result);

		// Merge row 1212 (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, i, 1, 1 + (i % 2));
		}
		result = b;
		b = Board::performMove(b, Move::Left);
		Assert::IsTrue(b == result);

		// Merge with empty tiles inbetween
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, 0, i, i+1);
			Board::setTile(b, BOARD_SIZE-1, i, i + 1);
			Board::setTile(result, 0, i, i + 2);
		}
		b = Board::performMove(b, Move::Left);
		Assert::IsTrue(b == result);

		// Merge a full board
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				Board::setTile(b, x, y, TILE_MAX-1);
				if (x < BOARD_SIZE / 2) {
					Board::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = Board::performMove(b, Move::Left);
		Assert::IsTrue(b == result);

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x >= y) {
					Board::setTile(b, x, y, 1);
				}
			}
		}
		Board::setTile(result, 0, 0, 2);
		Board::setTile(result, 1, 0, 2);
		Board::setTile(result, 0, 1, 2);
		Board::setTile(result, 1, 1, 1);
		Board::setTile(result, 0, 2, 2);
		Board::setTile(result, 1, 2, 0);
		Board::setTile(result, 0, 3, 1);
		b = Board::performMove(b, Move::Left);
		Assert::IsTrue(b == result);
#endif

	}

	/// <summary>
	/// Test performing a right move.
	/// </summary>
	TEST_METHOD(PerformMoveRight) {
		BOARD b = 0;
		BOARD result = 0;

		b = Board::performMove(b, Move::Right);
		Assert::IsTrue(b == (BOARD)0);

		// Merge upper-right tile (nothing to do)
		Board::clearBoard(b);
		Board::setTile(b, BOARD_SIZE-1, 0, 2);
		result = b;
		b = Board::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge right column (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, BOARD_SIZE-1, i, 2);
		}
		result = b;
		b = Board::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge row 1212 (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, i, 1, 1 + (i % 2));
		}
		result = b;
		b = Board::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge with empty tiles inbetween
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, 0, i, i + 1);
			Board::setTile(b, BOARD_SIZE - 1, i, i + 1);
			Board::setTile(result, BOARD_SIZE-1, i, i + 2);
		}
		b = Board::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

		// Merge a full board
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				Board::setTile(b, x, y, TILE_MAX - 1);
				if (x >= BOARD_SIZE / 2) {
					Board::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = Board::performMove(b, Move::Right);
		Assert::IsTrue(b == result);

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x <= y) {
					Board::setTile(b, x, y, 1);
				}
			}
		}
		Board::setTile(result, 3, 0, 1);
		Board::setTile(result, 2, 1, 0);
		Board::setTile(result, 3, 1, 2);
		Board::setTile(result, 2, 2, 1);
		Board::setTile(result, 3, 2, 2);
		Board::setTile(result, 2, 3, 2);
		Board::setTile(result, 3, 3, 2);
		b = Board::performMove(b, Move::Right);
		Assert::IsTrue(b == result);
#endif
	}

	/// <summary>
	/// Test performing an upward move.
	/// </summary>
	TEST_METHOD(PerformMoveUp) {
		BOARD b = 0;
		BOARD result = 0;

		b = Board::performMove(b, Move::Up);
		Assert::IsTrue(b == (BOARD)0);

		// Merge upper-left tile (nothing to do)
		Board::clearBoard(b);
		Board::setTile(b, 0, 0, 2);
		result = b;
		b = Board::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge first row (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, i, 0, 2);
		}
		result = b;
		b = Board::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge column 1212 (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, 1, i, 1 + (i % 2));
		}
		result = b;
		b = Board::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge with empty tiles inbetween
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, i, 0, i + 1);
			Board::setTile(b, i, BOARD_SIZE - 1, i + 1);
			Board::setTile(result, i, 0, i + 2);
		}
		b = Board::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

		// Merge a full board
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				Board::setTile(b, x, y, TILE_MAX - 1);
				if (y < BOARD_SIZE / 2) {
					Board::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = Board::performMove(b, Move::Up);
		Assert::IsTrue(b == result);

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x <= y) {
					Board::setTile(b, x, y, 1);
				}
			}
		}
		Board::setTile(result, 3, 0, 1);
		Board::setTile(result, 2, 0, 2);
		Board::setTile(result, 2, 1, 0);
		Board::setTile(result, 1, 0, 2);
		Board::setTile(result, 1, 1, 1);
		Board::setTile(result, 0, 0, 2);
		Board::setTile(result, 0, 1, 2);
		b = Board::performMove(b, Move::Up);
		Assert::IsTrue(b == result);
#endif
	}

	/// <summary>
	/// Test performing an downward move.
	/// </summary>
	TEST_METHOD(PerformMoveDown) {
		BOARD b = 0;
		BOARD result = 0;

		b = Board::performMove(b, Move::Down);
		Assert::IsTrue(b == (BOARD)0);

		// Merge lower-left tile (nothing to do)
		Board::clearBoard(b);
		Board::setTile(b, 0, BOARD_SIZE-1, 2);
		result = b;
		b = Board::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge last row (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, i, BOARD_SIZE-1, 2);
		}
		result = b;
		b = Board::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge column 1212 (nothing to do)
		Board::clearBoard(b);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, 1, i, 1 + (i % 2));
		}
		result = b;
		b = Board::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge with empty tiles inbetween
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int i = 0; i < BOARD_SIZE; i++) {
			Board::setTile(b, i, 0, i + 1);
			Board::setTile(b, i, BOARD_SIZE - 1, i + 1);
			Board::setTile(result, i, BOARD_SIZE - 1, i + 2);
		}
		b = Board::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

		// Merge a full board
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				Board::setTile(b, x, y, TILE_MAX - 1);
				if (y >= BOARD_SIZE / 2) {
					Board::setTile(result, x, y, TILE_MAX);
				}
			}
		}
		b = Board::performMove(b, Move::Down);
		Assert::IsTrue(b == result);

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		Board::clearBoard(b);
		Board::clearBoard(result);
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x >= y) {
					Board::setTile(b, x, y, 1);
				}
			}
		}
		Board::setTile(result, 0, 3, 1);
		Board::setTile(result, 1, 2, 0);
		Board::setTile(result, 1, 3, 2);
		Board::setTile(result, 2, 2, 1);
		Board::setTile(result, 2, 3, 2);
		Board::setTile(result, 3, 2, 2);
		Board::setTile(result, 3, 3, 2);
		b = Board::performMove(b, Move::Down);
		Assert::IsTrue(b == result);
#endif
	}

	};
}