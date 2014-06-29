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
		Board b;
		Board result;

		b.performMove(Move::Left);
		Assert::IsTrue(b.getBoard() == (BOARD)0);

		// Merge upper-left tile (nothing to do)
		b.clearBoard();
		b.setTile(0, 0, 2);
		result.setBoard(b);
		b.performMove(Move::Left);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge left column (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(0, i, 2);
		}
		result.setBoard(b);
		b.performMove(Move::Left);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge row 1212 (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(i, 1, 1 + (i % 2));
		}
		result.setBoard(b);
		b.performMove(Move::Left);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge with empty tiles inbetween
		b.clearBoard();
		result.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(0, i, i+1);
			b.setTile(BOARD_SIZE-1, i, i + 1);
			result.setTile(0, i, i + 2);
		}
		b.performMove(Move::Left);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge a full board
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				b.setTile(x, y, TILE_MAX-1);
				if (x < BOARD_SIZE / 2) {
					result.setTile(x, y, TILE_MAX);
				}
			}
		}
		b.performMove(Move::Left);
		Assert::IsTrue(b.getBoard() == result.getBoard());

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x >= y) {
					b.setTile(x, y, 1);
				}
			}
		}
		result.setTile(0, 0, 2);
		result.setTile(1, 0, 2);
		result.setTile(0, 1, 2);
		result.setTile(1, 1, 1);
		result.setTile(0, 2, 2);
		result.setTile(1, 2, 0);
		result.setTile(0, 3, 1);
		b.performMove(Move::Left);
		Assert::IsTrue(b.getBoard() == result.getBoard());
#endif

	}

	/// <summary>
	/// Test performing a right move.
	/// </summary>
	TEST_METHOD(PerformMoveRight) {
		Board b;
		Board result;

		b.performMove(Move::Right);
		Assert::IsTrue(b.getBoard() == (BOARD)0);

		// Merge upper-right tile (nothing to do)
		b.clearBoard();
		b.setTile(BOARD_SIZE-1, 0, 2);
		result.setBoard(b);
		b.performMove(Move::Right);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge right column (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(BOARD_SIZE-1, i, 2);
		}
		result.setBoard(b);
		b.performMove(Move::Right);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge row 1212 (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(i, 1, 1 + (i % 2));
		}
		result.setBoard(b);
		b.performMove(Move::Right);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge with empty tiles inbetween
		b.clearBoard();
		result.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(0, i, i + 1);
			b.setTile(BOARD_SIZE - 1, i, i + 1);
			result.setTile(BOARD_SIZE-1, i, i + 2);
		}
		b.performMove(Move::Right);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge a full board
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				b.setTile(x, y, TILE_MAX - 1);
				if (x >= BOARD_SIZE / 2) {
					result.setTile(x, y, TILE_MAX);
				}
			}
		}
		b.performMove(Move::Right);
		Assert::IsTrue(b.getBoard() == result.getBoard());

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x <= y) {
					b.setTile(x, y, 1);
				}
			}
		}
		result.setTile(3, 0, 1);
		result.setTile(2, 1, 0);
		result.setTile(3, 1, 2);
		result.setTile(2, 2, 1);
		result.setTile(3, 2, 2);
		result.setTile(2, 3, 2);
		result.setTile(3, 3, 2);
		b.performMove(Move::Right);
		Assert::IsTrue(b.getBoard() == result.getBoard());
#endif
	}

	/// <summary>
	/// Test performing an upward move.
	/// </summary>
	TEST_METHOD(PerformMoveUp) {
		Board b;
		Board result;

		b.performMove(Move::Up);
		Assert::IsTrue(b.getBoard() == (BOARD)0);

		// Merge upper-left tile (nothing to do)
		b.clearBoard();
		b.setTile(0, 0, 2);
		result.setBoard(b);
		b.performMove(Move::Up);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge first row (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(i, 0, 2);
		}
		result.setBoard(b);
		b.performMove(Move::Up);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge column 1212 (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(1, i, 1 + (i % 2));
		}
		result.setBoard(b);
		b.performMove(Move::Up);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge with empty tiles inbetween
		b.clearBoard();
		result.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(i, 0, i + 1);
			b.setTile(i, BOARD_SIZE - 1, i + 1);
			result.setTile(i, 0, i + 2);
		}
		b.performMove(Move::Up);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge a full board
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				b.setTile(x, y, TILE_MAX - 1);
				if (y < BOARD_SIZE / 2) {
					result.setTile(x, y, TILE_MAX);
				}
			}
		}
		b.performMove(Move::Up);
		Assert::IsTrue(b.getBoard() == result.getBoard());

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x <= y) {
					b.setTile(x, y, 1);
				}
			}
		}
		result.setTile(3, 0, 1);
		result.setTile(2, 0, 2);
		result.setTile(2, 1, 0);
		result.setTile(1, 0, 2);
		result.setTile(1, 1, 1);
		result.setTile(0, 0, 2);
		result.setTile(0, 1, 2);
		b.performMove(Move::Up);
		Assert::IsTrue(b.getBoard() == result.getBoard());
#endif
	}

	/// <summary>
	/// Test performing an downward move.
	/// </summary>
	TEST_METHOD(PerformMoveDown) {
		Board b;
		Board result;

		b.performMove(Move::Down);
		Assert::IsTrue(b.getBoard() == (BOARD)0);

		// Merge lower-left tile (nothing to do)
		b.clearBoard();
		b.setTile(0, BOARD_SIZE-1, 2);
		result.setBoard(b);
		b.performMove(Move::Down);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge last row (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(i, BOARD_SIZE-1, 2);
		}
		result.setBoard(b);
		b.performMove(Move::Down);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge column 1212 (nothing to do)
		b.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(1, i, 1 + (i % 2));
		}
		result.setBoard(b);
		b.performMove(Move::Down);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge with empty tiles inbetween
		b.clearBoard();
		result.clearBoard();
		for (int i = 0; i < BOARD_SIZE; i++) {
			b.setTile(i, 0, i + 1);
			b.setTile(i, BOARD_SIZE - 1, i + 1);
			result.setTile(i, BOARD_SIZE - 1, i + 2);
		}
		b.performMove(Move::Down);
		Assert::IsTrue(b.getBoard() == result.getBoard());

		// Merge a full board
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				b.setTile(x, y, TILE_MAX - 1);
				if (y >= BOARD_SIZE / 2) {
					result.setTile(x, y, TILE_MAX);
				}
			}
		}
		b.performMove(Move::Down);
		Assert::IsTrue(b.getBoard() == result.getBoard());

#if BOARD_SIZE == 4
		// Merge board where each row contains one less tile.
		b.clearBoard();
		result.clearBoard();
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (x >= y) {
					b.setTile(x, y, 1);
				}
			}
		}
		result.setTile(0, 3, 1);
		result.setTile(1, 2, 0);
		result.setTile(1, 3, 2);
		result.setTile(2, 2, 1);
		result.setTile(2, 3, 2);
		result.setTile(3, 2, 2);
		result.setTile(3, 3, 2);
		b.performMove(Move::Down);
		Assert::IsTrue(b.getBoard() == result.getBoard());
#endif
	}

	};
}