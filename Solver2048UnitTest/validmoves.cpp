#include "CppUnitTest.h"

#include "../Solver2048/board.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Solver2048UnitTest
{		
	TEST_CLASS(ValidMoves)
	{
	public:
		
		/// <summary>
		/// Tests whether valid moves exist due to empty/non-empty tiles.
		/// </summary>
		TEST_METHOD(ValidMovesFromEmptyTiles)
		{
			Board b;

			Assert::AreEqual(b.getValidMoves(), (char)0);

			b.setTile(0, 0, 2);
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Right | Move::Down));

			b.clearBoard();
			b.setTile(0, BOARD_SIZE-1, 4);
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Right | Move::Up));

			b.clearBoard();
			b.setTile(BOARD_SIZE-1, 0, 4);
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Left | Move::Down));

			b.clearBoard();
			b.setTile(BOARD_SIZE-1, BOARD_SIZE-1, 1);
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Left | Move::Up));

			b.clearBoard();
			b.setTile(1, 1, 1);
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Left | Move::Up | Move::Right | Move::Down));

			b.clearBoard();
			for (int i = 0; i < BOARD_SIZE; i++) {
				b.setTile(0, i, i + 1);
			}
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Right));

			b.clearBoard();
			for (int i = 0; i < BOARD_SIZE; i++) {
				b.setTile(i, 0, i + 1);
			}
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Down));
		}

		 //<summary>
		 //Tests whether valid moves exist due to neighboring tiles.
		 //</summary>
		TEST_METHOD(ValidMovesFromEqualTiles) {
			Board b;
			// Tile in the upper-left corner
			for (int i = 1; i <= TILE_MAX; i++) {
				b.clearBoard();
				b.setTile(0, 0, i);
				b.setTile(1, 0, i);
				Assert::AreEqual(b.getValidMoves(), (char)(Move::Left | Move::Right | Move::Down));
			}

			// Two tiles that lie horizontally next to each other: left and right must be possible.
			for (int i = 0; i < BOARD_SIZE; i++) {
				b.clearBoard();
				b.setTile(BOARD_SIZE-2, i, 1);
				b.setTile(BOARD_SIZE-1, i, 1);
				Assert::IsTrue((b.getValidMoves() & (Move::Right | Move::Left)) == (Move::Right | Move::Left));
			}

			// Two tiles that lie vertically next to each other: up and down must be possible.
			for (int i = 0; i < BOARD_SIZE; i++) {
				b.clearBoard();
				b.setTile(i, BOARD_SIZE - 2, 1);
				b.setTile(i, BOARD_SIZE - 1, 1);
				Assert::IsTrue((b.getValidMoves() & (Move::Down | Move::Up)) == (Move::Down | Move::Up));
			}
			// All tiles have max value
			b.clearBoard();
			for (int x = 0; x < BOARD_SIZE; x++) {
				for (int y = 0; y < BOARD_SIZE; y++) {
					b.setTile(x, y, TILE_MAX);
				}
			}
			Assert::AreEqual(b.getValidMoves(), (char)(Move::Left | Move::Up | Move::Right | Move::Down));

			// Checkerboard
			b.clearBoard();
			for (int x = 0; x < BOARD_SIZE; x++) {
				for (int y = 0; y < BOARD_SIZE; y++) {
					b.setTile(x, y, 1 + ((x + y) % 2));
				}
			}
			Assert::AreEqual(b.getValidMoves(), (char)0);

			// Increasing tiles
			b.clearBoard();
			for (int x = 0; x < BOARD_SIZE; x++) {
				for (int y = 0; y < BOARD_SIZE; y++) {
					BOARD v = 1 + ((x + (y*BOARD_SIZE)) % TILE_MAX);
					b.setTile(x, y, v);
				}
			}
			Assert::AreEqual(b.getValidMoves(), (char)0);
		}

	};
}