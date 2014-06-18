// Solver2048.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include "board.h"
#include "engine.h"
#include "bitmath.h"
#include "FastRand.h"

using namespace std;

void printBoard(Board& board) {
	char filler = '#';
	int tileWidth = 7;

	string outerBorder = string(tileWidth * BOARD_SIZE + 1, filler) + '\n';
	string innerBorder = string(1, filler);
	for (int i = 0; i < BOARD_SIZE; i++)
		innerBorder += string(tileWidth - 1, ' ') + filler;
	innerBorder += '\n';

	for (int y = 0; y < BOARD_SIZE; y++) {
		cout << outerBorder;
		cout << innerBorder;
		cout << filler;
		for (int x = 0; x < BOARD_SIZE; x++) {
			cout.width(tileWidth - 2);
			int logTile = board.getTile(x, y);
			if (logTile > 0)
				cout << (1 << logTile);
			else
				cout << ' ';
			cout << ' ' << filler;
		}
		cout << endl;
		cout << innerBorder;
	}
	cout << outerBorder;
}

void askTile(Board& board) {
	int x = 0;
	int y = 0;
	int tile = 0;
	cout << "Enter <row> <col> <val> (0 0 2 sets top-left to 2): ";
	cin >> x;
	cin >> y;
	cin >> tile;
	tile = log2(tile);
	board.setTile(x, y, tile);
}

void initRng(fastrand& fr) {
	uint32_t prngSeed[8];
	uint16_t *sptr = (uint16_t *)prngSeed;

	//
	// Randomize the seed values

	for (uint8_t i = 0; i < 8; i++) {
		prngSeed[i] = rand();
	}

	//
	// Initialize the PRNG

	InitFastRand(sptr[0], sptr[1],
				 sptr[2], sptr[3],
				 sptr[4], sptr[5],
				 sptr[6], sptr[7],
				 sptr[8], sptr[9],
				 sptr[10], sptr[11],
				 sptr[12], sptr[13],
				 sptr[14], sptr[15],
				 &fr);

}

int main(int argc, char* argv[]) {
	fastrand fr;
	Board b;
	initRng(fr);

	uint64_t totalSum = 0;
	uint64_t attempts = 0;
	uint64_t maxTotal = 0;
	uint64_t maxTile = 0;

	// Tile in the upper-left corner
	for (int i = 1; i <= TILE_MAX; i++) {
		b.clearBoard();
		b.setTile(0, 0, i);
		b.setTile(1, 0, i);
		b.getValidMoves();
	}


	while (true) {

		bool hasPossibleMove = true;
		b.clearBoard();

		while (hasPossibleMove) {
			// Get random numbers
			FastRand_SSE(&fr);

			// Get random empty tile.
			BOARD emptyMask = b.getEmptyMask();
			int emptyCount = popCount(emptyMask) / TILE_BITS;
			int randomEmptyTileNumber = fr.res[0] % emptyCount;
			int randomEmptyTilePosition = 0;
			while (true) {
				if (emptyMask & (TILE_MASK << randomEmptyTilePosition)) {
					if (randomEmptyTileNumber-- == 0)
						break;
				}
				randomEmptyTilePosition += TILE_BITS;
			}

			// Get random tile value with P(2|0.9) and P(4|0.1).
			// 0xe6666666 = 0.9 * 2^32.
			TILE randomTileValue = (fr.res[1] < 0xe6666666) ? 2 : 4;
			b.setTile(randomEmptyTilePosition, randomTileValue);

			// Do move
			char moves = b.getValidMoves();
			if (moves & Move::Down) {
				b.performMove(Move::Down);
			} else if (moves & Move::Left) {
				b.performMove(Move::Left);
			} else if (moves & Move::Right) {
				b.performMove(Move::Right);
			} else if (moves & Move::Up) {
				b.performMove(Move::Up);
			} else {
				// Game over!
				hasPossibleMove = false;
			}
		}

		attempts++;

		uint64_t sum = 0;
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				int v = (1 << b.getTile(x, y));
				if (v > maxTile) {
					maxTile = v;
					cout << maxTile << "\t" << attempts << std::endl;
				}
				sum += v;
			}
		}
	}

	return 0;
}

