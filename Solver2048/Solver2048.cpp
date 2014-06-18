// Solver2048.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include "board.h"
#include "engine.h"
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
	int tile = 0;
	cout << "Enter <row> <col> <val> (0 0 2 sets top-left to 2): ";
	cin >> x;
	cin >> y;
	cin >> tile;
	tile = log2(tile);
	board.setTile(x, y, tile);
}

int main(int argc, char* argv[]) {
	Board b;
	Board result;
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
	b.performMove(Move::Down);

	printBoard(b);

	// Solve board
	Engine* engine = new Engine(b);
	engine->solve();

	int test;
	cin >> test;

	return 0;
}

