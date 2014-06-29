// Solver2048.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include "board.h"
#include "engine.h"
#include "searchnode.h"
#include "bitmath.h"

using namespace std;

void printBoard(Board board) {
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
			Tile logTile = BoardLogic::getTile(board, x, y);
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
	Tile tile = 0;
	cout << "Enter <row> <col> <val> (0 0 2 sets top-left to 2): ";
	cin >> x;
	cin >> y;
	cin >> tile;
	tile = BitMath::log2(tile);
	BoardLogic::setTile(board, x, y, tile);
}

void playSimpleStrategy() {
	Board b;
	Engine e;

	uint64_t totalSum = 0;
	uint64_t attempts = 0;
	uint64_t maxTotal = 0;
	uint64_t maxTile = 0;

	while (true) {

		bool hasPossibleMove = true;
		BoardLogic::clearBoard(b);

		while (hasPossibleMove) {
			e.setRandomTile(b);

			// Do move
			char moves = BoardLogic::getValidMoves(b);
			if (moves & Move::Down) {
				b = BoardLogic::performMove(b, Move::Down);
			} else if (moves & Move::Left) {
				b = BoardLogic::performMove(b, Move::Left);
			} else if (moves & Move::Right) {
				b = BoardLogic::performMove(b, Move::Right);
			} else if (moves & Move::Up) {
				b = BoardLogic::performMove(b, Move::Up);
			} else {
				// Game over!
				hasPossibleMove = false;
			}
		}

		attempts++;

		uint64_t sum = 0;
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				int v = (1 << BoardLogic::getTile(b, x, y));
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

	b = 0x0000000102222355;
	printBoard(b);
	SearchNode sn;
	sn.generateChildren(b);

	cout << endl;
	for (int i = 0; i < 4; i++) {
		for (int v = 0; v < 2; v++) {
			for (int j = 0; j < sn.childCount[i][v]; j++) {
				ChildNode& childNode = sn.children[i][j][v];
				Board childBoard = childNode.board;
				cout << "Value: " << (1 << (v + 1)) << "\tPositions: " << (int)childNode.positions[0];
				for (int k = 1; k < 4 && childNode.positions[k] > 0; k++) {
					cout << ", " << (int)childNode.positions[k];
				}
				const char* moveNames[4] = { "Up", "Right", "Down", "Left" };
				cout << "\tMove: " << moveNames[i] << endl;
				printBoard(childBoard);
			}
		}
	}

	getchar();

	BoardLogic::setTile(b, 0, 0, 6);
	BoardLogic::setTile(b, 0, 1, 4);
	BoardLogic::setTile(b, 0, 2, 3);
	BoardLogic::setTile(b, 0, 3, 2);
	BoardLogic::setTile(b, 1, 0, 4);
	BoardLogic::setTile(b, 2, 0, 3);
	BoardLogic::setTile(b, 2, 1, 2);
	BoardLogic::setTile(b, 2, 2, 1);
	BoardLogic::setTile(b, 3, 0, 2);
	printBoard(b);

	uint64_t totalSum = 0;
	uint64_t attempts = 0;
	uint64_t maxTotal = 0;
	uint64_t maxTile = 0;
	
	// Set two random tiles.
	e.setRandomTile(b);
	e.setRandomTile(b);
	printBoard(b);

	while (BoardLogic::getValidMoves(b) != (Move)0) {
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

		cout << "\t" << b << endl;
		b = BoardLogic::performMove(b, bestMove);
		e.setRandomTile(b);

		cout << b << endl;
		printBoard(b);
	}

	getchar();

	return 0;
}

