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
			BOARD b = 0;

			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)0);

			Board::setTile(b, 0, 0, 2);
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Right | Move::Down));

			Board::clearBoard(b);
			Board::setTile(b, 0, BOARD_SIZE-1, 4);
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Right | Move::Up));

			Board::clearBoard(b);
			Board::setTile(b, BOARD_SIZE-1, 0, 4);
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Left | Move::Down));

			Board::clearBoard(b);
			Board::setTile(b, BOARD_SIZE-1, BOARD_SIZE-1, 1);
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Left | Move::Up));

			Board::clearBoard(b);
			Board::setTile(b, 1, 1, 1);
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Left | Move::Up | Move::Right | Move::Down));

			Board::clearBoard(b);
			for (int i = 0; i < BOARD_SIZE; i++) {
				Board::setTile(b, 0, i, i + 1);
			}
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Right));

			Board::clearBoard(b);
			for (int i = 0; i < BOARD_SIZE; i++) {
				Board::setTile(b, i, 0, i + 1);
			}
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Down));
		}

		 //<summary>
		 //Tests whether valid moves exist due to neighboring tiles.
		 //</summary>
		TEST_METHOD(ValidMovesFromEqualTiles) {
			BOARD b;
			// Tile in the upper-left corner
			for (int i = 1; i <= TILE_MAX; i++) {
				Board::clearBoard(b);
				Board::setTile(b, 0, 0, i);
				Board::setTile(b, 1, 0, i);
				Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Left | Move::Right | Move::Down));
			}

			// Two tiles that lie horizontally next to each other: left and right must be possible.
			for (int i = 0; i < BOARD_SIZE; i++) {
				Board::clearBoard(b);
				Board::setTile(b, BOARD_SIZE-2, i, 1);
				Board::setTile(b, BOARD_SIZE-1, i, 1);
				Assert::IsTrue((Board::getValidMoves(b) & (Move::Right | Move::Left)) == (Move::Right | Move::Left));
			}

			// Two tiles that lie vertically next to each other: up and down must be possible.
			for (int i = 0; i < BOARD_SIZE; i++) {
				Board::clearBoard(b);
				Board::setTile(b, i, BOARD_SIZE - 2, 1);
				Board::setTile(b, i, BOARD_SIZE - 1, 1);
				Assert::IsTrue((Board::getValidMoves(b) & (Move::Down | Move::Up)) == (Move::Down | Move::Up));
			}
			// All tiles have max value
			Board::clearBoard(b);
			for (int x = 0; x < BOARD_SIZE; x++) {
				for (int y = 0; y < BOARD_SIZE; y++) {
					Board::setTile(b, x, y, TILE_MAX);
				}
			}
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)(Move::Left | Move::Up | Move::Right | Move::Down));

			// Checkerboard
			Board::clearBoard(b);
			for (int x = 0; x < BOARD_SIZE; x++) {
				for (int y = 0; y < BOARD_SIZE; y++) {
					Board::setTile(b, x, y, 1 + ((x + y) % 2));
				}
			}
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)0);

			// Increasing tiles
			Board::clearBoard(b);
			for (int x = 0; x < BOARD_SIZE; x++) {
				for (int y = 0; y < BOARD_SIZE; y++) {
					BOARD v = 1 + ((x + (y*BOARD_SIZE)) % TILE_MAX);
					Board::setTile(b, x, y, v);
				}
			}
			Assert::AreEqual(Board::getValidMoves(b), (unsigned char)0);
		}

	};
}