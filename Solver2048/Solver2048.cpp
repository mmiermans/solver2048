// Solver2048.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include "board.h"
#include "engine.h"
#include "searchnode.h"
#include "bitmath.h"

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
	TILE tile = 0;
	cout << "Enter <row> <col> <val> (0 0 2 sets top-left to 2): ";
	cin >> x;
	cin >> y;
	cin >> tile;
	tile = BitMath::log2(tile);
	board.setTile(x, y, tile);
}

void playSimpleStrategy() {
	Board b;
	Engine e;

	uint64_t totalSum = 0;
	uint64_t attempts = 0;
	uint64_t maxTotal = 0;
	uint64_t maxTile = 0;

	getchar();

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
			e.setRandomTile(b);

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
				if (v == 2048) {
					printBoard(b);
				}
				sum += v;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	Board b;
	Engine e;

	b.setBoard(0x0000000102222355);
	printBoard(b);
	SearchNode sn;
	sn.generateChildren(b);

	cout << endl;
	for (int i = 0; i < 4; i++) {
		for (int v = 0; v < 2; v++) {
			for (int j = 0; j < sn.childCount[i][v]; j++) {
				ChildNode& childNode = sn.children[i][j][v];
				BOARD childBoard = childNode.board;
				Board childBoardClass(childBoard);
				cout << "Value: " << (1 << (v + 1)) << "\tPositions: " << (int)childNode.positions[0];
				for (int k = 1; k < 4 && childNode.positions[k] > 0; k++) {
					cout << ", " << (int)childNode.positions[k];
				}
				const char* moveNames[4] = { "Up", "Right", "Down", "Left" };
				cout << "\tMove: " << moveNames[i] << endl;
				printBoard(childBoardClass);
			}
		}
	}

	getchar();

	b.setTile(0, 0, 6);
	b.setTile(0, 1, 4);
	b.setTile(0, 2, 3);
	b.setTile(0, 3, 2);
	b.setTile(1, 0, 4);
	b.setTile(2, 0, 3);
	b.setTile(2, 1, 2);
	b.setTile(2, 2, 1);
	b.setTile(3, 0, 2);
	printBoard(b);

	uint64_t totalSum = 0;
	uint64_t attempts = 0;
	uint64_t maxTotal = 0;
	uint64_t maxTile = 0;
	
	// Set two random tiles.
	e.setRandomTile(b);
	e.setRandomTile(b);
	printBoard(b);

	while (b.getValidMoves() != (Move)0) {
		Move bestMove = e.solve(b);

		if (bestMove & Move::Down) {
			cout << "Down";
		} else if (bestMove & Move::Left) {
			cout << "Left";
		} else if (bestMove & Move::Right) {
			cout << "Right";
		} else if (bestMove & Move::Up) {
			cout << "Up";
		}

		cout << "\t" << b.getBoard() << endl;
		b.performMove(bestMove);
		e.setRandomTile(b);

		cout << b.getBoard() << endl;
		printBoard(b);
	}

	getchar();

	return 0;
}

